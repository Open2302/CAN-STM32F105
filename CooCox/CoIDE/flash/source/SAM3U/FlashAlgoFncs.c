/**
 *******************************************************************************
 * @file       FlashAlgoFncs.c
 * @version    V0.3
 * @date       2010.12.02
 * @brief      Flash  Algorithm For SAM3U with 128KB Flash Rom	
 *******************************************************************************
 * @copy
 *
 * INTERNAL FILE,DON'T PUBLIC.
 * 
 * <h2><center>&copy; COPYRIGHT 2009 CooCox </center></h2>
 *******************************************************************************
 */ 
 
#include <FlashAlgorithm.h>

#include "AT91SAM3U4.h"


#define KEY                    (0x37   << 16)
#define FKEY                   (0x5AUL << 24)
#define FLASH_PAGE_SIZE_BYTE   256

#define STACK_SIZE   64        // Stack Size


unsigned long base_adr;        // Base Address


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
int FlashInit (unsigned long baseAddr,unsigned long clk, unsigned long operateFuc)
{
  AT91PS_EFC      pEFC;

  base_adr = baseAddr;

#if SAM3U_B1_128
  pEFC = AT91C_BASE_EFC1;
#else
  pEFC = AT91C_BASE_EFC0;
#endif

  // Set Flash Waite State to maximum
  pEFC->EFC_FMR = AT91C_EFC_FWS;

  // Disable Watchdog
  *AT91C_WDTC_WDMR = AT91C_WDTC_WDDIS;

  // Enable the Main Oscillator:
  // SCK Period = 1/32768 = 30.51 us
  // Start-up Time = 8 * 15 / SCK = 120 * 30.51us = 3.6612 ms
  *AT91C_PMC_MOR = (AT91C_CKGR_MOSCSEL | KEY | (AT91C_CKGR_MOSCXTST & (0x0F << 8) | AT91C_CKGR_MOSCXTEN));

  // Wait the Start-up Time
  while(!(*AT91C_PMC_SR & AT91C_PMC_MOSCXTS));

  // Select Main Clock (CSS field)
  *AT91C_PMC_MCKR = (*AT91C_PMC_MCKR & ~AT91C_PMC_CSS) | AT91C_PMC_CSS_MAIN_CLK;

  // Wait for Clock ready
  while (!(*AT91C_PMC_SR & AT91C_PMC_MCKRDY));

  // Select Main Clock
  *AT91C_PMC_MCKR = AT91C_PMC_CSS_MAIN_CLK;

  // Wait for Clock ready
  while (!(*AT91C_PMC_SR & AT91C_PMC_MCKRDY));


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
int FlashUnInit(unsigned long operateFuc)
{
  unsigned long   FRR;
  AT91PS_EFC      pEFC;

#if SAM3U_B1_128
  pEFC = AT91C_BASE_EFC1;
#else
  pEFC = AT91C_BASE_EFC0;
#endif
  // after programming Flash  set GPNVMBit to boot from Flash
  if (operateFuc == FLASH_OPT_PROGRAMPAGE) {
    // Get GPNVM bit 2
    pEFC->EFC_FCR = FKEY | (2 << 8) | AT91C_EFC_FCMD_GFB;

    // Wait until the end of Command
    while (!(pEFC->EFC_FSR & AT91C_EFC_FRDY_S));

    FRR = pEFC->EFC_FRR;
    if ((pEFC == AT91C_BASE_EFC0) && (FRR != 2)) {
      // Set GPNVM bit 2 for booting from Flash
      pEFC->EFC_FCR = FKEY | (1 << 8) | AT91C_EFC_FCMD_SFB;

      // Wait until the end of Command
      while (!(pEFC->EFC_FSR & AT91C_EFC_FRDY_S));

      // Clear GPNVM bit 2 for booting from Flash
      pEFC->EFC_FCR = FKEY | (2 << 8) | AT91C_EFC_FCMD_CFB;

      // Wait until the end of Command
      while (!(pEFC->EFC_FSR & AT91C_EFC_FRDY_S));
    }

    if ((pEFC == AT91C_BASE_EFC1) && (FRR != 6)) {
      // Set GPNVM bit 2 for booting from Flash
      pEFC->EFC_FCR = FKEY | (1 << 8) | AT91C_EFC_FCMD_SFB;

      // Wait until the end of Command
      while (!(pEFC->EFC_FSR & AT91C_EFC_FRDY_S));

      // Clear GPNVM bit 2 for booting from Flash
      pEFC->EFC_FCR = FKEY | (2 << 8) | AT91C_EFC_FCMD_SFB;

      // Wait until the end of Command
      while (!(pEFC->EFC_FSR & AT91C_EFC_FRDY_S));
    }
  }

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
int FlashEraseChip (void)
{
  AT91PS_EFC      pEFC;

#if SAM3U_B1_128
  pEFC = AT91C_BASE_EFC1;
#else
  pEFC = AT91C_BASE_EFC0;
#endif

  // Erase All Command
  pEFC->EFC_FCR = FKEY | AT91C_EFC_FCMD_EA;

  // Wait until the end of Command
  while (!(pEFC->EFC_FSR & AT91C_EFC_FRDY_S));

  return (0);
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
  return (0);                  // Automatic Erase during Program Cycle
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
int FlashProgramPage (unsigned long pageAddr, unsigned long size, unsigned char *buf)
{
  unsigned long   page;
  unsigned long * Flash;
  AT91PS_EFC      pEFC;

#if SAM3U_B1_128
  pEFC = AT91C_BASE_EFC1;
#else
  pEFC = AT91C_BASE_EFC0;
#endif

  Flash = (unsigned long *)pageAddr;
  page  = (pageAddr - base_adr) / FLASH_PAGE_SIZE_BYTE;

  // Unlock Page Command
  pEFC->EFC_FCR = FKEY | AT91C_EFC_FCMD_CLB | (AT91C_EFC_FARG & (page << 8));

  // Wait until the end of Command
  while (!(pEFC->EFC_FSR & AT91C_EFC_FRDY_S));

  // Copy to the Write Buffer
  for (size = (size + 3) & ~3; size; size -= 4, buf += 4) {
	*Flash++ = *((unsigned long *)buf);
  }
  //* Start Programming Command
  pEFC->EFC_FCR = FKEY | AT91C_EFC_FCMD_EWP | (AT91C_EFC_FARG & (page << 8));

  // Wait until the end of Command
  while (!(pEFC->EFC_FSR & AT91C_EFC_FRDY_S));

  // Check for Errors
  if (pEFC->EFC_FSR & (AT91C_EFC_FCMDE | AT91C_EFC_LOCKE)) return (1);

  return (0);
}

/**
 *******************************************************************************
 * @brief      Page Verify Function. 
 * @param[in]  verifyAddr   Verify Start Address(Usually page start address).
 * @param[in]  size			Verify size
 * @param[in]  buf   		Source buf point.
 * @param[out] None  
 * @retval     0   			Verify pass.
 * @retval     others       Some error occurs or verify failed..		 
 *
 * @par Description
 * @details   Optional function. When this function is absence, 
 *            the link will read flash memory directly to do verify.  
 * @note 
 *******************************************************************************
 */ 
int FlashVerify(unsigned long verifyAddr,unsigned long size,unsigned char *buf)
{
  // TODO: your code for the page verify
  int i;
  unsigned char* pageBuf = (unsigned char*)verifyAddr;
  
  for(i = 0; i < size; i++)
  {
	if(pageBuf[i] != buf[i])
	{
		return (1);
	}
  }
   
  return (0);
}


