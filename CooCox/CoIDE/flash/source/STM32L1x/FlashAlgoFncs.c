
/**
 *******************************************************************************
 * @file       FlashAlgoFncs.c
 * @version    V0.4
 * @date       2011.08.04
 * @brief      Flash  Algorithm For STM32F10x
 *******************************************************************************
 * @copy
 *
 * INTERNAL FILE,DON'T PUBLIC.
 * 
 * <h2><center>&copy; COPYRIGHT 2009 CooCox </center></h2>
 *******************************************************************************
 */ 
 
#include <FlashAlgorithm.h>

typedef volatile unsigned char  vu8;
typedef volatile unsigned long  vu32;
typedef volatile unsigned short vu16;

#define M8(adr)  (*((vu8  *) (adr)))
#define M16(adr) (*((vu16 *) (adr)))
#define M32(adr) (*((vu32 *) (adr)))

// Peripheral Memory Map
#define IWDG_BASE       0x40003000
#define FLASH_BASE      0x40023C00

#define IWDG            ((IWDG_TypeDef *) IWDG_BASE)
#define FLASH           ((FLASH_TypeDef*) FLASH_BASE)

// Independent WATCHDOG
typedef struct {
  vu32 KR;                                      // offset  0x000 Key register (IWDG_KR)
  vu32 PR;                                      // offset  0x004 Prescaler register (IWDG_PR)
  vu32 RLR;                                     // offset  0x008 Reload register (IWDG_RLR)
  vu32 SR;                                      // offset  0x00C Status register (IWDG_SR)
} IWDG_TypeDef;

// Flash Registers
typedef struct {
  vu32 ACR;                                     // offset  0x000 Flash access control register (FLASH_ACR)
  vu32 PECR;                                    // offset  0x004 Flash program erase control register (FLASH_PECR)
  vu32 PDKEYR;                                  // offset  0x008 Flash power down key register (FLASH_PDKEYR)
  vu32 PEKEYR;                                  // offset  0x00C Flash program erase key register (FLASH_PEKEYR)
  vu32 PRGKEYR;                                 // offset  0x010 Flash program memory key register (FLASH_PRGKEYR)
  vu32 OPTKEYR;                                 // offset  0x014 Flash option key register (FLASH_OPTKEYR)
  vu32 SR;                                      // offset  0x018 Flash status register (FLASH_SR)
  vu32 OBR;                                     // offset  0x01C Option byte register (FLASH_OBR)
  vu32 WRPR;                                    // offset  0x020 Flash write protection register (FLASH_WRPR)
#ifdef STM32L1xx_384                            // High-Density device
  vu32 RESERVED[23];                            //
  vu32 WRPR1;                                   // offset  0x080 Flash write protection register (FLASH_WRPR1)
  vu32 WRPR2;                                   // offset  0x084 Flash write protection register (FLASH_WRPR2)
#endif

} FLASH_TypeDef;


// Flash Keys
#define FLASH_PEKEY1        0x89ABCDEF
#define FLASH_PEKEY2        0x02030405
#define FLASH_PRGKEY1       0x8C9DAEBF
#define FLASH_PRGKEY2       0x13141516
#define FLASH_OPTKEY1       0xFBEAD9C8
#define FLASH_OPTKEY2       0x24252627

// Flash program erase control register (FLASH_PECR) definitions
#define FLASH_PELOCK        0x00000001          // FLASH_PECR and data memory lock
#define FLASH_PRGLOCK       0x00000002          // Program memory lock
#define FLASH_OPTLOCK       0x00000004          // Option bytes block lock
#define FLASH_PROG          0x00000008          // Program memory selection
#define FLASH_DATA          0x00000010          // Data memory selection
#define FLASH_OPT           0x00000020          // Option Bytes memory selection
#define FLASH_FIX           0x00000100          // Fixed time data write for Byte, Half Word and Word programming
#define FLASH_ERASE         0x00000200          // Page or Double Word erase mode
#define FLASH_FPRG          0x00000400          // Half Page/Double Word programming mode
#define FLASH_GBHF_ER       0x00000800          // Global Half Erase mode

// Flash status register (FLASH_SR) definitions
#define FLASH_BSY           0x00000001          // Write/erase operations in progress
#define FLASH_EOP           0x00000002          // End of operation
#define FLASH_ENDHV         0x00000004          // End of high voltage
#define FLASH_WRPERR        0x00000100          // Write protected error
#define FLASH_PGAERR        0x00000200          // Programming alignment error
#define FLASH_SIZERR        0x00000400          // Size error
#define FLASH_OPTVERR       0x00000800          // Option validity error

#define FLASH_ERRs         (FLASH_PGAERR | FLASH_WRPERR | FLASH_SIZERR | FLASH_OPTVERR)

// Option byte register (FLASH_OBR) definitions
#define FLASH_IWDG_SW       0x00100000          // Software IWDG or Hardware IWDG selected


extern struct FlashAlgorithm const FlashDriver;
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

	      FLASH->SR |= FLASH_ERRs;                  // clear error flags

	      // Unlock PECR Register
	      FLASH->PEKEYR = FLASH_PEKEY1;
	      FLASH->PEKEYR = FLASH_PEKEY2;

	      // Unlock Program Matrix
	      FLASH->PRGKEYR = FLASH_PRGKEY1;
	      FLASH->PRGKEYR = FLASH_PRGKEY2;

	      // Test if IWDG is running (IWDG in HW mode)
	      if ((FLASH->OBR & FLASH_IWDG_SW) == 0x00) {
	        // Set IWDG time out to ~32.768 second
	        IWDG->KR  = 0x5555;                     // Enable write access to IWDG_PR and IWDG_RLR
	        IWDG->PR  = 0x06;                       // Set prescaler to 256
	        IWDG->RLR = 0xFFF;                      // Set reload value to 4095
	      }
	    
	  

	  return (0);
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

	      // Lock PECR register and program matrix
	      FLASH->PECR |= FLASH_PRGLOCK;             // Program memory lock
	      FLASH->PECR |= FLASH_PELOCK;              // FLASH_PECR and data memory lock


	      return (0);
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
	int i =0;
	while (i < 0x20000)
	{
		FlashEraseSector(i);
		i=i+256;
	}
    return (0);                                   // Done
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

	  FLASH->PECR |= FLASH_ERASE;                   // Page or Double Word Erase enabled
	  FLASH->PECR |= FLASH_PROG;                    // Program memory selected

	  M32(sectorAddr) = 0x00000000;                        // write '0' to the first address to erase page

	  while (FLASH->SR & FLASH_BSY) {
	    IWDG->KR = 0xAAAA;                          // Reload IWDG
	  }

	  FLASH->PECR &= ~FLASH_ERASE;                  // Page or Double Word Erase disabled
	  FLASH->PECR &= ~FLASH_PROG;                   // Program memory deselected

	  return (0);                                   // Done
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
	  unsigned long  cnt = 128;

	  size = (size + 255) & ~255;                       // adjust programming size

	  FLASH->PECR |= FLASH_FPRG;                    // Half Page programming mode enabled
	  FLASH->PECR |= FLASH_PROG;                    // Program memory selected

	  // program first half page
	  cnt = 128;
	  while (size && cnt) {
	     M32(pageAddr) = *((unsigned long *)buf);        // Program Word
	     pageAddr += 4;
	     buf += 4;
	     size  -= 4;
	     cnt -= 4;
	  }

	  while (FLASH->SR & FLASH_BSY) {
	    IWDG->KR = 0xAAAA;                          // Reload IWDG
	  }

	  // Check for Errors
	  if (FLASH->SR & (FLASH_ERRs)) {
	    FLASH->SR |= FLASH_ERRs;                    // clear error flags
	    return (1);                                 // Failed
	  }

	  FLASH->PECR &= ~FLASH_FPRG;                   // Half Page programming mode disabled
	  FLASH->PECR &= ~FLASH_PROG;                   // Program memory deselected

	  FLASH->PECR |= FLASH_FPRG;                    // Half Page programming mode enabled
	  FLASH->PECR |= FLASH_PROG;                    // Program memory selected

	  // program second half page
	  cnt = 128;
	  while (size && cnt) {
	    M32(pageAddr) = *((unsigned long *)buf);         // Program Word
	    pageAddr += 4;
	    buf += 4;
	    size  -= 4;
	    cnt -= 4;
	  }

	  while (FLASH->SR & FLASH_BSY) {
	    IWDG->KR = 0xAAAA;                          // Reload IWDG
	  }

	  // Check for Errors
	  if (FLASH->SR & (FLASH_ERRs)) {
	    FLASH->SR |= FLASH_ERRs;                    // clear error flags
	    return (1);                                 // Failed
	  }

	  FLASH->PECR &= ~FLASH_FPRG;                   // Half Page programming mode disabled
	  FLASH->PECR &= ~FLASH_PROG;                   // Program memory deselected

	  return (0);                                   // Done
}



