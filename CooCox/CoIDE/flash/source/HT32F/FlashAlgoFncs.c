
/**
 *******************************************************************************
 * @file       FlashAlgoFncs.c
 * @version    V0.1
 * @date       2011.03.17
 * @brief      Flash  Algorithm For HT32F125x
 *******************************************************************************
 * @copy
 *
 * INTERNAL FILE,DON'T PUBLIC.
 * 
 * <h2><center>&copy; COPYRIGHT 2009 CooCox </center></h2>
 *******************************************************************************
 */ 

#include <FlashAlgorithm.h>


#define FMC_BASE              (0x40080000)
#define FMC_OPT_BASE          (0x1FF00000)
#define FMC_TADR              (FMC_BASE + 0x000)
#define FMC_WRDR              (FMC_BASE + 0x004)
#define FMC_OCMR              (FMC_BASE + 0x00C)
#define FMC_OPCR              (FMC_BASE + 0x010)
#define FMC_VMCR              (FMC_BASE + 0x100)
#define FMC_MDID              (FMC_BASE + 0x180)
#define FMC_PNSR              (FMC_BASE + 0x184)
#define FMC_PSSR              (FMC_BASE + 0x188)

#define OPM_IDLE              (0x0C)
#define OPM_CMD_TO_MAIN       (0x14)
#define OPM_FINISH_ON_MAIN    (0x1C)

#define CMD_IDLE              (0x0)
#define CMD_WORD_PROGRAM      (0x4)
#define CMD_PAGE_ERASE        (0x8)
#define CMD_MASS_ERASE        (0xA)

#define VMCR_MAIN             (0x3)
#define HWREG(x)              (*((volatile unsigned long *)(x)))

int fidx = 0;
int fsize = 0;
int psize = 0;
int lastErase = 0xFFFFFFFF;


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
  unsigned long temp;
  HWREG(FMC_VMCR) = 3;

  temp = HWREG(FMC_MDID);

  switch (temp)
  {
    case 0x03761552:
    {
      psize = HWREG(FMC_PSSR);
      fsize = 0x7C00;
      break;
    }
    default:
    {
      psize = HWREG(FMC_PSSR);
      fsize = psize * HWREG(FMC_PNSR);
      break;
    }
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
  HWREG(FMC_OPCR) = OPM_IDLE;
  HWREG(FMC_OCMR) = CMD_IDLE;
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
  HWREG(FMC_OCMR) = CMD_MASS_ERASE;
  HWREG(FMC_OPCR) = OPM_CMD_TO_MAIN;

  // Wait until all operations have been finished
  while (1)
  {
    if (((HWREG(FMC_OPCR)& 0x1E) == OPM_FINISH_ON_MAIN))
    {
      break;
    }
  }
  
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
  // Address must be page aligned
  sectorAddr = sectorAddr & (~(psize - 1));

  // Skip erased address
  if ((sectorAddr) == lastErase)
  {
    return (0);
  }

  // Check sectorAddr, provent address over range
  if (sectorAddr >= fsize)
  {
    return (1);
  }

  HWREG(FMC_TADR) = sectorAddr;
  HWREG(FMC_OCMR) = CMD_PAGE_ERASE;
  HWREG(FMC_OPCR) = OPM_CMD_TO_MAIN;

  // Wait until all operations have been finished
  while (1)
  {
    if (((HWREG(FMC_OPCR)& 0x1E) == OPM_FINISH_ON_MAIN))
    {
      break;
    }
  }

  lastErase = sectorAddr;

  return (0);
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
  if ((pageAddr & 3) || (size & 3))
  {
    return (1);
  }

  HWREG(FMC_OCMR) = CMD_WORD_PROGRAM;
  while (size)
  {
    // Check pageAddr, provent programming address over range
    if (pageAddr >= fsize)
    {
      return (1);
    }

    HWREG(FMC_TADR) = pageAddr;
    HWREG(FMC_WRDR) = *((unsigned long *) buf);
    HWREG(FMC_OPCR) = OPM_CMD_TO_MAIN;

    // Wait until all operations have been finished
    while (1)
    {
      if (((HWREG(FMC_OPCR)& 0x1E) == OPM_FINISH_ON_MAIN))
      {
        break;
      }
    }

    pageAddr += 4;
    size  -= 4;
    buf += 4;
  }

  return (0);
}


