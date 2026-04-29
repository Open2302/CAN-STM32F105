/**
 *******************************************************************************
 * @file       FlashAlgoFncs.c
 * @version    V0.3
 * @date       2010.12.02
 * @brief      Flash  Algorithm For NUC1xx with 128/64/32KB Flash Rom	
 *******************************************************************************
 * @copy
 *
 * INTERNAL FILE,DON'T PUBLIC.
 * 
 * <h2><center>&copy; COPYRIGHT 2009 CooCox </center></h2>
 *******************************************************************************
 */

#include <FlashAlgorithm.h>

typedef volatile unsigned long    vu32;
typedef          unsigned long     u32;
typedef volatile unsigned short   vu16;
typedef          unsigned short    u16;

#define M16(adr) (*((vu16 *) (adr)))

// Peripheral Memory Map
#define WFIF_BASE          (0x200E0000)
#define FIF_BASE           (0x40000000)
#define HWWDT_BASE         (0x40011000)

#define FIF               ((FIF_TypeDef  *) FIF_BASE)
#define WFIF              ((WFIF_TypeDef *) WFIF_BASE)
#define HWWDT             ((WDT_TypeDef  *) HWWDT_BASE)


typedef struct {             // Flash Interface
  vu32 FASZR;                // Offset: 0x000 (R/W) Flash Access Size Register
  vu32 FRWTR;                // Offset: 0x004 (R/W) Flash Read Wait Register
  vu32 FSTR;                 // Offset: 0x008 (R/W) Flash Status Register
  vu32 RESERVED0;
  vu32 FSYNDN;               // Offset: 0x010 (R/W) Flash Sync Down Register
} FIF_TypeDef;

typedef struct {             // WorkFlash Interface
  vu32 WFASZR;               // Offset: 0x000 (R/W) WorkFlash Access Size Register
  vu32 WFRWTR;               // Offset: 0x004 (R/W) WorkFlash Read Wait Register
  vu32 WFSTR;                // Offset: 0x008 (R/W) WorkFlash Status Register
} WFIF_TypeDef;

typedef struct {             // Watchdog Timer
  vu32 LDR;                  // offset: 0x000 (R/W) Watchdog Timer Load Register
  vu32 VLR;                  // Offset: 0x004 (R/W) Watchdog Timer Value Register
  vu32 CTL;                  // Offset: 0x008 (R/W) Watchdog Timer Control Register
  vu32 ICL;                  // Offset: 0x00C (R/W) Watchdog Timer Clear Register
  vu32 RIS;                  // Offset: 0x010 (R/W) Watchdog Timer Interrupt Status Register
  vu32 RESERVED0[763];
  vu32 LCK;                  // Offset: 0xC00 (R/W) Watchdog Timer Lock Register
} WDT_TypeDef;


/* MB9BF500 device
  -------------+-----------+-------------+-------------+-------------+-------------+-------------+--------------
               |           |  1st write  |  2nd write  |  3rd write  |  4th write  |  5th write  |  6th write
     Command   | Number of |-------------+-------------+-------------+-------------+-------------+--------------
               | writes    | Addr.| Data | Addr.| Data | Addr.| Data | Addr.| Data | Addr.| Data | Addr.| Data
  -------------+-----------+------+------+------+------+------+------+------+------+------+------+------+-------
  Read/Reset         1      0xXXX  0xF0   RA     RD     --     --     --     --     --     --     --     --
  Write              4      0xAA8  0xAA   0x554  0x55   0xAA8  0xA0   PA     PD     --     --     --     --
  Chip erase         6      0xAA8  0xAA   0x554  0x55   0xAA8  0x80   0xAA8  0xAA   0x554  0x55   0xAA8  0x10
  Sector erase       6      0xAA8  0xAA   0x554  0x55   0xAA8  0x80   0xAA8  0xAA   0x554  0x55   SA     0x30
 */

/* MB9BFxxx WorkFlash
  -------------+-----------+-------------+-------------+-------------+-------------+-------------+--------------
               |           |  1st write  |  2nd write  |  3rd write  |  4th write  |  5th write  |  6th write
     Command   | Number of |-------------+-------------+-------------+-------------+-------------+--------------
               | writes    | Addr.| Data | Addr.| Data | Addr.| Data | Addr.| Data | Addr.| Data | Addr.| Data
  -------------+-----------+------+------+------+------+------+------+------+------+------+------+------+-------
  Read/Reset         1      0xXXX  0xF0   RA     RD     --     --     --     --     --     --     --     --
  Write              4      0xAA8  0xAA   0x554  0x55   0xAA8  0xA0   PA     PD     --     --     --     --
  Chip erase         6      0xAA8  0xAA   0x554  0x55   0xAA8  0x80   0xAA8  0xAA   0x554  0x55   0xAA8  0x10
  Sector erase       6      0xAA8  0xAA   0x554  0x55   0xAA8  0x80   0xAA8  0xAA   0x554  0x55   SA     0x30
 */

/* MB9BFx0x devices
  -------------+-----------+-------------+-------------+-------------+-------------+-------------+--------------
               |           |  1st write  |  2nd write  |  3rd write  |  4th write  |  5th write  |  6th write
     Command   | Number of |-------------+-------------+-------------+-------------+-------------+--------------
               | writes    | Addr.| Data | Addr.| Data | Addr.| Data | Addr.| Data | Addr.| Data | Addr.| Data
  -------------+-----------+------+------+------+------+------+------+------+------+------+------+------+-------
  Read/Reset         1      0xXXX  0xF0   RA     RD     --     --     --     --     --     --     --     --
  Write              4      0x1550 0xAA   0x0AA8 0x55   0x1550 0xA0   PA     PD     --     --     --     --
  Chip erase         6      0x1550 0xAA   0x0AA8 0x55   0x1550 0x80   0x1550 0xAA   0x0AA8 0x55   0x1550 0x10
  Sector erase       6      0x1550 0xAA   0x0AA8 0x55   0x1550 0x80   0x1550 0xAA   0x0AA8 0x55   SA     0x30
 */

/* define Address for command write cycle */
#ifdef MB9BF500_256
  #define ADDR_1st    0x0AA8
  #define ADDR_2nd    0x0554
  #define ADDR_3rd    0x0AA8
  #define ADDR_4th    0x0AA8
  #define ADDR_5th    0x0554
  #define ADDR_6th    0x0AA8
#elif defined (MB9xFxxx_32WF)
  #define ADDR_1st    0x0AA8
  #define ADDR_2nd    0x0554
  #define ADDR_3rd    0x0AA8
  #define ADDR_4th    0x0AA8
  #define ADDR_5th    0x0554
  #define ADDR_6th    0x0AA8
#else
#define ADDR_1st    0x0AA8
#define ADDR_2nd    0x0554
#define ADDR_3rd    0x0AA8
#define ADDR_4th    0x0AA8
#define ADDR_5th    0x0554
#define ADDR_6th    0x0AA8
#endif

union fsreg {                  // Flash Status Register
  struct b  {
    unsigned int q0:1;
    unsigned int q1:1;
    unsigned int q2:1;
    unsigned int q3:1;
    unsigned int q4:1;
    unsigned int q5:1;
    unsigned int q6:1;
    unsigned int q7:1;
  } b;
  unsigned int v;
} fsr;

unsigned long base_adr;



/**
 *******************************************************************************
 * @brief      Check if Program/Erase completed
 * @param[in]  adr     Flash device base address.
 * @param[out] None
 * @retval     0   					All is OK.
 * @retval     others       Some error occurs.
 *
 * @par Description
 * @details
 * @note
 *******************************************************************************
 */

int Polling (unsigned long adr)
{
  unsigned int q6;

  fsr.v = M16(adr);
  q6 = fsr.b.q6;
  do {
    fsr.v = M16(adr);
    if (fsr.b.q6 == q6) return (0);  // Done
    q6 = fsr.b.q6;
  } while (fsr.b.q5 == 0);           // Check for Timeout
  fsr.v = M16(adr);
  q6 = fsr.b.q6;
  fsr.v = M16(adr);
  if (fsr.b.q6 == q6) return (0);    // Done
  M16(adr) = 0xF0;                   // Reset Device
  return (1);                        // Failed
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

int FlashInit(unsigned long baseAddr, unsigned long clk,
		      unsigned long operateFuc)
{
	HWWDT->LCK = 0x1ACCE551;
	HWWDT->LCK = 0xE5331AAE;
	HWWDT->CTL = 0;                                       // Disable Watchdog

	base_adr = baseAddr;

	return (0);                                           // Done
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

int FlashEraseSector(unsigned long sectorAddr)
{
  vu32 dummy;

#ifdef MB9xFxxx_32WF                                    // MB9xFxxx 32KB WorkFlash
  WFIF->WFASZR = 0x00;                                  // set CPU programming mode
  dummy = WFIF->WFASZR;                                 // dummy read
#else
  FIF->FASZR = 0x01;                                    // set CPU programming mode
  dummy = FIF->FASZR;                                   // dummy read
#endif

  // Start Erase Sector Command
  M16(base_adr + ADDR_1st) = 0xAA;
  M16(base_adr + ADDR_2nd) = 0x55;
  M16(base_adr + ADDR_3rd) = 0x80;
  M16(base_adr + ADDR_4th) = 0xAA;
  M16(base_adr + ADDR_5th) = 0x55;
  M16(sectorAddr)     = 0x30;                                  // erase fist   internal sector
#if !defined (MB9BF500_256) && !defined (MB9xFxxx_32WF)
  M16(sectorAddr + 4) = 0x30;                                  // erase second internal sector
#endif
  do {
	fsr.v = M16(sectorAddr);
  } while (fsr.b.q3 == 0);                              // Wait for Sector Erase Timeout

  if (Polling(sectorAddr) != 0) return (1);                    // Wait until Erase completed

#ifdef MB9xFxxx_32WF                                    // MB9xFxxx 32KB WorkFlash
  WFIF->WFASZR = 0x01;                                  // set CPU ROM mode
  dummy = WFIF->WFASZR;                                 // dummy read
#else
  FIF->FASZR = 0x02;                                    // set CPU ROM mode
  dummy = FIF->FASZR;                                   // dummy read
#endif

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
vu32 dummy;
#ifdef MB9xFxxx_32WF                                    // MB9xFxxx 32KB WorkFlash
  WFIF->WFASZR = 0x00;                                  // set CPU programming mode
  dummy = WFIF->WFASZR;                                 // dummy read
#else
  FIF->FASZR = 0x01;                                    // set CPU programming mode
  dummy = FIF->FASZR;                                   // dummy read
#endif
  // Start Chip Erase Command
  M16(base_adr + ADDR_1st) = 0xAA;
  M16(base_adr + ADDR_2nd) = 0x55;
  M16(base_adr + ADDR_3rd) = 0x80;
  M16(base_adr + ADDR_4th) = 0xAA;
  M16(base_adr + ADDR_5th) = 0x55;
  M16(base_adr + ADDR_6th) = 0x10;

  if (Polling(base_adr) != 0) return (1);               // Wait until Erase completed

#ifdef MB9xFxxx_32WF                                    // MB9xFxxx 32KB WorkFlash
  WFIF->WFASZR = 0x01;                                  // set CPU ROM mode
  dummy = WFIF->WFASZR;                                 // dummy read
#else
  FIF->FASZR = 0x02;                                    // set CPU ROM mode
  dummy = FIF->FASZR;                                   // dummy read
#endif

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
int FlashProgramPage(unsigned long pageAddr, unsigned long size, unsigned char *buf)
{
  int  i;
  vu32 dummy;

#ifdef MB9xFxxx_32WF                                    // MB9xFxxx 32KB WorkFlash
  WFIF->WFASZR = 0x00;                                  // set CPU programming mode
  dummy = WFIF->WFASZR;                                 // dummy read
#else
  FIF->FASZR = 0x01;                                    // set CPU programming mode
  dummy = FIF->FASZR;                                   // dummy read
#endif

  for (i = 0; i < ((size+1)/2); i++)  {
	// Start Program Command
	M16(base_adr + ADDR_1st) = 0xAA;
	M16(base_adr + ADDR_2nd) = 0x55;
	M16(base_adr + ADDR_3rd) = 0xA0;
	M16(pageAddr) = *((u16 *) buf);
	if (Polling(pageAddr) != 0) return (1);
	buf += 2;
	pageAddr += 2;
  }
#ifdef MB9xFxxx_32WF                                    // MB9xFxxx 32KB WorkFlash
  WFIF->WFASZR = 0x01;                                  // set CPU ROM mode
  dummy = WFIF->WFASZR;                                 // dummy read
#else
  FIF->FASZR = 0x02;                                    // set CPU ROM mode
  dummy = FIF->FASZR;                                   // dummy read
#endif
  return (0);
}


