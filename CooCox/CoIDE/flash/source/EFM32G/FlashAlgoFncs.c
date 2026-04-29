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
#include "efm32.h"

#define EFM32_GEOMETRY_FLASH_PAGE_SIZE      0x00000200
#define EFM32_FLASH_PROGRAM_WORD_TIMEOUT    10000000
#define EFM32_GEOMETRY_FLASH_BASE           0x0

/* The size of the flash in the device */
uint32_t flashSize;

/**
 *******************************************************************************
 * @brief      Initialize before Flash Programming/Erase Functions 
 * @param[in]  baseAddr     Flash device base address.
 * @param[in]  clk     			Flash program clock.
 * @param[in]  operateFuc   Init for what operation
 														(FLASH_OPT_ERASECHIP/FLASH_OPT_ERASESECTORS/FLASH_OPT_PROGRAMPAGE/
 														 FLASH_OPT_VERIFY/ FLASH_OPT_BLANKCHECK).
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
  /* Unlock the MSC */
  MSC->LOCK = MSC_UNLOCK_CODE;

  /* Read the flash size from the Device Information Page */
  flashSize = (DEVINFO->MSIZE & _DEVINFO_MSIZE_FLASH_MASK)
              >> _DEVINFO_MSIZE_FLASH_SHIFT;

  /* Convert to Bytes */
  flashSize = flashSize << 10;

  /* Enable writing in MSC */
  MSC->WRITECTRL |= MSC_WRITECTRL_WREN;

  return(0);
}


/**
 *******************************************************************************
 * @brief      Un-Init after Flash Programming/Erase Functions  
 * @param[in]  operateFuc   Init for what operation
 														(FLASH_OPT_ERASECHIP/FLASH_OPT_ERASESECTORS/FLASH_OPT_PROGRAMPAGE/
 														 FLASH_OPT_VERIFY/FLASH_OPT_BLANKCHECK).
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
  /* Disable write in MSC */
  MSC->WRITECTRL &= ~MSC_WRITECTRL_WREN;
  
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
  // TODO: your code for sector erase
    int timeOut = EFM32_FLASH_PROGRAM_WORD_TIMEOUT;

  /* Check that the address is within bounds */
  if (sectorAddr > flashSize)
  {
    return(1);
  }

  /* The address must be block aligned. */
  if ((sectorAddr) & (EFM32_GEOMETRY_FLASH_PAGE_SIZE - 1))
  {
    return(1);
  }

  /* Load address */
  MSC->ADDRB     = sectorAddr;
  MSC->WRITECMD |= MSC_WRITECMD_LADDRIM;

  /* Check for invalid address */
  if (MSC->STATUS & MSC_STATUS_INVADDR)
  {
    MSC->WRITECTRL &= ~MSC_WRITECTRL_WREN;
    return(1);
  }

  /* Send Erase Page command */
  MSC->WRITECMD |= MSC_WRITECMD_ERASEPAGE;

  /* Waiting for the write to complete */
  while ((MSC->STATUS & MSC_STATUS_BUSY) && (timeOut != 0))
  {
    timeOut--;
  }

  /* Check to see if the erase was aborted due to locked page */
  if (MSC->STATUS & MSC_STATUS_LOCKED)
  {
    return 1;
  }

  /* Return 1 if there was a timeout */
  if (timeOut == 0)
  {
    return 1;
  }
  
  return (0);                 
}

/**
 *******************************************************************************
 * @brief      Write a single word (32 bit) to Flash Memory. 
 * @param[in]  adr          Sector Address.
 * @param[out] None  
 * @retval     0   					All is OK.
 * @retval     others       Some error occurs.		 
 *
 * @par Description
 * @details     
 * @note 
 *******************************************************************************
 */ 
uint32_t WriteWord(uint32_t adr, uint32_t data)
{
  int timeOut = EFM32_FLASH_PROGRAM_WORD_TIMEOUT;

  /* Check that the address is within bounds */
  if (adr > flashSize)
  {
    return(1);
  }

  /* Load address, do checks, trigger write once */
  MSC->ADDRB     = adr;
  MSC->WRITECMD |= MSC_WRITECMD_LADDRIM;

  /* Check for invalid address */
  if (MSC->STATUS & MSC_STATUS_INVADDR)
  {
    MSC->WRITECTRL &= ~MSC_WRITECTRL_WREN;
    return 1;
  }
  /* Check that the MSC is ready to receive a new word */
  if (!(MSC->STATUS & MSC_STATUS_WDATAREADY))
  {
    MSC->WRITECTRL &= ~MSC_WRITECTRL_WREN;
    return 1;
  }

  /* Load data into Write Data register */
  MSC->WDATA = data;
  /* Trigger write once */
  MSC->WRITECMD |= MSC_WRITECMD_WRITEONCE;

  /* Waiting for the write to complete */
  while ((MSC->STATUS & MSC_STATUS_BUSY) && (timeOut != 0))
  {
    timeOut--;
  }

  /* Check to see if the write was aborted due to locked page */
  if (MSC->STATUS & MSC_STATUS_LOCKED)
  {
    return 1;
  }

  /* Return */
  if (timeOut == 0)
  {
    return 1;
  }
  return 0;
}


/**
 *******************************************************************************
 * @brief      Proram a page. 
 * @param[in]  pageAddr     Page's start address.
 * @param[in]  size			    Page size
 * @param[in]  buf   			  Source buf point.
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
  // TODO: your code for the page program
  uint32_t count   = 0;
  uint32_t ret     = 0;
  uint32_t adr     = pageAddr;
  uint32_t *buffer = (uint32_t *) buf;

  /* Block must be word aligned */
  if ((adr & 3))
  {
    return(1);
  }

  /* Iterate across the buffer, write one word per iteration. */
  while ((count < size) && (ret == 0))
  {
    ret = WriteWord(adr, *buffer);
    buffer++;
    count += 4;
    adr   += 4;
  }

  return(ret);
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
//int FlashVerify(unsigned long verifyAddr,unsigned long size,unsigned char *buf)
//{
//  // TODO: your code for the page verify
//  return (0);
//}


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
//int FlashBlankCheck (unsigned long checkAddr,unsigned long size)
//{
//  // TODO: your code for the page check
//  return (0);
//}