
/**
 *******************************************************************************
 * @file       FlashAlgoFncs.c
 * @version    V0.3
 * @date       2010.12.02
 * @brief      Flash  Algorithm For Freescale MKLx with 128/64/32KB Flash Rom	
 *******************************************************************************
 * @copy
 *
 * INTERNAL FILE,DON'T PUBLIC.
 * 
 * <h2><center>&copy; COPYRIGHT 2009 CooCox </center></h2>
 *******************************************************************************
 */ 
 
#include <FlashAlgorithm.h>

#if defined ( KLxx_128_PRG_NO_CFG )
#define PRG_NO_CONFIG
#endif

#if defined ( KLxx_64_PRG_NO_CFG )
#define PRG_NO_CONFIG
#endif

#if defined ( KLxx_32_PRG_NO_CFG )
#define PRG_NO_CONFIG
#endif

#if defined ( KLxx_16_PRG_NO_CFG )
#define PRG_NO_CONFIG
#endif

#if defined ( KLxx_8_PRG_NO_CFG )
#define PRG_NO_CONFIG
#endif

typedef volatile unsigned long    vu32;
typedef volatile unsigned char     vu8;
typedef          unsigned long     u32;

#define M32(adr) (*((vu32 *) (adr)))

// Flash Memory Map
#define APROM_BASE        (0x00000000)

// Peripheral Memory Map
#define SIM_BASE           (0x40048044)
#define MCG_BASE           (0x40064000)
#define FTFA_BASE          (0x40020000)

#define SIM               ((SIM_TypeDef *)  SIM_BASE)
#define MCG               ((MCG_TypeDef *)  MCG_BASE)
#define FTFA              ((FTFA_TypeDef *) FTFA_BASE)


// System Manager Control Registers
typedef struct {
  vu32 CLKDIV1;               // offset 0x004 R/W System Reset Source Register
} SIM_TypeDef;

// Clock Control Registers
typedef struct {
  vu8 MCG_C1;               // offset 0x000 R/W System Power Down Control Register
  vu8 MCG_C2;               // offset 0x004 R/W AHB Devices Clock Enable Control Register
  vu8 MCG_C3;               // offset 0x008 (R/W APB Devices Clock Enable Control Register
  vu8 MCG_C4;
  vu8 MCG_C5;               // offset 0x010 R/W Clock Source Select Control Register 0
  vu8 MCG_C6;               // offset 0x014 R/W Clock Source Select Control Register 1
  vu8 MCG_S;                // offset 0x018 R/W Clock Divider Number Register

} MCG_TypeDef;

// FLASH MEMORY CONTROLLER (FTFA) Registers
typedef struct {
	vu8 FTFA_FSTAT;               // Flash Status Register (FTFA_FSTAT)
	vu8 FTFA_FCNFG;               // Flash Configuration Register (FTFA_FCNFG)
	vu8 FTFA_FSEC;                // Flash Security Register (FTFA_FSEC)
	vu8 FTFA_FOPT;                // Flash Option Register (FTFA_FOPT)
	vu8 FTFA_FCCOB3;              // Flash Common Command Object Registers
	vu8 FTFA_FCCOB2;              // Flash Common Command Object Registers
	vu8 FTFA_FCCOB1;              // Flash Common Command Object Registers
	vu8 FTFA_FCCOB0;              // Flash Common Command Object Registers
	vu8 FTFA_FCCOB7;              // Flash Common Command Object Registers
	vu8 FTFA_FCCOB6;              // Flash Common Command Object Registers
	vu8 FTFA_FCCOB5;              // Flash Common Command Object Registers
	vu8 FTFA_FCCOB4;              // Flash Common Command Object Registers
    vu8 FTFA_FCCOBB;              // Flash Common Command Object Registers
	vu8 FTFA_FCCOBA;              // Flash Common Command Object Registers
	vu8 FTFA_FCCOB9;              // Flash Common Command Object Registers
	vu8 FTFA_FCCOB8;              // Flash Common Command Object Registers
	vu8 FTFA_FPROT3;              // Program Flash Protection Registers
	vu8 FTFA_FPROT2;              // Program Flash Protection Registers
	vu8 FTFA_FPROT1;              // Program Flash Protection Registers
	vu8 FTFA_FPROT0;              // Program Flash Protection Registers
} FTFA_TypeDef;



// ISP Control Register (ISPCON) definitions
#define FTFA_CCIF                ((unsigned char)0x80)
#define FTFA_CLEAR               ((unsigned char)0x70)

// ISP Command (ISPCMD) definitions

#define FTFA_READ1S_SECTION      ((unsigned char)0x01)
#define FTFA_CHECK               ((unsigned char)0x02)
#define FTFA_READ_IFR            ((unsigned char)0x03)
#define FTFA_PROGRAM             ((unsigned char)0x06)
#define FTFA_ERASE_SECTOR        ((unsigned char)0x09)
#define FTFA_READ1S_ALL          ((unsigned char)0x40)
#define FTFA_READ_ONCE           ((unsigned char)0x41)
#define FTFA_PROGRAM_ONCE        ((unsigned char)0x43)
#define FTFA_ERASE_ALL           ((unsigned char)0x44)
#define FTFA_VERIFY_BACKDOOR     ((unsigned char)0x45)

unsigned char gFlashConfig[16] = { 0 };
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
  
  (*((volatile unsigned long *)(0x40048100))) = 0x00;

  FTFA->FTFA_FSTAT |= FTFA_CLEAR;                      // clear error flag

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
    int iErrCode;
    
#ifdef PRG_NO_CONFIG     
	int i;
    unsigned long adr;
    unsigned long sz;
    unsigned char *buf = 0;
#endif
    
#ifdef PRG_NO_CONFIG 
	if(sectorAddr == 0x400)
	{
      for(i = 0; i < 16; i++)
      {
        gFlashConfig[i] = *((unsigned char*)(sectorAddr + i));
        
      }
	}
#endif
	
	offs = 0x00000000;
	
	FTFA->FTFA_FSTAT = FTFA_CLEAR;                       // Reset Error Flags
	
	FTFA->FTFA_FCCOB0 = FTFA_ERASE_SECTOR;                // Prepare Command
	offs = ((sectorAddr) & ~0x00000003);           // Prepare Address
	FTFA->FTFA_FCCOB1 = (unsigned char)(offs >> 16);
	FTFA->FTFA_FCCOB2 = (unsigned char)(offs >> 8);
	FTFA->FTFA_FCCOB3 = (unsigned char)(offs);

	FTFA->FTFA_FSTAT |= FTFA_CCIF;                        // Exacute Command
	
	while(!(FTFA->FTFA_FSTAT & FTFA_CCIF))                // Wait until command is finished
	{
	  /* reload Watdchdog */                              // Reload IWDG
	}
	
	if(FTFA->FTFA_FSTAT & 0x70)                           // Check for Error
	{    
      FTFA->FTFA_FSTAT = FTFA_CLEAR;                     // Reset Error Flags
      iErrCode = 1;                                           // Failed
	}
	
    iErrCode = 0;
    
#ifdef PRG_NO_CONFIG 
	if(sectorAddr == 0x400)
	{
        sz = 16;
        adr = 0x400;
        buf = gFlashConfig;
        
        FTFA->FTFA_FSTAT = FTFA_CLEAR; 
        
        while (sz) 
        {
            FTFA->FTFA_FCCOB0 = FTFA_PROGRAM;                   // Prepare Command
            offs = ((adr) & ~0x00000003);                       // Prepare Address
            FTFA->FTFA_FCCOB1 = (unsigned char)(offs >> 16);
            FTFA->FTFA_FCCOB2 = (unsigned char)(offs >> 8);
            FTFA->FTFA_FCCOB3 = (unsigned char)(offs);

            FTFA->FTFA_FCCOB7 = buf[0];
            FTFA->FTFA_FCCOB6 = buf[1];
            FTFA->FTFA_FCCOB5 = buf[2];
            FTFA->FTFA_FCCOB4 = buf[3];                         // Prepare Data
            
            FTFA->FTFA_FSTAT |= FTFA_CCIF;                      // Exacute Command

            while(!(FTFA->FTFA_FSTAT & FTFA_CCIF))              // Wait until command is finished
            {
              /* reload Watdchdog */                            // Reload IWDG
            }
            
            if(FTFA->FTFA_FSTAT & 0x70)                         // Check for Error
            {
              FTFA->FTFA_FSTAT = FTFA_CLEAR;                   // Reset Error Flags
              return 1;                                         // Failed
            }
            
            adr += 4;                                           // Go to next Word
            buf += 4;
            sz  -= 4;
        }
    
	  return iErrCode;
	}
#endif

  return iErrCode;
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

	
	offs = 0x00000000;
	
	sz = (sz + 3) & ~3;                                 	// Adjust size for Words

#ifdef PRG_NO_CONFIG
    if(adr == 0x400)
    {
        if(sz <= 0x10) 
        {
            return 0;
        }
        else
        {
            adr += 0x10;
            buf += 0x10;
            sz -= 0x10;
        }        
    }

#endif
	
	FTFA->FTFA_FSTAT = FTFA_CLEAR;                        	// Reset Error Flags
	
	while (sz) 
	{
		FTFA->FTFA_FCCOB0 = FTFA_PROGRAM;                   // Prepare Command
		offs = ((adr) & ~0x00000003);                       // Prepare Address
		FTFA->FTFA_FCCOB1 = (unsigned char)(offs >> 16);
		FTFA->FTFA_FCCOB2 = (unsigned char)(offs >> 8);
		FTFA->FTFA_FCCOB3 = (unsigned char)(offs);

		FTFA->FTFA_FCCOB7 = buf[0];
		FTFA->FTFA_FCCOB6 = buf[1];
		FTFA->FTFA_FCCOB5 = buf[2];
		FTFA->FTFA_FCCOB4 = buf[3];                         // Prepare Data
		
		FTFA->FTFA_FSTAT |= FTFA_CCIF;                      // Exacute Command

		while(!(FTFA->FTFA_FSTAT & FTFA_CCIF))              // Wait until command is finished
		{
		  /* reload Watdchdog */                            // Reload IWDG
		}
		
		if(FTFA->FTFA_FSTAT & 0x70)                         // Check for Error
		{
		  FTFA->FTFA_FSTAT = FTFA_CLEAR;                   // Reset Error Flags
		  return 1;                                         // Failed
		}
		
		adr += 4;                                           // Go to next Word
		buf += 4;
		sz  -= 4;
	}

  
  return 0;
}

/**
 *******************************************************************************
 * @brief      Page Verify Function. 
 * @param[in]  verifyAddr   Verify Start Address(Usually page start address).
 * @param[in]  size			    Verify size
 * @param[in]  buf   			  Source buf point.
 * @param[out] None  
 * @retval     0   					Verify pass.
 * @retval     others       Some error occurs or verify failed..		 
 *
 * @par Description
 * @details   Optional function. When this function is absence, 
 *            the link will read flash memory directly to do verify.  
 * @note 
 *******************************************************************************
 */ 
#if 0
int FlashVerify(unsigned long verifyAddr,unsigned long size,unsigned char *buf)
{ 
  int i;
  unsigned long sz = size;
  unsigned char* pageBuf = (unsigned char*)verifyAddr;
  
#ifdef PRG_NO_CONFIG
    if(verifyAddr == 0x400)
    {
        if(sz <= 0x10) 
        {
            return 0;
        }
        else
        {
            pageBuf += 0x10;
            buf += 0x10;
            sz -= 0x10;
        }        
    }

#endif
  
		
  for(i = 0; i < sz; i++)
  {
	if(pageBuf[i] != buf[i])
	{
		return (1);
	}
  }
   
  return (0);
}
#endif

/**
 *******************************************************************************
 * @brief      Page Blank Check Function. 
 * @param[in]  checkAddr    Check Start Address(Usually page start address).
 * @param[in]  size			    Check size
 * @param[out] None  
 * @retval     0   					Check pass.
 * @retval     others       Some error occurs or check failed.		 
 *
 * @par Description
 * @details   Optional function. When this function is absence, 
 *            the link will read flash memory directly to do verify.  
 * @note      Sector can be significant.We check the page rather than the sector,
 *            having regard to the actual size of the ram.
 *******************************************************************************
 */
 #if 0
int FlashBlankCheck (unsigned long checkAddr,unsigned long size)
{
  int i;
  unsigned long sz = size;
  unsigned char* pageBuf = (unsigned char*)checkAddr;
  
#ifdef PRG_NO_CONFIG
    if(checkAddr == 0x400)
    {
        if(sz <= 0x10) 
        {
            return 0;
        }
        else
        {
            pageBuf += 0x10;
            sz -= 0x10;
        }        
    }

#endif
  
		
  for(i = 0; i < sz; i++)
  {
	if(pageBuf[i] != 0xFF)
	{
		return (1);
	}
  }
   
  return (0);
  
}
#endif

