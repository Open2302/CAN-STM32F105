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

#if defined ( STM32F10x_LD_16 )
#if defined ( __GNUC__   )
__attribute__ ((section(".driver_info")))
#endif
struct FlashAlgorithm const FlashDriver = {
	((FLASH_FRAMEWORK_VERSION << 16) | FLASH_ALGORITHM_VERSION),
	"STM32F10x Low-density 16KB Flash",
	{
		FLASH_TYPE_ONCHIP,
		0x08000000,                 // Device Start Address
		0x00004000,                 // Device Size in Bytes
		1024,                       // Programming Page Size
		0xFF,                       // Initial Content of Erased Memory
		{
			{ 0x0400, 0x00000000 },   
			{ FLASH_SECTOR_END   },
		},
	},
	5000,												// Time Out of FlashInit/FlashUnInit/FlashVerify/FlashBlankCheck function (in msec)
	5000,    											// Time Out of FlashProgramage  Function (in msec)
	5000,  												// Time Out of FlashEraseSector Function (in msec)
	5000, 												// Time Out of FlashEraseChip   Function (in msec)

};
#endif

#if defined ( STM32F10x_LD_32 )
#if defined ( __GNUC__   )
__attribute__ ((section(".driver_info")))
#endif
struct FlashAlgorithm const FlashDriver = {
	((FLASH_FRAMEWORK_VERSION << 16) | FLASH_ALGORITHM_VERSION),
	"STM32F10x Low-density 32KB Flash",
	{
		FLASH_TYPE_ONCHIP,
		0x08000000,                 // Device Start Address
		0x00008000,                 // Device Size in Bytes
		1024,                       // Programming Page Size
		0xFF,                       // Initial Content of Erased Memory
		{
			{ 0x0400, 0x00000000 },   
			{ FLASH_SECTOR_END   },
		},
	},
	5000,												// Time Out of FlashInit/FlashUnInit/FlashVerify/FlashBlankCheck function (in msec)
	5000,    											// Time Out of FlashProgramage  Function (in msec)
	5000,  												// Time Out of FlashEraseSector Function (in msec)
	5000, 												// Time Out of FlashEraseChip   Function (in msec)

};
#endif

#if defined ( STM32F10x_XLD_768 )
#if defined ( __GNUC__   )
__attribute__ ((section(".driver_info")))
#endif
struct FlashAlgorithm const FlashDriver = {
	((FLASH_FRAMEWORK_VERSION << 16) | FLASH_ALGORITHM_VERSION),
	"STM32F10x XLow-density 768KB Flash",
	{
		FLASH_TYPE_ONCHIP,
		0x08000000,                 // Device Start Address
		0x000C0000,                 // Device Size in Bytes
		1024,                       // Programming Page Size
		0xFF,                       // Initial Content of Erased Memory
		{
			{ 0x0800, 0x00000000 },   
			{ FLASH_SECTOR_END   },
		},
	},
	5000,												// Time Out of FlashInit/FlashUnInit/FlashVerify/FlashBlankCheck function (in msec)
	5000,    											// Time Out of FlashProgramage  Function (in msec)
	5000,  												// Time Out of FlashEraseSector Function (in msec)
	5000, 												// Time Out of FlashEraseChip   Function (in msec)

};
#endif

#if defined ( STM32F10x_XLD_1024 )
#if defined ( __GNUC__   )
__attribute__ ((section(".driver_info")))
#endif
struct FlashAlgorithm const FlashDriver = {
	((FLASH_FRAMEWORK_VERSION << 16) | FLASH_ALGORITHM_VERSION),
	"STM32F10x XLow-density 1MB Flash",
	{
		FLASH_TYPE_ONCHIP,
		0x08000000,                 // Device Start Address
		0x00100000,                 // Device Size in Bytes
		1024,                       // Programming Page Size
		0xFF,                       // Initial Content of Erased Memory
		{
			{ 0x0800, 0x00000000 },   
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

#if defined ( STM32F10x_MD_32 )
#if defined ( __GNUC__   )
__attribute__ ((section(".driver_info")))
#endif
struct FlashAlgorithm const FlashDriver = {
	((FLASH_FRAMEWORK_VERSION << 16) | FLASH_ALGORITHM_VERSION),
	"STM32F10x Med-density 32KB Flash",
	{
		FLASH_TYPE_ONCHIP,
		0x08000000,                 // Device Start Address
		0x00008000,                 // Device Size in Bytes
		1024,                       // Programming Page Size
		0xFF,                       // Initial Content of Erased Memory
		{
			{ 0x0400, 0x00000000 },   
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
#if defined ( STM32F10x_MD_64 )
#if defined ( __GNUC__   )
__attribute__ ((section(".driver_info")))
#endif
struct FlashAlgorithm const FlashDriver = {
	((FLASH_FRAMEWORK_VERSION << 16) | FLASH_ALGORITHM_VERSION),
	"STM32F10x Med-density 64KB Flash",	
	{
		FLASH_TYPE_ONCHIP,
		0x08000000,                 // Device Start Address
		0x00010000,                 // Device Size in Bytes
		1024,                       // Programming Page Size
		0xFF,                       // Initial Content of Erased Memory
		{
			{ 0x0400, 0x00000000 },   
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

#if defined ( STM32F10x_MD_128 )
#if defined ( __GNUC__   )
__attribute__ ((section(".driver_info")))
#endif
struct FlashAlgorithm const FlashDriver = {
	((FLASH_FRAMEWORK_VERSION << 16) | FLASH_ALGORITHM_VERSION),
	"STM32F10x Med-density 128KB Flash",
	{
		FLASH_TYPE_ONCHIP,
		0x08000000,                 // Device Start Address
		0x00020000,                 // Device Size in Bytes
		1024,                       // Programming Page Size
		0xFF,                       // Initial Content of Erased Memory
		{
			{ 0x0400, 0x00000000 },   
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

#if defined ( STM32F10x_HD_256 )
#if defined ( __GNUC__   )
__attribute__ ((section(".driver_info")))
#endif
struct FlashAlgorithm const FlashDriver = {
	((FLASH_FRAMEWORK_VERSION << 16) | FLASH_ALGORITHM_VERSION),
	"STM32F10x High-density 256KB Flash",
	{
		FLASH_TYPE_ONCHIP,
		0x08000000,                 // Device Start Address
		0x00040000,                 // Device Size in Bytes
		1024,                       // Programming Page Size
		0xFF,                       // Initial Content of Erased Memory
		{
			{ 0x0800, 0x00000000 },
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

#if defined ( STM32F10x_HD_384 )
#if defined ( __GNUC__   )
__attribute__ ((section(".driver_info")))
#endif
struct FlashAlgorithm const FlashDriver = {
	((FLASH_FRAMEWORK_VERSION << 16) | FLASH_ALGORITHM_VERSION),
	"STM32F10x High-density 384KB Flash",
	{
		FLASH_TYPE_ONCHIP,
		0x08000000,                 // Device Start Address
		0x00060000,                 // Device Size in Bytes
		1024,                       // Programming Page Size
		0xFF,                       // Initial Content of Erased Memory
		{
			{ 0x0800, 0x00000000 },
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

#if defined ( STM32F10x_HD_512 )
#if defined ( __GNUC__   )
__attribute__ ((section(".driver_info")))
#endif
struct FlashAlgorithm const FlashDriver = {
	((FLASH_FRAMEWORK_VERSION << 16) | FLASH_ALGORITHM_VERSION),
	"STM32F10x High-density 512KB Flash",  
	{
		FLASH_TYPE_ONCHIP,
		0x08000000,                 // Device Start Address
		0x00080000,                 // Device Size in Bytes
		1024,                       // Programming Page Size
		0xFF,                       // Initial Content of Erased Memory
		{
			{ 0x0800, 0x00000000 },
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

#if defined ( STM32F10x_CL_64 )
#if defined ( __GNUC__   )
__attribute__ ((section(".driver_info")))
#endif
struct FlashAlgorithm const FlashDriver = {
	((FLASH_FRAMEWORK_VERSION << 16) | FLASH_ALGORITHM_VERSION),
	"STM32F10x Connectivity Line 64KB Flash", 
	{
		FLASH_TYPE_ONCHIP,
		0x08000000,                 // Device Start Address
		0x00010000,                 // Device Size in Bytes
		1024,                       // Programming Page Size
		0xFF,                       // Initial Content of Erased Memory
		{
			{ 0x0800, 0x00000000 },
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
#if defined ( STM32F10x_CL_128 )
#if defined ( __GNUC__   )
__attribute__ ((section(".driver_info")))
#endif
struct FlashAlgorithm const FlashDriver = {
	((FLASH_FRAMEWORK_VERSION << 16) | FLASH_ALGORITHM_VERSION),
	"STM32F10x Connectivity Line 128KB Flash", 
	{
		FLASH_TYPE_ONCHIP,
		0x08000000,                 // Device Start Address
		0x00020000,                 // Device Size in Bytes
		1024,                       // Programming Page Size
		0xFF,                       // Initial Content of Erased Memory
		{
			{ 0x0800, 0x00000000 },
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

#if defined ( STM32F10x_CL_256 )
#if defined ( __GNUC__   )
__attribute__ ((section(".driver_info")))
#endif
struct FlashAlgorithm const FlashDriver = {
	((FLASH_FRAMEWORK_VERSION << 16) | FLASH_ALGORITHM_VERSION),
	"STM32F10x Connectivity Line 256KB Flash", 
	{
		FLASH_TYPE_ONCHIP,
		0x08000000,                 // Device Start Address
		0x00040000,                 // Device Size in Bytes
		1024,                       // Programming Page Size
		0xFF,                       // Initial Content of Erased Memory
		{
			{ 0x0800, 0x00000000 },
			{ FLASH_SECTOR_END   },
		},
	},
	5000,												// Time Out of FlashInit/FlashUnInit/FlashVerify/FlashBlankCheck function (in msec)
	5000,    											// Time Out of FlashProgramage  Function (in msec)
	5000,  												// Time Out of FlashEraseSector Function (in msec)
	5000, 												// Time Out of FlashEraseChip   Function (in msec)
};
#endif


