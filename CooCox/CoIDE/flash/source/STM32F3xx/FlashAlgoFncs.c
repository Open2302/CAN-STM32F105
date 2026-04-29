/**
 *******************************************************************************
 * @file       FlashAlgoFncs.c
 * @version    V0.1
 * @date       2012.03.12
 * @brief      Flash  Algorithm For STM32F4xx
 *******************************************************************************
 * @copy
 *
 * <h2><center>&copy; COPYRIGHT 2012 CooCox </center></h2>
 *******************************************************************************
 */ 
 
#include <FlashAlgorithm.h>


typedef volatile unsigned short   vu16;
typedef          unsigned short    u16;
typedef volatile unsigned long    vu32;
typedef          unsigned long     u32;

#define M16(adr) (*((vu16 *) (adr)))
#define M32(adr) (*((vu32 *) (adr)))

// Peripheral Memory Map
#define IWDG_BASE       0x40003000
#define FLASH_BASE      0x40022000

#define IWDG            ((IWDG_TypeDef *) IWDG_BASE)
#define FLASH           ((FLASH_TypeDef*) FLASH_BASE)

// Independent WATCHDOG
typedef struct {
  vu32 KR;
  vu32 PR;
  vu32 RLR;
  vu32 SR;
} IWDG_TypeDef;

// Flash Registers
typedef struct {
  vu32 ACR;                                     // offset  0x000
  vu32 KEYR;                                    // offset  0x004
  vu32 OPTKEYR;                                 // offset  0x008
  vu32 SR;                                      // offset  0x00C
  vu32 CR;                                      // offset  0x010
  vu32 AR;                                      // offset  0x014
  vu32 RESERVED0[1];
  vu32 OBR;                                     // offset  0x01C
  vu32 WRPR;                                    // offset  0x020
} FLASH_TypeDef;


// Flash Keys
#define RDPRT_KEY       0x55AA
#define FLASH_KEY1      0x45670123
#define FLASH_KEY2      0xCDEF89AB

// Flash Control Register definitions
#define FLASH_PG                ((unsigned int)(1ul << 0))
#define FLASH_PER               ((unsigned int)(1ul << 1))
#define FLASH_MER               ((unsigned int)(1ul << 2))
#define FLASH_OPTPG             ((unsigned int)(1ul << 4))
#define FLASH_OPTER             ((unsigned int)(1ul << 5))
#define FLASH_STRT              ((unsigned int)(1ul << 6))
#define FLASH_LOCK              ((unsigned int)(1ul << 7))
#define FLASH_OPTWRE            ((unsigned int)(1ul << 9))

// Flash Status Register definitions
#define FLASH_BSY               ((unsigned int)(1ul << 0))
#define FLASH_PGERR             ((unsigned int)(1ul << 2))
#define FLASH_WRPRTERR          ((unsigned int)(1ul << 4))
#define FLASH_EOP               ((unsigned int)(1ul << 5))

#define FLASH_ERR               (FLASH_PGERR | FLASH_WRPRTERR)

/**
 *******************************************************************************
 * @brief      Get Sector Number 
 * @param[in]  addr     Sector Address
 * @param[out] None  
 * @retval     Sector Number		 
 *
 *******************************************************************************
 */
unsigned long GetSecNum (unsigned long addr) 
{
  unsigned long n;

  n = (addr >> 12) & 0x000FF;                            // only bits 12..19

  if (n >= 0x20) 
  {
    n = 4 + (n >> 5);                                    // 128kB Sector
  }
  else if (n >= 0x10) 
  {
    n = 3 + (n >> 4);                                    //  64kB Sector
  }
  else                
  {
    n = 0 + (n >> 2);                                    //  16kB Sector
  }

  return (n);                                            // Sector Number
}

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
	FLASH->KEYR = FLASH_KEY1;                             // Unlock Flash
	FLASH->KEYR = FLASH_KEY2;

	FLASH->ACR  = 0x00000000;                             // Zero Wait State, no Cache, no Prefetch
	FLASH->SR  |= FLASH_ERR;                              // Reset Error Flags

	if ((FLASH->OBR & (1ul << 8)) == 0) {                 // Test if IWDG is running (IWDG in HW mode)
	// Set IWDG time out to ~32.768 second
	IWDG->KR  = 0x5555;                                 // Enable write access to IWDG_PR and IWDG_RLR
	IWDG->PR  = 0x06;                                   // Set prescaler to 256
	IWDG->RLR = 4095;                                   // Set reload value to 4095
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
  FLASH->CR |=  FLASH_LOCK;                             // Lock Flash

  return (0);

  return 0;
}

int FlashEraseChip   (void)
{
	  FLASH->CR |=  FLASH_MER;                              // Mass Erase Enabled
	  FLASH->CR |=  FLASH_STRT;                             // Start Erase

	  while (FLASH->SR & FLASH_BSY) {
	    IWDG->KR = 0xAAAA;                                  // Reload IWDG
	  }

	  FLASH->CR &= ~FLASH_MER;                              // Mass Erase Disabled

	  return (0);                                           // Done
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
    FLASH->CR  |=  FLASH_PER;                           // Page Erase Enabled
    FLASH->AR   =  sectorAddr;                                 // Page Address
    FLASH->CR  |=  FLASH_STRT;                          // Start Erase

    while (FLASH->SR  & FLASH_BSY) {
      IWDG->KR = 0xAAAA;                                // Reload IWDG
    }

    FLASH->CR  &= ~FLASH_PER;                           // Page Erase Disabled

  return (0);                                           // Done
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
  
	size = (size + 1) & ~1;                                   // Adjust size for Half Words

	  while (size) {

	    FLASH->CR  |=  FLASH_PG;                            // Programming Enabled

	    M16(pageAddr) = *((u16 *)buf);                           // Program Half Word
	    while (FLASH->SR  & FLASH_BSY);

	    FLASH->CR  &= ~FLASH_PG;                            // Programming Disabled

	    if (FLASH->SR & FLASH_ERR) {                        // Check for Error
	      FLASH->SR |= FLASH_ERR;                           // Reset Error Flags
	      return (1);                                       // Failed
	    }

	    pageAddr += 2;                                           // Go to next Half Word
	    buf += 2;
	    size  -= 2;
	  }

	  return (0);                                           // Done
}



