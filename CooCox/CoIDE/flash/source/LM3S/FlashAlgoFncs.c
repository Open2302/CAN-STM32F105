
/**
 *******************************************************************************
 * @file       FlashAlgoFncs.c
 * @version    V0.3
 * @date       2010.12.02
 * @brief      Flash  Algorithm For Lm3s with 256KB Flash Rom	
 *******************************************************************************
 * @copy
 *
 * INTERNAL FILE,DON'T PUBLIC.
 * 
 * <h2><center>&copy; COPYRIGHT 2009 CooCox </center></h2>
 *******************************************************************************
 */ 
#include <FlashAlgorithm.h>

#include "hw_types.h"
#include "hw_flash.h"
#include "cpu.h"


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
int FlashInit (unsigned long baseAddr, unsigned long clk, unsigned long operateFuc)
{
  //
  // Disables the processor interrupt.
  //
  CPUcpsid();
  
  // Set the Number of Clocks per microsecond for the Flash Controller
  // Approximate division by 1000000 (no Library Code)
  HWREG(FLASH_USECRL) = ((1074*(clk >> 10)) >> 20) - 1;  // clk / 1000000 - 1;

  //
  // Clear the BA bit in RMCTL by writing a 1 to this bit (0 : The Flash memory is at address 0x0)
  //
  HWREG(FLASH_RMCTL)  = FLASH_RMCTL_BA;
  
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
  //
  // Do not add any interrupt re-enable function call.
  //
  
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
int FlashEraseChip (void) {

  // Clear the Flash Access Interrupt
  HWREG(FLASH_FCMISC) = FLASH_FCMISC_ACCESS;

  // Mass Erase according to Errata for 256kB Flash

  // Mass Erase Part 1
  HWREG(FLASH_FMA) = 0x00000000;
  HWREG(FLASH_FMC) = FLASH_FMC_WRKEY | FLASH_FMC_MERASE;

  // Wait until Erase is done
  while (HWREG(FLASH_FMC) & FLASH_FMC_MERASE);

  // Mass Erase Part 2
  HWREG(FLASH_FMA) = 0x00020000;
  HWREG(FLASH_FMC) = FLASH_FMC_WRKEY | FLASH_FMC_MERASE;

  // Wait until Erase is done
  while (HWREG(FLASH_FMC) & FLASH_FMC_MERASE);


  // Check Access Violation
  if (HWREG(FLASH_FCRIS) & FLASH_FCRIS_ACCESS) {
    return (1);
  }

  return (0);   // Success
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

  // Address must be Block aligned
  if (sectorAddr & (FLASH_ERASE_SIZE - 1)) {
    return (1);
  }

  // Clear the Flash Access Interrupt
  HWREG(FLASH_FCMISC) = FLASH_FCMISC_ACCESS;

  // Erase the Block
  HWREG(FLASH_FMA) = sectorAddr;
  HWREG(FLASH_FMC) = FLASH_FMC_WRKEY | FLASH_FMC_ERASE;

  // Wait until Erase is done
  while (HWREG(FLASH_FMC) & FLASH_FMC_ERASE);

  // Check Access Violation
  if (HWREG(FLASH_FCRIS) & FLASH_FCRIS_ACCESS) {
    return (1);
  }

  return (0);   // Success
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

  // Address and Size must be Word aligned
  if ((pageAddr & 3) || (size & 3)) {
    return (1);
  }

  // Clear the Flash Access Interrupt
  HWREG(FLASH_FCMISC) = FLASH_FCMISC_ACCESS;

  // Loop over the Words to be programmed
  while (size) {

    // Program Word
    HWREG(FLASH_FMA) = pageAddr;
    HWREG(FLASH_FMD) = *((unsigned long *)buf);
    HWREG(FLASH_FMC) = FLASH_FMC_WRKEY | FLASH_FMC_WRITE;

    // Wait unitl Word has been programmed
    while (HWREG(FLASH_FMC) & FLASH_FMC_WRITE);

    // Prepeare Next Word
    pageAddr += 4;
    buf += 4;
    size  -= 4;

  }

  // Check Access Violation
  if (HWREG(FLASH_FCRIS) & FLASH_FCRIS_ACCESS) {
    return (1);
  }

  return (0);   // Success
}

