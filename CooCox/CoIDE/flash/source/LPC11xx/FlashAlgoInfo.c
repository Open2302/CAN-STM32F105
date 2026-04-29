/**
 *******************************************************************************
 * @file       FlashAlgoInfo.c
 * @version    V0.3
 * @date       2011.05.16
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



#if defined ( LPC122x_128 )
#if defined ( __GNUC__   )
__attribute__ ((section(".driver_info")))
#endif
struct FlashAlgorithm const FlashDriver = {
	((FLASH_FRAMEWORK_VERSION << 16) | FLASH_ALGORITHM_VERSION),
	"LPC122x IAP 128KB Flash",
	{
		FLASH_TYPE_ONCHIP,
		0x00000000,                 // Device Start Address
		0x00020000,                 // Device Size in Bytes (128kB)
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




#if defined ( LPC122x_96 )
#if defined ( __GNUC__   )
__attribute__ ((section(".driver_info")))
#endif
struct FlashAlgorithm const FlashDriver = {
	((FLASH_FRAMEWORK_VERSION << 16) | FLASH_ALGORITHM_VERSION),
	"LPC122x IAP 96KB Flash",
	{
		FLASH_TYPE_ONCHIP,
		0x00000000,                 // Device Start Address
		0x00018000,                 // Device Size in Bytes (96kB)
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




#if defined ( LPC122x_80 )
#if defined ( __GNUC__   )
__attribute__ ((section(".driver_info")))
#endif
struct FlashAlgorithm const FlashDriver = {
	((FLASH_FRAMEWORK_VERSION << 16) | FLASH_ALGORITHM_VERSION),
	"LPC122x IAP 80KB Flash",
	{
		FLASH_TYPE_ONCHIP,
		0x00000000,                 // Device Start Address
		0x00014000,                 // Device Size in Bytes (80kB)
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




#if defined ( LPC122x_64 )
#if defined ( __GNUC__   )
__attribute__ ((section(".driver_info")))
#endif
struct FlashAlgorithm const FlashDriver = {
	((FLASH_FRAMEWORK_VERSION << 16) | FLASH_ALGORITHM_VERSION),
	"LPC122x IAP 64KB Flash",
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


#if defined ( LPC122x_48 )
#if defined ( __GNUC__   )
__attribute__ ((section(".driver_info")))
#endif
struct FlashAlgorithm const FlashDriver = {
	((FLASH_FRAMEWORK_VERSION << 16) | FLASH_ALGORITHM_VERSION),
	"LPC122x IAP 48KB Flash",
	{
		FLASH_TYPE_ONCHIP,
		0x00000000,                 // Device Start Address
		0x0000C000,                 // Device Size in Bytes (48kB)
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



//----------------------------------------------------------------------------
/////////////////////////////////////////////////////////////////////////////

#if defined ( LPC11xx_32 )
#if defined ( __GNUC__   )
__attribute__ ((section(".driver_info")))
#endif
struct FlashAlgorithm const FlashDriver = {
	((FLASH_FRAMEWORK_VERSION << 16) | FLASH_ALGORITHM_VERSION),
	"LPC11xx/122x/13xx IAP 32KB Flash",
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


#if defined ( LPC11xx_24 )
#if defined ( __GNUC__   )
__attribute__ ((section(".driver_info")))
#endif
struct FlashAlgorithm const FlashDriver = {
	((FLASH_FRAMEWORK_VERSION << 16) | FLASH_ALGORITHM_VERSION),
	"LPC11xx/13xx IAP 24KB Flash",
	{
		FLASH_TYPE_ONCHIP,
		0x00000000,                 // Device Start Address
		0x00006000,                 // Device Size in Bytes (32kB)
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

#if defined ( LPC11xx_16 )
#if defined ( __GNUC__   )
__attribute__ ((section(".driver_info")))
#endif
struct FlashAlgorithm const FlashDriver = {
	((FLASH_FRAMEWORK_VERSION << 16) | FLASH_ALGORITHM_VERSION),
	"LPC11xx/13xx IAP 16KB Flash",
	{
		FLASH_TYPE_ONCHIP,
		0x00000000,                 // Device Start Address
		0x00004000,                 // Device Size in Bytes (32kB)
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

#if defined ( LPC11xx_8 )
#if defined ( __GNUC__   )
__attribute__ ((section(".driver_info")))
#endif
struct FlashAlgorithm const FlashDriver = {
	((FLASH_FRAMEWORK_VERSION << 16) | FLASH_ALGORITHM_VERSION),
	"LPC11xx/13xx IAP 8KB Flash",
	{
		FLASH_TYPE_ONCHIP,
		0x00000000,                 // Device Start Address
		0x00002000,                 // Device Size in Bytes (32kB)
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












