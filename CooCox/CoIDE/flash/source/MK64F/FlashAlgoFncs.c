
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
#include "SSD_Types.h"
#include "SSD_FTFx.h"
#include "SSD_FTFx_Internal.h"
#include "ConfigureKeil.h"

#define M8(adr)  (*((volatile unsigned char  *)(adr)))
#define M16(adr) (*((volatile unsigned short *)(adr)))
#define M32(adr) (*((volatile unsigned long  *)(adr)))

#define STACK_SIZE   1024      // Stack Size
#define PAGE_SIZE    2048      // Page Size

    /* FTFL module base */
#define FTFL_REG_BASE           0x40020000
#define EERAM_BLOCK_BASE        0x14000000

  #define PBLOCK_SIZE             0x00100000      /* 1 MB       */
  #define EERAM_BLOCK_SIZE        0x00004000      /* 16 KB size */
  #define DEFLASH_BLOCK_BASE      0x10000000
  
#define DATAFLASH_IFR_READRESOURCE_ADDRESS   0x100000FC 

FLASH_SSD_CONFIG flashSSDConfig; 
unsigned long base_adr;
const unsigned long unsecureword = 0xFFFFFFFE;
const unsigned long long unsecurewordLL = 0xFFFFFFFEFFFFFFFF;

 #define WDOG_UNLOCK                   *((volatile unsigned short *)0x4005200E) 
 #define WDOG_STCTRLH                  *((volatile unsigned short *)0x40052000) 
 #define WDOG_STCTRLH_WDOGEN_MASK_X    0x00000001
 
 #define FLASH_CONFIG_FSEC             0x0000040C
 
 
/************************************************************************
*
*  Function Name    : FlashProgramPhrase.c
*  Description      : Program data into Flash
*  Arguments        : PFLASH_SSD_CONFIG, UINT32, UINT32, UINT32,
*                     pFLASHCOMMANDSEQUENCE
*  Return Value     : UINT32
*
*************************************************************************/

UINT32 FlashProgramPhrase(PFLASH_SSD_CONFIG PSSDConfig, \
                                   UINT32 destination, \
                                   UINT32 size, \
                                   UINT32 source, \
                                   pFLASHCOMMANDSEQUENCE FlashCommandSequence)
{
    UINT8 pCommandArray[12]; /* command sequence array */
    UINT32 returnCode;      /* return code variable */
    UINT32 endAddress;      /* storing end address */
    
    /* set the default return code as FTFx_OK */
    returnCode = FTFx_OK;
        
    /* calculating Flash end address */
    endAddress = destination + size;
        
    /* check if the destination is Longword aligned or not */
    if (0 != (destination % FTFx_PHRASE_SIZE))
    {
        /* return an error code FTFx_ERR_ADDR */
        returnCode = FTFx_ERR_ADDR;
        goto EXIT;
    }
        
    /* check if the size is Longword alignment or not */
    if(0 != (size % FTFx_PHRASE_SIZE))
    {
        /* return an error code FTFx_ERR_SIZE */
        returnCode = FTFx_ERR_SIZE;
            goto EXIT;
    }
    
    /* check for valid range of the target addresses */
    if((destination < PSSDConfig->PFlashBlockBase) || \
        (endAddress > (PSSDConfig->PFlashBlockBase + PSSDConfig->PFlashBlockSize)))
    {
        if((destination < PSSDConfig->DFlashBlockBase) || \
            (endAddress > (PSSDConfig->DFlashBlockBase + PSSDConfig->DFlashBlockSize)))
        {
            /* return an error code FTFx_ERR_RANGE */
            returnCode = FTFx_ERR_RANGE;
            goto EXIT;
        }
        else
        {
            /* Convert System memory address to FTFx internal memory address */
            destination = destination - PSSDConfig->DFlashBlockBase + 0x800000;
        }
    }
    else
    {
        /* Convert System memory address to FTFx internal memory address */
        destination -= PSSDConfig->PFlashBlockBase;
    }
    
    /* check for error return code */
    if(FTFx_OK == returnCode)
    {
        while(size > 0)
        {
            /* preparing passing parameter to program the flash block */
            pCommandArray[0] = FTFx_PROGRAM_PHRASE;
            pCommandArray[1] = (UINT8)(destination >> 16);
            pCommandArray[2] = (UINT8)((destination >> 8) & 0xFF);
            pCommandArray[3] = (UINT8)(destination & 0xFF);
        
#if (ENDIANNESS == BIG_ENDIAN)  /* Big Endian */ 
            pCommandArray[4] = READ8(source);
            pCommandArray[5] = READ8(source + 1);
            pCommandArray[6] = READ8(source + 2);
            pCommandArray[7] = READ8(source + 3);
            pCommandArray[8] = READ8(source + 4);
            pCommandArray[9] = READ8(source + 5);
            pCommandArray[10] = READ8(source + 6);
            pCommandArray[11] = READ8(source + 7);
#else /* Little Endian */
            pCommandArray[4] = READ8(source + 3);
            pCommandArray[5] = READ8(source + 2);
            pCommandArray[6] = READ8(source + 1);
            pCommandArray[7] = READ8(source);
            pCommandArray[8] = READ8(source + 7);
            pCommandArray[9] = READ8(source + 6);
            pCommandArray[10] = READ8(source + 5);
            pCommandArray[11] = READ8(source + 4);
#endif
            
            /* calling flash command sequence function to execute the command */
            returnCode = FlashCommandSequence(PSSDConfig, 11, pCommandArray);
    
            /* checking for the success of command execution */
            if(FTFx_OK != returnCode)
            {
                break;
            }
            else
            {
                /* update destination address for next iteration */
                destination += FTFx_PHRASE_SIZE;
                /* update size for next iteration */
                size -= FTFx_PHRASE_SIZE;
                        /* increment the source adress by 1 */
                source += FTFx_PHRASE_SIZE;
            }
        }
    }

EXIT:
    /* Enter Debug state if enabled */
    if (TRUE == (PSSDConfig->DebugEnable))
    {
#if ((CPU_CORE == ARM_CM4) && (COMPILER == IAR))    /* Kx Products */
        asm
        (
            " BKPT #0 \n "           /* enter Debug state */
        );
#endif
    }

    return(returnCode);
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
  
  base_adr = baseAddr;
    /* Write 0xC520 to the unlock register */
  WDOG_UNLOCK = 0xC520;
  /* Followed by 0xD928 to complete the unlock */
  WDOG_UNLOCK = 0xD928;
  /* Clear the WDOGEN bit to disable the watchdog */
  WDOG_STCTRLH &= ~WDOG_STCTRLH_WDOGEN_MASK_X;
  
  flashSSDConfig.ftfxRegBase        = FTFL_REG_BASE;           /* FTFL control register base */
  flashSSDConfig.PFlashBlockBase    = 0;                       /* base address of PFlash block */
  flashSSDConfig.PFlashBlockSize    = PBLOCK_SIZE;             /* size of PFlash block */
#if(DEBLOCK_SIZE != 0)	
  flashSSDConfig.DFlashBlockBase    = DEFLASH_BLOCK_BASE;      /* base address of DFlash block */
#endif
  flashSSDConfig.EERAMBlockBase     = EERAM_BLOCK_BASE;        /* base address of EERAM block */
  flashSSDConfig.EERAMBlockSize     = EERAM_BLOCK_SIZE;        /* size of EERAM block */
  flashSSDConfig.DebugEnable        = 0;                       /* background debug mode enable bit */
  flashSSDConfig.CallBack           = NULL_CALLBACK;           /* pointer to callback function */
  if (FTFx_OK != pFlashInit(&flashSSDConfig)) return (1);
  return (0);

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
  // Start Sector Erase Command
  if (FTFx_OK != pFlashEraseSector(&flashSSDConfig, sectorAddr, FTFx_PSECTOR_SIZE, pFlashCommandSequence)) return (1);
  return (0);                                  // Finished without Errors
}

int FlashEraseChip (void)
{
  if (FTFx_OK != pFlashEraseAllBlock(&flashSSDConfig,pFlashCommandSequence)) return (1);
  if (FTFx_OK != pFlashProgramPhrase(&flashSSDConfig, 0x408, 8, (unsigned long long)&unsecurewordLL, pFlashCommandSequence)) return (1);
  return (0);                                  // Finished without Errors
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
  size = (size + 7) & ~0x00000007;                 // Align to FTFL_PHRASE_SIZE
  if (FTFx_OK != pFlashProgramPhrase(&flashSSDConfig, pageAddr, size, (unsigned long)buf, pFlashCommandSequence)) return (1);
  return (0);  
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
  
  
}
#endif

