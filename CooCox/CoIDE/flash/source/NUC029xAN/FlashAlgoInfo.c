/**
 *******************************************************************************
 * @file       FlashAlgoInfo.c
 * @version    V0.3
 * @date       2012.12.29
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


#if defined ( NUC029xAN_64 )
#if defined ( __GNUC__   )
__attribute__ ((section(".driver_info")))
#endif
struct FlashAlgorithm const FlashDriver = {
	((FLASH_FRAMEWORK_VERSION << 16) | FLASH_ALGORITHM_VERSION),
	"NUC029xAN 64KB Flash",
	{
		FLASH_TYPE_ONCHIP,
		0x00000000,                 // Device Start Address
		0x00010000,                 // Device Size in Bytes
		512,                        // Programming Page Size
		0xFF,                       // Initial Content of Erased Memory
		{
			{ 0x0200, 0x00000000 }, 
			{ FLASH_SECTOR_END   },
		},
	},
	5000,												// Time Out of FlashInit/FlashUnInit/FlashVerify/FlashBlankCheck function (in msec)
	5000,    											// Time Out of FlashProgramage  Function (in msec)
	5000,  												// Time Out of FlashEraseSector Function (in msec)
	10000, 												// Time Out of FlashEraseChip   Function (in msec)

};
#endif

/////////////////////////////////////////////////////////////////////////////


/////////////////////////////////////////////////////////////////////////////

#if defined ( NUC029xAN_32 )
#if defined ( __GNUC__   )
__attribute__ ((section(".driver_info")))
#endif
struct FlashAlgorithm const FlashDriver = {
	((FLASH_FRAMEWORK_VERSION << 16) | FLASH_ALGORITHM_VERSION),
	"NUC029xAN 32KB Flash",
	{
		FLASH_TYPE_ONCHIP,
		0x00000000,                 // Device Start Address
		0x00008000,                 // Device Size in Bytes
		512,                        // Programming Page Size
		0xFF,                       // Initial Content of Erased Memory
		{
			{ 0x0200, 0x00000000 },   
			{ FLASH_SECTOR_END   },
		},
	},
	5000,												// Time Out of FlashInit/FlashUnInit/FlashVerify/FlashBlankCheck function (in msec)
	5000,    											// Time Out of FlashProgramage  Function (in msec)
	5000,  												// Time Out of FlashEraseSector Function (in msec)
	10000, 												// Time Out of FlashEraseChip   Function (in msec)

};
#endif


/////////////////////////////////////////////////////////////////////////////

#if defined ( NUC029xAN_16 )
#if defined ( __GNUC__   )
__attribute__ ((section(".driver_info")))
#endif
struct FlashAlgorithm const FlashDriver = {
	((FLASH_FRAMEWORK_VERSION << 16) | FLASH_ALGORITHM_VERSION),
	"NUC029xAN 16KB Flash",
	{
		FLASH_TYPE_ONCHIP,
		0x00000000,                 // Device Start Address
		0x00004000,                 // Device Size in Bytes
		512,                        // Programming Page Size
		0xFF,                       // Initial Content of Erased Memory
		{
			{ 0x0200, 0x00000000 },   
			{ FLASH_SECTOR_END   },
		},
	},
	5000,												// Time Out of FlashInit/FlashUnInit/FlashVerify/FlashBlankCheck function (in msec)
	5000,    											// Time Out of FlashProgramage  Function (in msec)
	5000,  												// Time Out of FlashEraseSector Function (in msec)
	10000, 												// Time Out of FlashEraseChip   Function (in msec)

};
#endif




/////////////////////////////////////////////////////////////////////////////

