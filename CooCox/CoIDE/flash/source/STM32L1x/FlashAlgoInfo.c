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

#define FLASH_ALGORITHM_VERSION 0x0020


/////////////////////////////////////////////////////////////////////////////

#if defined ( STM32L1xx_128 )
#if defined ( __GNUC__   )
__attribute__ ((section(".driver_info")))
#endif
struct FlashAlgorithm const FlashDriver = {
	((FLASH_FRAMEWORK_VERSION << 16) | FLASH_ALGORITHM_VERSION),
	"STM32L1xx Med-density 128K Flash",
	{
		FLASH_TYPE_ONCHIP,
		0x08000000,                 // Device Start Address
		0x00020000,                 // Device Size in Bytes
		256,                       // Programming Page Size
		0xFF,                       // Initial Content of Erased Memory
		{
			{ 0x0100, 0x00000000 },
			{ FLASH_SECTOR_END   },
		},
	},
	5000,												// Time Out of FlashInit/FlashUnInit/FlashVerify/FlashBlankCheck function (in msec)
	5000,    											// Time Out of FlashProgramage  Function (in msec)
	5000,  												// Time Out of FlashEraseSector Function (in msec)
	5000, 												// Time Out of FlashEraseChip   Function (in msec)

};
#endif

#if defined ( STM32L1xx_256 )
#if defined ( __GNUC__   )
__attribute__ ((section(".driver_info")))
#endif
struct FlashAlgorithm const FlashDriver = {
	((FLASH_FRAMEWORK_VERSION << 16) | FLASH_ALGORITHM_VERSION),
	"STM32L1xx Med+-density 256K Flash",
	{
		FLASH_TYPE_ONCHIP,
		0x08000000,                 // Device Start Address
		0x00040000,                 // Device Size in Bytes
		256,                       // Programming Page Size
		0xFF,                       // Initial Content of Erased Memory
		{
			{ 0x0100, 0x00000000 },
			{ FLASH_SECTOR_END   },
		},
	},
	5000,												// Time Out of FlashInit/FlashUnInit/FlashVerify/FlashBlankCheck function (in msec)
	5000,    											// Time Out of FlashProgramage  Function (in msec)
	5000,  												// Time Out of FlashEraseSector Function (in msec)
	5000, 												// Time Out of FlashEraseChip   Function (in msec)

};
#endif

#if defined ( STM32L1xx_384 )
#if defined ( __GNUC__   )
__attribute__ ((section(".driver_info")))
#endif
struct FlashAlgorithm const FlashDriver = {
	((FLASH_FRAMEWORK_VERSION << 16) | FLASH_ALGORITHM_VERSION),
	"STM32L1xx High-density 384K Flash",
	{
		FLASH_TYPE_ONCHIP,
		0x08000000,                 // Device Start Address
		0x00060000,                 // Device Size in Bytes
		256,                       // Programming Page Size
		0xFF,                       // Initial Content of Erased Memory
		{
			{ 0x0100, 0x00000000 },
			{ FLASH_SECTOR_END   },
		},
	},
	5000,												// Time Out of FlashInit/FlashUnInit/FlashVerify/FlashBlankCheck function (in msec)
	5000,    											// Time Out of FlashProgramage  Function (in msec)
	5000,  												// Time Out of FlashEraseSector Function (in msec)
	5000, 												// Time Out of FlashEraseChip   Function (in msec)

};
#endif


#if defined ( STM32L1xx_EEPROM )
#if defined ( __GNUC__   )
__attribute__ ((section(".driver_info")))
#endif
struct FlashAlgorithm const FlashDriver = {
	((FLASH_FRAMEWORK_VERSION << 16) | FLASH_ALGORITHM_VERSION),
	"STM32L1xx Data EEPROM",
	{
		FLASH_TYPE_ONCHIP,
		0x08080000,                 // Device Start Address
		0x00001000,                 // Device Size in Bytes
		256,                       // Programming Page Size
		0xFF,                       // Initial Content of Erased Memory
		{
			{ 0x0100, 0x00000000 },
			{ FLASH_SECTOR_END   },
		},
	},
	5000,												// Time Out of FlashInit/FlashUnInit/FlashVerify/FlashBlankCheck function (in msec)
	5000,    											// Time Out of FlashProgramage  Function (in msec)
	5000,  												// Time Out of FlashEraseSector Function (in msec)
	5000, 												// Time Out of FlashEraseChip   Function (in msec)

};
#endif
