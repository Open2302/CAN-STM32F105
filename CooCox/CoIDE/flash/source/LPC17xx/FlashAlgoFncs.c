
/**
 *******************************************************************************
 * @file       FlashAlgoFncs.c
 * @version    V0.3
 * @date       2010.12.02
 * @brief      Flash  Algorithm For Lpc17xx	
 *******************************************************************************
 * @copy
 *
 * INTERNAL FILE,DON'T PUBLIC.
 * 
 * <h2><center>&copy; COPYRIGHT 2009 CooCox </center></h2>
 *******************************************************************************
 */ 
 
#include <FlashAlgorithm.h>


// Memory Mapping Control
#define MEMMAP   (*((volatile unsigned char *) 0x400FC040))

// Phase Locked Loop (Main PLL)
#define PLL0CON  (*((volatile unsigned char *) 0x400FC080))
#define PLL0CFG  (*((volatile unsigned long *) 0x400FC084))
#define PLL0STAT (*((volatile unsigned long *) 0x400FC088))
#define PLL0FEED (*((volatile unsigned char *) 0x400FC08C))


#define SET_VALID_CODE 1       // Set Valid User Code Signature

#ifdef LPC17xx_512
#define END_SECTOR     29
#endif
#ifdef LPC17xx_256
#define END_SECTOR     21
#endif
#ifdef LPC17xx_128
#define END_SECTOR     17
#endif
#ifdef LPC17xx_64
#define END_SECTOR     15
#endif
#ifdef LPC17xx_32
#define END_SECTOR     7
#endif


unsigned long CCLK;            // CCLK in kHz

struct sIAP {                  // IAP Structure
  unsigned long cmd;           // Command
  unsigned long par[4];        // Parameters
  unsigned long stat;          // Status
  unsigned long res[2];        // Result
} IAP;


/* IAP Call */
typedef void (*IAP_Entry) (unsigned long *cmd, unsigned long *stat);
#define IAP_Call ((IAP_Entry) 0x1FFF1FF1)


unsigned long GetSecNum (unsigned long adr) {
  unsigned long n;

  n = adr >> 12;                               //  4kB Sector
  if (n >= 0x10) {
    n = 0x0E + (n >> 3);                       // 32kB Sector
  }

  return (n);                                  // Sector Number
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
int FlashInit (unsigned long baseAddr, unsigned long clk, unsigned long operateFuc)
{

  CCLK     = 4000;                             // 4MHz Internal RC Oscillator

  PLL0CON  = 0x00;                             // Disable PLL (use Oscillator)
  PLL0FEED = 0xAA;                             // Feed Sequence Part #1
  PLL0FEED = 0x55;                             // Feed Sequence Part #2

  MEMMAP   = 0x01;                             // User Flash Mode

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
__attribute__((optimize("O0"))) int FlashEraseChip (void)
{
  int i;
  i = 50;
  IAP.cmd    = 50;                             // Prepare Sector for Erase
  IAP.par[0] = 0;                              // Start Sector
  IAP.par[1] = END_SECTOR;               	   // End Sector
  IAP_Call (&(IAP.cmd), &(IAP.stat));              // Call IAP Command
  if (IAP.stat) return (1);                    // Command Failed

  IAP.cmd    = 52;                             // Erase Sector
  IAP.par[0] = 0;                              // Start Sector
  IAP.par[1] = END_SECTOR;                     // End Sector
  IAP.par[2] = CCLK;                           // CCLK in kHz
  IAP_Call (&IAP.cmd, &IAP.stat);              // Call IAP Command
  if (IAP.stat) return (1);                    // Command Failed

  return (0);                                  // Finished without Errors
  
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
__attribute__((optimize("O0")))int FlashEraseSector (unsigned long sectorAddr)
{
  unsigned long n;

  n = GetSecNum(sectorAddr);                   // Get Sector Number

  IAP.cmd    = 50;                             // Prepare Sector for Erase
  IAP.par[0] = n;                              // Start Sector
  IAP.par[1] = n;                              // End Sector
  IAP_Call (&IAP.cmd, &IAP.stat);              // Call IAP Command
  if (IAP.stat) return (1);                    // Command Failed

  IAP.cmd    = 52;                             // Erase Sector
  IAP.par[0] = n;                              // Start Sector
  IAP.par[1] = n;                              // End Sector
  IAP.par[2] = CCLK;                           // CCLK in kHz
  IAP_Call (&IAP.cmd, &IAP.stat);              // Call IAP Command
  if (IAP.stat) return (1);                    // Command Failed

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
__attribute__((optimize("O0")))int FlashProgramPage (unsigned long pageAddr, unsigned long size, unsigned char *buf)
{
  unsigned long n;

#if SET_VALID_CODE != 0                        // Set valid User Code Signature
  if (pageAddr == 0) {                         // Check for Vector Table
    n = *((unsigned long *)(buf + 0x00)) +
        *((unsigned long *)(buf + 0x04)) +
        *((unsigned long *)(buf + 0x08)) +
        *((unsigned long *)(buf + 0x0C)) +
        *((unsigned long *)(buf + 0x10)) +
        *((unsigned long *)(buf + 0x14)) +
        *((unsigned long *)(buf + 0x18));
    *((unsigned long *)(buf + 0x1C)) = 0 - n;  // Signature at Reserved Vector
  }
#endif

  n = GetSecNum(pageAddr);                     // Get Sector Number

  IAP.cmd    = 50;                             // Prepare Sector for Write
  IAP.par[0] = n;                              // Start Sector
  IAP.par[1] = n;                              // End Sector
  IAP_Call (&IAP.cmd, &IAP.stat);              // Call IAP Command
  if (IAP.stat) return (1);                    // Command Failed

  IAP.cmd    = 51;                             // Copy RAM to Flash
  IAP.par[0] = pageAddr;                       // Destination Flash Address
  IAP.par[1] = (unsigned long)buf;             // Source RAM Address
  IAP.par[2] = 1024;                           // Fixed Page Size
  IAP.par[3] = CCLK;                           // CCLK in kHz
  IAP_Call (&IAP.cmd, &IAP.stat);              // Call IAP Command
  if (IAP.stat) return (1);                    // Command Failed

  return (0);                                  // Finished without Errors
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
__attribute__((optimize("O0")))int FlashVerify(unsigned long verifyAddr,unsigned long size,unsigned char *buf)
{
  // TODO: your code for the page verify 
  int i;
  unsigned long sum = 0;
  unsigned char* pageBuf = (unsigned char*)verifyAddr;
  
  // Gen CheckSum
  if(verifyAddr == 0)
  {
	  sum = *((unsigned long *)(buf + 0x00)) +
			*((unsigned long *)(buf + 0x04)) +
			*((unsigned long *)(buf + 0x08)) +
			*((unsigned long *)(buf + 0x0C)) +
			*((unsigned long *)(buf + 0x10)) +
			*((unsigned long *)(buf + 0x14)) +
			*((unsigned long *)(buf + 0x18));
		
	  *((unsigned long *)(buf + 0x1C)) = 0 - sum;  // Signature at Reserved Vector
  }
		
  for(i = 0; i < size; i++)
  {
	if(pageBuf[i] != buf[i])
	{
		return (1);
	}
  }
   
  return (0);
}



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
int FlashBlankCheck (unsigned long checkAddr,unsigned long size)
{
  #define JUMP_VECT_SIZE  0x400UL
  unsigned char* pageBuf = (unsigned char*)checkAddr;
  unsigned  long i;
  unsigned  long resize = size;
  //int sectorNum = GetSecNum(checkAddr);
  if (checkAddr < JUMP_VECT_SIZE) {
    if ((checkAddr + size) > JUMP_VECT_SIZE) {
	  resize = size - (JUMP_VECT_SIZE - checkAddr);
	  pageBuf = (unsigned char*)JUMP_VECT_SIZE;
	}
	else 
	{
	  return (0);
	} 
	  
  }
  
  for(i = 0; i < resize; i++)
  {
	if(pageBuf[i] != 0xFF)
	{
		return (1);
	}
  }
  return (0);
}
