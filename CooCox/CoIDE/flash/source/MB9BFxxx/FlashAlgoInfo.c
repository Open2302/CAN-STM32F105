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

#if defined ( MB9BF500_256 )
#if defined ( __GNUC__   )
__attribute__ ((section(".driver_info")))
#endif
struct FlashAlgorithm const FlashDriver = {
	((FLASH_FRAMEWORK_VERSION << 16) | FLASH_ALGORITHM_VERSION),
	"MB9BF500 256kB Flash",
	{
		FLASH_TYPE_ONCHIP,
		0x00000000,                 // Device Start Address
		0x00040000,                 // Device Size in Bytes
		1024,                       // Programming Page Size
		0xFF,                       // Initial Content of Erased Memory
		{
			{ 0x001000, 0x000000 },
			{ 0x00C000, 0x004000 },
			{ 0x010000, 0x010000 },
			{ FLASH_SECTOR_END   },
		},
	},
	5000,												// Time Out of FlashInit/FlashUnInit/FlashVerify/FlashBlankCheck function (in msec)
	5000,    											// Time Out of FlashProgramage  Function (in msec)
	5000,  												// Time Out of FlashEraseSector Function (in msec)
	5000, 												// Time Out of FlashEraseChip   Function (in msec)

};
#endif

/////////////////////////////////////////////////////////////////////////////




/////////////////////////////////////////////////////////////////////////////

#if defined ( MB9BFxx1_64 )
#if defined ( __GNUC__   )
__attribute__ ((section(".driver_info")))
#endif
struct FlashAlgorithm const FlashDriver = {
	((FLASH_FRAMEWORK_VERSION << 16) | FLASH_ALGORITHM_VERSION),
	"MB9xFxx1 64kB Flash",
	{
		FLASH_TYPE_ONCHIP,
		0x00000000,                 // Device Start Address
		0x00010000,                 // Device Size in Bytes
		1024,                       // Programming Page Size
		0xFF,                       // Initial Content of Erased Memory
		{
			{ 0x001000, 0x000000 },
			{ 0x008000, 0x008000 },
			{ FLASH_SECTOR_END   },
		},
	},
	5000,												// Time Out of FlashInit/FlashUnInit/FlashVerify/FlashBlankCheck function (in msec)
	5000,    											// Time Out of FlashProgramage  Function (in msec)
	5000,  												// Time Out of FlashEraseSector Function (in msec)
	5000, 												// Time Out of FlashEraseChip   Function (in msec)

};
#endif

/////////////////////////////////////////////////////////////////////////////



/////////////////////////////////////////////////////////////////////////////

#if defined ( MB9BFxx2_128 )
#if defined ( __GNUC__   )
__attribute__ ((section(".driver_info")))
#endif
struct FlashAlgorithm const FlashDriver = {
	((FLASH_FRAMEWORK_VERSION << 16) | FLASH_ALGORITHM_VERSION),
	"MB9xFxx2 128kB Flash",
	{
		FLASH_TYPE_ONCHIP,
		0x00000000,                 // Device Start Address
		0x00020000,                 // Device Size in Bytes
		1024,                       // Programming Page Size
		0xFF,                       // Initial Content of Erased Memory
		{
			{ 0x001000, 0x000000 },
			{ 0x018000, 0x008000 },
			{ FLASH_SECTOR_END   },
		},
	},
	5000,												// Time Out of FlashInit/FlashUnInit/FlashVerify/FlashBlankCheck function (in msec)
	5000,    											// Time Out of FlashProgramage  Function (in msec)
	5000,  												// Time Out of FlashEraseSector Function (in msec)
	5000, 												// Time Out of FlashEraseChip   Function (in msec)

};
#endif

/////////////////////////////////////////////////////////////////////////////




/////////////////////////////////////////////////////////////////////////////

#if defined ( MB9BFxx4_256 )
#if defined ( __GNUC__   )
__attribute__ ((section(".driver_info")))
#endif
struct FlashAlgorithm const FlashDriver = {
	((FLASH_FRAMEWORK_VERSION << 16) | FLASH_ALGORITHM_VERSION),
	"MB9BFxx4 256kB Flash",
	{
		FLASH_TYPE_ONCHIP,
		0x00000000,                 // Device Start Address
		0x00040000,                 // Device Size in Bytes
		1024,                       // Programming Page Size
		0xFF,                       // Initial Content of Erased Memory
		{
			{ 0x001000, 0x000000 },
			{ 0x018000, 0x008000 },
			{ 0x020000, 0x020000 },
			{ FLASH_SECTOR_END   },
		},
	},
	5000,												// Time Out of FlashInit/FlashUnInit/FlashVerify/FlashBlankCheck function (in msec)
	5000,    											// Time Out of FlashProgramage  Function (in msec)
	5000,  												// Time Out of FlashEraseSector Function (in msec)
	5000, 												// Time Out of FlashEraseChip   Function (in msec)

};
#endif

/////////////////////////////////////////////////////////////////////////////




/////////////////////////////////////////////////////////////////////////////

#if defined ( MB9BFxx5_384 )
#if defined ( __GNUC__   )
__attribute__ ((section(".driver_info")))
#endif
struct FlashAlgorithm const FlashDriver = {
	((FLASH_FRAMEWORK_VERSION << 16) | FLASH_ALGORITHM_VERSION),
	"MB9BFxx5 384kB Flash",
	{
		FLASH_TYPE_ONCHIP,
		0x00000000,                 // Device Start Address
		0x00060000,                 // Device Size in Bytes
		1024,                       // Programming Page Size
		0xFF,                       // Initial Content of Erased Memory
		{
			{ 0x001000, 0x000000 },
			{ 0x018000, 0x008000 },
			{ 0x020000, 0x020000 },
			{ FLASH_SECTOR_END   },
		},
	},
	5000,												// Time Out of FlashInit/FlashUnInit/FlashVerify/FlashBlankCheck function (in msec)
	5000,    											// Time Out of FlashProgramage  Function (in msec)
	5000,  												// Time Out of FlashEraseSector Function (in msec)
	5000, 												// Time Out of FlashEraseChip   Function (in msec)

};
#endif

/////////////////////////////////////////////////////////////////////////////




/////////////////////////////////////////////////////////////////////////////

#if defined ( MB9BFxx6_512 )
#if defined ( __GNUC__   )
__attribute__ ((section(".driver_info")))
#endif
struct FlashAlgorithm const FlashDriver = {
	((FLASH_FRAMEWORK_VERSION << 16) | FLASH_ALGORITHM_VERSION),
	"MB9BFxx6 512kB Flash",
	{
		FLASH_TYPE_ONCHIP,
		0x00000000,                 // Device Start Address
		0x00080000,                 // Device Size in Bytes
		1024,                       // Programming Page Size
		0xFF,                       // Initial Content of Erased Memory
		{
			{ 0x001000, 0x000000 },
			{ 0x018000, 0x008000 },
			{ 0x020000, 0x020000 },
			{ FLASH_SECTOR_END   },
		},
	},
	5000,												// Time Out of FlashInit/FlashUnInit/FlashVerify/FlashBlankCheck function (in msec)
	5000,    											// Time Out of FlashProgramage  Function (in msec)
	5000,  												// Time Out of FlashEraseSector Function (in msec)
	5000, 												// Time Out of FlashEraseChip   Function (in msec)

};
#endif

/////////////////////////////////////////////////////////////////////////////


/////////////////////////////////////////////////////////////////////////////

#if defined ( MB9BFxx7_768 )
#if defined ( __GNUC__   )
__attribute__ ((section(".driver_info")))
#endif
struct FlashAlgorithm const FlashDriver = {
	((FLASH_FRAMEWORK_VERSION << 16) | FLASH_ALGORITHM_VERSION),
	"MB9BFxx7 768kB Flash",
	{
		FLASH_TYPE_ONCHIP,
		0x00000000,                 // Device Start Address
		0x000C0000,                 // Device Size in Bytes
		1024,                       // Programming Page Size
		0xFF,                       // Initial Content of Erased Memory
		{
			{ 0x001000, 0x000000 },
			{ 0x018000, 0x008000 },
			{ 0x020000, 0x020000 },
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

#if defined ( MB9BFxx8_1024 )
#if defined ( __GNUC__   )
__attribute__ ((section(".driver_info")))
#endif
struct FlashAlgorithm const FlashDriver = {
	((FLASH_FRAMEWORK_VERSION << 16) | FLASH_ALGORITHM_VERSION),
	"MB9BFxx8 1024kB Flash",
	{
		FLASH_TYPE_ONCHIP,
		0x00000000,                 // Device Start Address
		0x00100000,                 // Device Size in Bytes
		1024,                       // Programming Page Size
		0xFF,                       // Initial Content of Erased Memory
		{
			{ 0x001000, 0x000000 },
			{ 0x018000, 0x008000 },
			{ 0x020000, 0x020000 },
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

#if defined ( MB9xFxxx_32WF )
#if defined ( __GNUC__   )
__attribute__ ((section(".driver_info")))
#endif
struct FlashAlgorithm const FlashDriver = {
	((FLASH_FRAMEWORK_VERSION << 16) | FLASH_ALGORITHM_VERSION),
	"MB9xFxxx 32kB WorkFlash",
	{
		FLASH_TYPE_ONCHIP,
		0x200C0000,                 // Device Start Address
		0x00008000,                 // Device Size in Bytes
		1024,                       // Programming Page Size
		0xFF,                       // Initial Content of Erased Memory
		{
			{ 0x002000, 0x000000 },
			{ FLASH_SECTOR_END   },
		},
	},
	5000,												// Time Out of FlashInit/FlashUnInit/FlashVerify/FlashBlankCheck function (in msec)
	5000,    											// Time Out of FlashProgramage  Function (in msec)
	5000,  												// Time Out of FlashEraseSector Function (in msec)
	5000, 												// Time Out of FlashEraseChip   Function (in msec)

};
#endif

/////////////////////////////////////////////////////////////////////////////
