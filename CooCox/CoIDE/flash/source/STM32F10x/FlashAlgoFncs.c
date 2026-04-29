
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
#define FLASH_BASE      0x40022000

#define IWDG            ((IWDG_TypeDef *) IWDG_BASE)
#define FLASH           ((FLASH_TypeDef*) FLASH_BASE)


// FLASH BANK size
#define BANK1_SIZE      0x00080000      // Bank1 Size = 512kB

// Independent WATCHDOG
typedef struct {
  vu32 KR;
  vu32 PR;
  vu32 RLR;
  vu32 SR;
} IWDG_TypeDef;

// Flash Registers
typedef struct {
  vu32 ACR;
  vu32 KEYR;
  vu32 OPTKEYR;
  vu32 SR;
  vu32 CR;
  vu32 AR;
  vu32 RESERVED;
  vu32 OBR;
  vu32 WRPR;
#ifdef STM32F10x_XLD_1024 || STM32F10x_XLD_768
  vu32 RESERVED1[8];
  vu32 KEYR2;                                   // offset  0x044
  vu32 RESERVED2[1];
  vu32 SR2;                                     // offset  0x04C
  vu32 CR2;                                     // offset  0x050
  vu32 AR2;                                     // offset  0x054
#endif
} FLASH_TypeDef;


// Flash Keys
#define RDPRT_KEY       0x5AA5
#define FLASH_KEY1      0x45670123
#define FLASH_KEY2      0xCDEF89AB

// Flash Control Register definitions
#define FLASH_PG        0x00000001
#define FLASH_PER       0x00000002
#define FLASH_MER       0x00000004
#define FLASH_OPTPG     0x00000010
#define FLASH_OPTER     0x00000020
#define FLASH_STRT      0x00000040
#define FLASH_LOCK      0x00000080
#define FLASH_OPTWRE    0x00000100

// Flash Status Register definitions
#define FLASH_BSY       0x00000001
#define FLASH_PGERR     0x00000004
#define FLASH_WRPRTERR  0x00000010
#define FLASH_EOP       0x00000020

unsigned long base_adr;

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
  base_adr = baseAddr & ~(BANK1_SIZE - 1);          // Align to Size Boundary
  
  *((volatile unsigned int *)0x40021004) = 0x00000000;
  *((volatile unsigned int *)0x40021000) = 0x00010081;
  *((volatile unsigned int *)0x40021008) = 0x00000000;

  // Unlock Flash    
  FLASH->KEYR = FLASH_KEY1;
  FLASH->KEYR = FLASH_KEY2;
  
#ifdef STM32F10x_XLD_1024 || STM32F10x_XLD_768
  FLASH->KEYR2 = FLASH_KEY1;                    // Flash bank 2
  FLASH->KEYR2 = FLASH_KEY2;
#endif
  
  // Zero Wait State
  FLASH->ACR  =  0x00000000;
  
  FLASH->CR   =  0x00000000;
  FLASH->SR   =  0x00000034;

  // Test if IWDG is running (IWDG in HW mode)
  if ((FLASH->OBR & 0x04) == 0x00) {
    // Set IWDG time out to ~32.768 second
    IWDG->KR  = 0x5555; // Enable write access to IWDG_PR and IWDG_RLR     
    IWDG->PR  = 0x06;   // Set prescaler to 256  
    IWDG->RLR = 4095;   // Set reload value to 4095
  }  
                      
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
  // Lock Flash
  //FLASH->CR |=  FLASH_LOCK;

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
  FLASH->CR  =  FLASH_MER;                      // Mass Erase Enabled
  FLASH->CR |=  FLASH_STRT;                     // Start Erase

  while (FLASH->SR & FLASH_BSY) {
    IWDG->KR = 0xAAAA;                          // Reload IWDG
  }

  FLASH->CR &= ~FLASH_MER;                      // Mass Erase Disabled
  
#ifdef STM32F10x_XLD_1024 || STM32F10x_XLD_768                           // Flash bank 2
  FLASH->CR2 |=  FLASH_MER;
  FLASH->CR2 |=  FLASH_STRT;

  while (FLASH->SR2 & FLASH_BSY) {
    IWDG->KR = 0xAAAA;
  }

  FLASH->CR2 &= ~FLASH_MER;
#endif

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
#ifdef STM32F10x_XLD_1024 || STM32F10x_XLD_768
  if (sectorAddr < (base_adr + BANK1_SIZE)) {          // Flash bank 2
#endif
  FLASH->CR  =  FLASH_PER;                      // Page Erase Enabled 
  FLASH->AR  =  sectorAddr;                     // Page Address
  FLASH->CR |=  FLASH_STRT;                     // Start Erase

  while (FLASH->SR & FLASH_BSY) {
    IWDG->KR = 0xAAAA;                          // Reload IWDG
  }

  FLASH->CR = 0;                                // Page Erase Disabled 
#ifdef STM32F10x_XLD_1024 || STM32F10x_XLD_768
  }
  else {                                        // Flash bank 2
    FLASH->CR2 |=  FLASH_PER;
    FLASH->AR2  =  sectorAddr;
    FLASH->CR2 |=  FLASH_STRT;

    while (FLASH->SR2 & FLASH_BSY) {
      IWDG->KR = 0xAAAA;
    }

    FLASH->CR2 &= ~FLASH_PER;
  }
#endif

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
  size = (size + 1) & ~1;                       // Adjust size for Half Words
#ifdef STM32F10x_XLD_1024 || STM32F10x_XLD_768
  if (pageAddr < (base_adr + BANK1_SIZE)) {          // Flash bank 2
#endif  
  while (size) {

    FLASH->CR  =  FLASH_PG;                     // Programming Enabled

    M16(pageAddr) = *((unsigned short *)buf);   // Program Half Word
    while (FLASH->SR & FLASH_BSY);

    FLASH->CR  = 0;                     // Programming Disabled

    // Check for Errors
    if (FLASH->SR & (FLASH_PGERR | FLASH_WRPRTERR)) {
      FLASH->SR |= FLASH_PGERR | FLASH_WRPRTERR;
      return (1);                               // Failed
    }

    // Go to next Half Word
    pageAddr += 2;
    buf += 2;
    size -= 2;
   
  }
#ifdef STM32F10x_XLD_1024 || STM32F10x_XLD_768
  }
  else {                                        // Flash bank 2
    while (size) {

      FLASH->CR2 |=  FLASH_PG;

      M16(pageAddr) = *((unsigned short *)buf);
      while (FLASH->SR2 & FLASH_BSY);

      FLASH->CR2 &= ~FLASH_PG;

      // Check for Errors
      if (FLASH->SR2 & (FLASH_PGERR | FLASH_WRPRTERR)) {
        FLASH->SR2 |= FLASH_PGERR | FLASH_WRPRTERR;
        return (1);
      }

      // Go to next Half Word
      pageAddr += 2;
      buf += 2;
      size  -= 2;
    }
  }
#endif
  return 0;
}



