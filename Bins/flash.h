#ifndef __FLASH_H
#define __FLASH_H 

#include "at45db161d.h"
#include "stm32f4xx_hal.h"
#include "spi.h"
//#include "Bin2.h"
/*
menu
-------------------------------------------------
MAVTO   - 0x000000 - 0x0257FF
MDINR   - 0x025800 - 0x04AFFF
MENU    - 0x04B000 - 0x0707FF
MKAVI   - 0x070800 - 0x095FFF
MSTAT   - 0x096000 - 0x0BB7FF
MULZV   - 0x0BB800 - 0x0E0FFF
MUSTA1a - 0x0E1000 - 0x1067FF
TestF   - 0x106800 - 0x12BFFF
------------------------------------------------------

knob

STARO    - 0x12C000 - 0x12CFBF
STARZ    - 0x12CFC0 - 0x12DF7F
STOPO    - 0x12DF80 - 0x12EF3F
STOPZ    - 0x12EF40 - 0x12FEF0
Knob_r   - 0x12FF00 - 0x1300D3
Knob_s   - 0x1300D4 - 0x1302A7
SLeft_r  - 0x1302A8 - 0x13047B
SLeft_s  - 0x13047C - 0x13064F
SRight_r - 0x130650 - 0x130823
SRight_s - 0x130824 - 0x1309F7

---------------------------------------------------------

strelki

SLSZE   - 0x1309F8 - 0x130AFF
STRE1   - 0x130B00 - 0x130C5F
NSTRE   - 0x130C60 - 0x130DBF

--------------------------------------------------------

progressbar

POGRS   - 0x130DC0 - 0x131AFF
POGRV   - 0x131B00 - 0x132E7F
NLSZE   - 0x132E80 - 0x132F87

--------------------------------------------------

24 files							22528
*/

#define PAGE_SIZES     528
#define BLOCK_SIZES    4224
#define SECTOR_SIZES_0 4224			// SECTOR_0 = 4224			
#define SECTOR_SIZES   130944		// SECTOR_x = 130944



void PrintBin(uint32_t addres, uint16_t x, uint16_t y, uint16_t w, uint16_t h);
void LoadFlash(uint8_t *Datas, uint32_t addres, uint32_t sizeblock, uint8_t erase);










#endif
