/**
 *******************************************************************************
 * @file       FlashAlgoInfo.c
 * @version    V0.1
 * @date       2011.03.17
 * @brief      Flash  Algorithm and Flash Device Description.	
 *******************************************************************************
 * @copy
 *
 * INTERNAL FILE,DON'T PUBLIC.
 * 
 * <h2><center>&copy; COPYRIGHT 2009 CooCox </center></h2>
 *******************************************************************************
 */
#include <FlashAlgorithm.h>

#define FLASH_ALGORITHM_VERSION 0x0010


/////////////////////////////////////////////////////////////////////////////

#if defined ( HT32F_127 )
#if defined ( __GNUC__   )
__attribute__ ((section(".driver_info")))
#endif
struct FlashAlgorithm const FlashDriver = {
	((FLASH_FRAMEWORK_VERSION << 16) | FLASH_ALGORITHM_VERSION),
	"HT32F 127KB Flash",
	{
		FLASH_TYPE_ONCHIP,
		0x00000000,                 // Device Start Address
		0x0001FC00,                 // Device Size in Bytes (127kB)
		1024,                       // Programming Page Size
		0xFF,                       // Initial Content of Erased Memory
		{
			{ 0x000400, 0x00000000 }, // Sector Size  1kB(Page Erase)
			{ FLASH_SECTOR_END     },
		},
	},
	10000,												// Time Out of FlashInit/FlashUnInit/FlashVerify/FlashBlankCheck function (in msec)
	10000,    											// Time Out of FlashProgramage  Function (in msec)
	10000,  											// Time Out of FlashEraseSector Function (in msec)
	10000, 												// Time Out of FlashEraseChip   Function (in msec)
};
#endif


/////////////////////////////////////////////////////////////////////////////

#if defined ( HT32F_31 )
#if defined ( __GNUC__   )
__attribute__ ((section(".driver_info")))
#endif
struct FlashAlgorithm const FlashDriver = {
	((FLASH_FRAMEWORK_VERSION << 16) | FLASH_ALGORITHM_VERSION),
	"HT32F 31KB Flash",
	{
		FLASH_TYPE_ONCHIP,
		0x00000000,                 // Device Start Address
		0x00007C00,                 // Device Size in Bytes (31kB)
		1024,                       // Programming Page Size
		0xFF,                       // Initial Content of Erased Memory
		{
			{ 0x000400, 0x00000000 }, // Sector Size  1kB(Page Erase)
			{ FLASH_SECTOR_END     },
		},
	},
	5000,												// Time Out of FlashInit/FlashUnInit/FlashVerify/FlashBlankCheck function (in msec)
	5000,    											// Time Out of FlashProgramage  Function (in msec)
	5000,  												// Time Out of FlashEraseSector Function (in msec)
	5000, 												// Time Out of FlashEraseChip   Function (in msec)
};
#endif


/////////////////////////////////////////////////////////////////////////////

#if defined ( HT32F_16 )
#if defined ( __GNUC__   )
__attribute__ ((section(".driver_info")))
#endif
struct FlashAlgorithm const FlashDriver = {
	((FLASH_FRAMEWORK_VERSION << 16) | FLASH_ALGORITHM_VERSION),
	"HT32F 16KB Flash",
	{
		FLASH_TYPE_ONCHIP,
		0x00000000,                 // Device Start Address
		0x00004000,                 // Device Size in Bytes (16kB)
		1024,                       // Programming Page Size
		0xFF,                       // Initial Content of Erased Memory
		{
			{ 0x000400, 0x00000000 }, // Sector Size  1kB(Page Erase)
			{ FLASH_SECTOR_END     },
		},
	},
	5000,												// Time Out of FlashInit/FlashUnInit/FlashVerify/FlashBlankCheck function (in msec)
	5000,    											// Time Out of FlashProgramage  Function (in msec)
	5000,  												// Time Out of FlashEraseSector Function (in msec)
	5000, 												// Time Out of FlashEraseChip   Function (in msec)
};
#endif


/////////////////////////////////////////////////////////////////////////////

#if defined ( HT32F_8 )
#if defined ( __GNUC__   )
__attribute__ ((section(".driver_info")))
#endif
struct FlashAlgorithm const FlashDriver = {
	((FLASH_FRAMEWORK_VERSION << 16) | FLASH_ALGORITHM_VERSION),
	"HT32F 8KB Flash",
	{
		FLASH_TYPE_ONCHIP,
		0x00000000,                 // Device Start Address
		0x00002000,                 // Device Size in Bytes (8kB)
		1024,                       // Programming Page Size
		0xFF,                       // Initial Content of Erased Memory
		{
			{ 0x000400, 0x00000000 }, // Sector Size  1kB(Page Erase)
			{ FLASH_SECTOR_END     },
		},
	},
	5000,												// Time Out of FlashInit/FlashUnInit/FlashVerify/FlashBlankCheck function (in msec)
	5000,    											// Time Out of FlashProgramage  Function (in msec)
	5000,  												// Time Out of FlashEraseSector Function (in msec)
	5000, 												// Time Out of FlashEraseChip   Function (in msec)
};
#endif


/////////////////////////////////////////////////////////////////////////////












