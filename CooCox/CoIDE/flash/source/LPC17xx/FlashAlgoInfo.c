/**
 *******************************************************************************
 * @file       FlashAlgoInfo.c
 * @version    V0.3
 * @date       2010.12.02
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

#define FLASH_ALGORITHM_VERSION 0x0030



/////////////////////////////////////////////////////////////////////////////

#if defined ( LPC17xx_32 )
#if defined ( __GNUC__   )
__attribute__ ((section(".driver_info")))
#endif
struct FlashAlgorithm const FlashDriver = {
	((FLASH_FRAMEWORK_VERSION << 16) | FLASH_ALGORITHM_VERSION),
	"LPC17xx IAP 32KB Flash",
	{
		FLASH_TYPE_ONCHIP,
		0x00000000,                 // Device Start Address
		0x00008000,                 // Device Size in Bytes (32kB)
		1024,                       // Programming Page Size
		0xFF,                       // Initial Content of Erased Memory
		{
			{ 0x001000, 0x00000000 }, // Sector Size  4kB
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

#if defined ( LPC17xx_64 )
#if defined ( __GNUC__   )
__attribute__ ((section(".driver_info")))
#endif
struct FlashAlgorithm const FlashDriver = {
	((FLASH_FRAMEWORK_VERSION << 16) | FLASH_ALGORITHM_VERSION),
	"LPC17xx IAP 64KB Flash",
	{
		FLASH_TYPE_ONCHIP,
		0x00000000,                 // Device Start Address
		0x00010000,                 // Device Size in Bytes (64kB)
		1024,                       // Programming Page Size
		0xFF,                       // Initial Content of Erased Memory
		{
			{ 0x001000, 0x00000000 }, // Sector Size  4kB
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

#if defined ( LPC17xx_128 )
#if defined ( __GNUC__   )
__attribute__ ((section(".driver_info")))
#endif
struct FlashAlgorithm const FlashDriver = {
	((FLASH_FRAMEWORK_VERSION << 16) | FLASH_ALGORITHM_VERSION),
	"LPC17xx IAP 128KB Flash",
	{
		FLASH_TYPE_ONCHIP,
		0x00000000,                 // Device Start Address
		0x00020000,                 // Device Size in Bytes (128kB)
		1024,                       // Programming Page Size
		0xFF,                       // Initial Content of Erased Memory
		{
			{ 0x001000, 0x00000000 },
			{ 0x008000, 0x00010000 },
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

#if defined ( LPC17xx_256 )
#if defined ( __GNUC__   )
__attribute__ ((section(".driver_info")))
#endif
struct FlashAlgorithm const FlashDriver = {
	((FLASH_FRAMEWORK_VERSION << 16) | FLASH_ALGORITHM_VERSION),
	"LPC17xx IAP 256KB Flash",
	{
		FLASH_TYPE_ONCHIP,
		0x00000000,                 // Device Start Address
		0x00040000,                 // Device Size in Bytes (256kB)
		1024,                       // Programming Page Size
		0xFF,                       // Initial Content of Erased Memory
		{
			{ 0x001000, 0x00000000 },
			{ 0x008000, 0x00010000 },
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

#if defined ( LPC17xx_512 )
#if defined ( __GNUC__   )
__attribute__ ((section(".driver_info")))
#endif
struct FlashAlgorithm const FlashDriver = {
	((FLASH_FRAMEWORK_VERSION << 16) | FLASH_ALGORITHM_VERSION),
	"LPC17xx IAP 512KB Flash",
	{
		FLASH_TYPE_ONCHIP,
		0x00000000,                 // Device Start Address
		0x00080000,                 // Device Size in Bytes (512kB)
		1024,                       // Programming Page Size
		0xFF,                       // Initial Content of Erased Memory
		{
		  
			{ 0x001000, 0x00000000 },
			{ 0x008000, 0x00010000 },
			{ FLASH_SECTOR_END     },
		},
	},
	5000,												// Time Out of FlashInit/FlashUnInit/FlashVerify/FlashBlankCheck function (in msec)
	5000,    											// Time Out of FlashProgramage  Function (in msec)
	5000,  												// Time Out of FlashEraseSector Function (in msec)
	5000, 												// Time Out of FlashEraseChip   Function (in msec)
};
#endif









