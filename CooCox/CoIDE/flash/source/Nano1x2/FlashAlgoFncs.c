
/**
 *******************************************************************************
 * @file       FlashAlgoFncs.c
 * @version    V0.3
 * @date       2010.12.02
 * @brief      Flash  Algorithm For NANO1xx with 128/64/32KB/16KB/8KB Flash Rom	
 *******************************************************************************
 * @copy
 *
 * INTERNAL FILE,DON'T PUBLIC.
 * 
 * <h2><center>&copy; COPYRIGHT 2009 CooCox </center></h2>
 *******************************************************************************
 */ 
 
#include <FlashAlgorithm.h>

typedef volatile unsigned long    vu32;
typedef          unsigned long     u32;

#define M32(adr) (*((vu32 *) (adr)))

// Flash Memory Map
#define APROM_BASE        (0x00000000)
#define LDROM_BASE        (0x00100000)
#define CFG_BASE          (0x00300000)

// Peripheral Memory Map
#define GCR_BASE          (0x50000000)
#define CLK_BASE          (0x50000200)
#define FMC_BASE          (0x5000C000)

#define GCR               ((GCR_TypeDef *) GCR_BASE)
#define CLK               ((CLK_TypeDef *) CLK_BASE)
#define FMC               ((FMC_TypeDef *) FMC_BASE)



// System Manager Control Registers
typedef struct
{
	vu32 PDID;   
	vu32 RSTSRC;
	vu32 IPRSTC1;
	vu32 IPRSTC2;
	vu32 RESERVE0[2];
	vu32 BODCR;
	vu32 RESERVE1[2];
	vu32 PORCR;
	vu32 RESERVE2[2];
	vu32 P0_MFP;
	vu32 P1_MFP;
	vu32 P2_MFP;
	vu32 P3_MFP;
	vu32 P4_MFP;
	vu32 RESERVE3[47];
	vu32 REGLOCK;
	vu32 RESERVE4[3];
	vu32 RCADJ;
} GCR_TypeDef;

// Clock Control Registers
typedef struct {
  vu32 PWRCON;               // offset 0x000 R/W System Power Down Control Register
  vu32 AHBCLK;               // offset 0x004 R/W AHB Devices Clock Enable Control Register
  vu32 APBCLK;               // offset 0x008 (R/W APB Devices Clock Enable Control Register
  vu32 CLKSTATUS;            // offset 0x00c
  vu32 CLKSEL0;              // offset 0x010 R/W Clock Source Select Control Register 0
  vu32 CLKSEL1;              // offset 0x014 R/W Clock Source Select Control Register 1
  vu32 CLKDIV;               // offset 0x018 R/W Clock Divider Number Register
  vu32 CLKSEL2;              // offset 0X01c
  vu32 PLLCON;               // offset 0x020 R/W PLL Control Register
  vu32 FRQDIV;               // offset 0x024
} CLK_TypeDef;

// FLASH MEMORY CONTROLLER (FMC) Registers
typedef struct {
  vu32 ISPCON;               // offset 0x000 R/W ISP Control Register
  vu32 ISPADR;               // offset 0x004 R/W ISP Address Register
  vu32 ISPDAT;               // offset 0x008 R/W ISP Data Register
  vu32 ISPCMD;               // offset 0x00C R/W ISP Command Register
  vu32 ISPTRG;               // offset 0x010 R/W ISP Trigger Register
  vu32 DFBADR;               // offset 0x014 R Data Flash Start Address
  vu32 FATCON;               // offset 0x018 R/W Flash Access Window Control Register
  vu32 ICPCON;               // offset 0x01C R/W ??
  vu32 RMPCON;               // offset 0x020 R/W ??
} FMC_TypeDef;


// CLK AHBCLK ISP_EN
#define CLK_ISP_EN            ((unsigned int)0x00000040)

// ISP Control Register (ISPCON) definitions
#define FMC_ISPFF             ((unsigned int)0x00000040)
#define FMC_LDUEN             ((unsigned int)0x00000020)
#define FMC_ISPEN             ((unsigned int)0x00000001)

// ICP Control Register (ICPCON) definitions
#define FMC_ICPEN             ((unsigned int)0x00000001)

// ISP Command (ISPCMD) definitions
#define FMC_STANDBY             ((unsigned int)0x00000030)
#define FMC_READ                ((unsigned int)0x00000000)
#define FMC_PROGRAM             ((unsigned int)0x00000021)
#define FMC_PAGE_ERASE          ((unsigned int)0x00000022)
#define FMC_READ_CID            ((unsigned int)0x0000000B)
#define FMC_READ_DID            ((unsigned int)0x0000000C)

// ISP Trigger Control Register (ISPTRG) definitions
#define FMC_ISPGO               ((unsigned int)0x00000001)


#if defined  NANO1xx_128 
#define      SECTOR_NUM            256
#endif

#if defined  NANO1xx_64 
#define      SECTOR_NUM            128
#endif

#if defined  NANO1xx_32 
#define      SECTOR_NUM             64
#endif

#if defined  NANO1xx_16 
#define      SECTOR_NUM             32
#endif

#if defined  NANO1xx_8 
#define      SECTOR_NUM             16
#endif

#if defined  NANO1xx_LDROM 
#define      SECTOR_NUM             8
#endif

/**
 *******************************************************************************
 * @brief      Initialize before Flash Programming/Erase Functions 
 * @param[in]  baseAddr     Flash device base address.
 * @param[in]  clk     			Flash program clock.
 * @param[in]  operateFuc   Init for what operation
 														(FLASH_OPT_ERASECHIP/FLASH_OPT_ERASESECTORS/FLASH_OPT_PROGRAMPAGE).
 * @param[out] None  
 * @retval     0   					All is OK.
 * @retval     others       Some error occurs.		 
 *
 * @par Description
 * @details    This function is called before flash programming/erase. 
 * @note 
 *******************************************************************************
 */
int FlashInit (unsigned long baseAddr,
               unsigned long clk,
               unsigned long operateFuc)
{
  
  int delay = 100;

  GCR->REGLOCK = 0x59;                                   // Unlock System Control Registers
  GCR->REGLOCK = 0x16;                              
  GCR->REGLOCK = 0x88;                              

  if (!(GCR->REGLOCK & 0x01))
  {
    return 1;                                           // Not able to unlock 
  }

  CLK->PWRCON |= (1 << 2);                              // enable internal OSC22MHz 
  while (delay--);

  CLK->AHBCLK |= CLK_ISP_EN;                            // enable the Flash ISP controller clock
  
  FMC->ISPCON |= (FMC_ISPEN | FMC_LDUEN);               // Enable ISP function
  FMC->ICPCON |= (FMC_ICPEN);                           // Enable ICP functions ?

  if (!(FMC->ISPCON & FMC_ISPEN))
  {
    return 1;                                           // ISP not enabled 
  }

  FMC->ISPCON |= FMC_ISPFF;                             // Reset Error Flag
   
                         
  return 0;
}


/**
 *******************************************************************************
 * @brief      Un-Init after Flash Programming/Erase Functions  
 * @param[in]  operateFuc   Init for what operation
 														(FLASH_OPT_ERASECHIP/FLASH_OPT_ERASESECTORS/FLASH_OPT_PROGRAMPAGE).
 * @param[out] None  
 * @retval     0   					All is OK.
 * @retval     others       Some error occurs.		 
 *
 * @par Description
 * @details    This function is called after flash programming/erase. 
 * @note 
 *******************************************************************************
 */
int FlashUnInit (unsigned long operateFuc)
{
	return 0;
}


/**
 *******************************************************************************
 * @brief      Erase the full chip.  
 * @param[in]  None.
 * @param[out] None  
 * @retval     0   					All is OK.
 * @retval     others       Some error occurs.		 
 *
 * @par Description
 * @details     
 * @note 
 *******************************************************************************
 */
int FlashEraseChip   (void)
{
	unsigned long offs;                                   // Flash Offset
	unsigned long adr;
	int i;
	
#ifdef NANO1xx_LDROM
  offs = 0x00100000;
#else
  offs = 0x00000000;
#endif

	FMC->ISPCON |= FMC_ISPFF;                             // Reset Error Flags
	adr = 0;
	for(i = 0; i < SECTOR_NUM; i++)
	{
		
		FMC->ISPCMD = FMC_PAGE_ERASE;                       // Prepare Command
		FMC->ISPADR = ((adr + offs) & ~0x00000003);         // Prepare Address
		FMC->ISPTRG = FMC_ISPGO;                            // Exacute Command
		
		while (FMC->ISPTRG & FMC_ISPGO)                     // Wait until command is finished
		{
		/* reload Watdchdog */                              // Reload IWDG
		}
		
		if (FMC->ISPCON &  FMC_ISPFF)                       // Check for Error
		{
			FMC->ISPCON |= FMC_ISPFF;                       // Reset Error Flags
			return 1;                                       // Failed
		}
		
		adr += 512;
		
	}
	
  return 0;
}

/**
 *******************************************************************************
 * @brief      Erase the select Sector. 
 * @param[in]  sectorAddr   Sector's start address.
 * @param[out] None  
 * @retval     0   					All is OK.
 * @retval     others       Some error occurs.		 
 *
 * @par Description
 * @details     
 * @note 
 *******************************************************************************
 */
int FlashEraseSector (unsigned long sectorAddr)
{
	unsigned long offs;                                   // Flash Offset

#ifdef NANO1xx_LDROM
	offs = 0x00100000;
#else
	offs = 0x00000000;
#endif
	
	FMC->ISPCON |= FMC_ISPFF;                             // Reset Error Flags
	
	FMC->ISPCMD = FMC_PAGE_ERASE;                         // Prepare Command
	FMC->ISPADR = ((sectorAddr + offs) & ~0x00000003);    // Prepare Address
	FMC->ISPTRG = FMC_ISPGO;                              // Exacute Command
	
	while (FMC->ISPTRG & FMC_ISPGO)                       // Wait until command is finished
	{
	  /* reload Watdchdog */                              // Reload IWDG
	}
	
	if (FMC->ISPCON &  FMC_ISPFF)                         // Check for Error
	{    
      FMC->ISPCON |= FMC_ISPFF;                           // Reset Error Flags
      return 1;                                           // Failed
	}
	
  
  return 0;
}




/**
 *******************************************************************************
 * @brief      Proram a page. 
 * @param[in]  pageAddr   Page's start address.
 * @param[in]  size			  Page size
 * @param[in]  buf   			source point.
 * @param[out] None  
 * @retval     0   					All is OK.
 * @retval     others       Some error occurs.		 
 *
 * @par Description
 * @details     
 * @note 
 *******************************************************************************
 */ 
int FlashProgramPage (unsigned long pageAddr,     
                      unsigned long size,
                      unsigned char *buf)
{
	unsigned long offs;                                 	// Flash Offset
	unsigned long adr = pageAddr;
	unsigned long sz = size;
	
#ifdef NANO1xx_LDROM
	offs = 0x00100000;
#else
	offs = 0x00000000;
#endif
	
	sz = (sz + 3) & ~3;                                 	// Adjust size for Words
	
	FMC->ISPCON |= FMC_ISPFF;                           	// Reset Error Flags
	
	while (sz) 
	{
		FMC->ISPCMD = FMC_PROGRAM;                          // Prepare Command
		FMC->ISPADR = ((adr + offs) & ~0x00000003);         // Prepare Address
		FMC->ISPDAT	= *((u32 *)buf);                        // Prepare Data
		FMC->ISPTRG = FMC_ISPGO;                            // Exacute Command
		
		while (FMC->ISPTRG & FMC_ISPGO) 
		{                   																// Wait until command is finished
		  /* reload Watdchdog */                            // Reload IWDG
		}
		
		if (FMC->ISPCON &  FMC_ISPFF) 
	    {                     															// Check for Error
		    FMC->ISPCON |= FMC_ISPFF;                       // Reset Error Flags
		    return 1;                                       // Failed
		}
		
		adr += 4;                                           // Go to next Word
		buf += 4;
		sz  -= 4;
	}

  
  return 0;
}
