/**
 * AT45DB161D module for arduino (C) Vincent
 *   SPI flash memory
 *   http://blog.blockos.org/?p=27
 *
 * bug fix by todotani
 *   http://todotani.cocolog-nifty.com/blog/2009/07/arduino-4cf4.html
 *
 * Modified for mbed, 2011 Suga.
 */
#include "at45db161d.h"
#include <stdlib.h>

typedef struct _ID
{
  uint8_t manufacturer;       /**< Manufacturer id                           **/
  uint8_t device[2];          /**< Device id                                 **/
  uint8_t extendedInfoLength; /**< Extended device information string length **/
} ID;
ID *id;

//ID *id;
void AT45_ReadManufacturerAndDeviceID(ID *id);//(void);//(ID *id);
 
/** CTOR **/
//00014 ATD45DB161D::ATD45DB161D(PinName mosi, PinName miso, PinName sclk, PinName cs)
//  : _spi(mosi, miso, sclk), _cs(cs)
//{}
 
//ATD45DB161D::ATD45DB161D(SPI &spi, PinName cs)
//  : _spi(spi), _cs(cs)
//{}
 
/** DTOR **/
//00023 ATD45DB161D::~ATD45DB161D()
//{}
 

 
/** 
 * Read status register
 * @return The content of the status register
 **/
uint8_t AT45_ReadStatusRegister()
{
    uint8_t status = 0;
 
    HAL_GPIO_WritePin(GPIOD, FLASH_CS_Pin, GPIO_PIN_SET);//DF_CS_inactive;    /* Make sure to toggle CS signal in order */
    HAL_GPIO_WritePin(GPIOD, FLASH_CS_Pin, GPIO_PIN_RESET);//DF_CS_active;      /* to reset Dataflash command decoder     */
  
    /* Send status read command */
  //  spi_transfer(AT45DB161D_STATUS_REGISTER_READ);
	status = AT45DB161D_STATUS_REGISTER_READ;
	HAL_SPI_Transmit(&hspi2, &status, 1, 100);
    /* Get result with a dummy write */
  //  status = spi_transfer(0x00);
	HAL_SPI_Receive(&hspi2, &status, 1, 100);
 
    return status;
}
 
/** 
 * Read Manufacturer and Device ID 
 * @note if id.extendedInfoLength is not equal to zero,
 *       successive calls to spi_transfer(0xff) will return
 *       the extended device information string bytes.
 * @param id Pointer to the ID structure to initialize
 **/
void AT45_ReadManufacturerAndDeviceID(ID *id)//(void)//(ID *id)
{
	uint8_t Data;
//   ID *id;
	//id = (ID*)malloc(sizeof(ID));
	
  HAL_GPIO_WritePin(GPIOD, FLASH_CS_Pin, GPIO_PIN_SET);//DF_CS_inactive;    /* Make sure to toggle CS signal in order */
  HAL_GPIO_WritePin(GPIOD, FLASH_CS_Pin, GPIO_PIN_RESET);//DF_CS_active;      /* to reset Dataflash command decoder     */
  
    /* Send status read command */
	//Data = 0x9F;	
	Data = AT45DB161D_READ_MANUFACTURER_AND_DEVICE_ID;
	HAL_SPI_Transmit(&hspi2, &Data, 1, 100);
	
    /* Manufacturer ID */
	HAL_SPI_Receive(&hspi2, &Data, 1, 100);
	id->manufacturer = Data;
	//HAL_SPI_Receive(&hspi1, &id->manufacturer, 1, 100);	
	Data = id->manufacturer;
		/* Device ID (part 1) */
	HAL_SPI_Receive(&hspi2, &Data, 1, 100);
	id->device[0] = Data;
	//HAL_SPI_Receive(&hspi1, &id->device[0], 1, 100);	
	Data = id->device[0];
    /* Device ID (part 2) */
	HAL_SPI_Receive(&hspi2, &Data, 1, 100);
	id->device[1] = Data;
	//HAL_SPI_Receive(&hspi1, &id->device[1], 1, 100);	
	Data = id->device[1];
    /* Extended Device Information String Length */
	HAL_SPI_Receive(&hspi2, &Data, 1, 100);
	id->extendedInfoLength = Data;
	//HAL_SPI_Receive(&hspi1, &id->extendedInfoLength, 1, 100);	
	Data = id->extendedInfoLength;
    
}



 
/** 
	* Основная память Страница пользователя.
  * Основная страница памяти чтения позволяет пользователю считывать данные непосредственно из
  * Любой одной из страниц 4096 в основной памяти, минуя оба из
  * Буферы данных и оставляя содержимое буферов без изменений.
  *
  * @param page Страница основной памяти для чтения
  * @param offset начальный адрес байта внутри страницы
 **/
void AT45_ReadMainMemoryPage(uint16_t page, uint16_t offset)
{
	uint8_t Data;
    HAL_GPIO_WritePin(GPIOD, FLASH_CS_Pin, GPIO_PIN_SET);//DF_CS_inactive;    /* Make sure to toggle CS signal in order */
    HAL_GPIO_WritePin(GPIOD, FLASH_CS_Pin, GPIO_PIN_RESET);//DF_CS_active;      /* to reset Dataflash command decoder     */
 
    /* Send opcode */
   // spi_transfer(AT45DB161D_PAGE_READ);
	Data = AT45DB161D_PAGE_READ;
	HAL_SPI_Transmit(&hspi2, &Data, 1, 100);
    
    /* Address (page | offset)  */
    //spi_transfer((uint8_t)(page >> 6));																// 290>>6= 0x8804(34820)   E489
	Data = page >> 6;													// 290>>6 = 4
	HAL_SPI_Transmit(&hspi2, &Data, 1, 100);
    //spi_transfer((uint8_t)((page << 2) | (offset >> 8)));
	Data = ((page << 2) | (offset >> 8));			//290<<2=136 | 1   = 5
	HAL_SPI_Transmit(&hspi2, &Data, 1, 100);			//1161
    //spi_transfer((uint8_t)(offset & 0xff));
	Data = offset & 0xff;												//480&FF=224
	HAL_SPI_Transmit(&hspi2, &Data, 1, 100);
    
    /* 4 "don't care" bytes */
    //spi_transfer(0x00);
		Data = 0x00;
	HAL_SPI_Transmit(&hspi2, &Data, 1, 100);
    //spi_transfer(0x00);
	HAL_SPI_Transmit(&hspi2, &Data, 1, 100);
    //spi_transfer(0x00);
	HAL_SPI_Transmit(&hspi2, &Data, 1, 100);
    //spi_transfer(0x00);
	HAL_SPI_Transmit(&hspi2, &Data, 1, 100);
}
 
/** 
	* Непрерывный массив чтения.
  * Последовательно читать непрерывный поток данных.
  * @param page Страница из основной памяти, где начнется последовательное чтение
  * @param offset начальный адрес байта внутри страницы
  * @param low Если это правда операция чтения будет выполняться в режиме низкой скорости (и в режиме высокой скорости, если оно ложно).
  * @note Устаревший режим не поддерживается
 **/
void AT45_ContinuousArrayRead(uint16_t page, uint16_t offset, uint8_t low)
{
	uint8_t Data;
    HAL_GPIO_WritePin(GPIOD, FLASH_CS_Pin, GPIO_PIN_SET);//DF_CS_inactive;    /* Make sure to toggle CS signal in order */
    HAL_GPIO_WritePin(GPIOD, FLASH_CS_Pin, GPIO_PIN_RESET);//DF_CS_active;      /* to reset Dataflash command decoder     */
 
    /* Send opcode */
    //spi_transfer( low ? AT45DB161D_CONTINUOUS_READ_LOW_FREQ :
    //                    AT45DB161D_CONTINUOUS_READ_HIGH_FREQ );
	Data = (low ? AT45DB161D_CONTINUOUS_READ_LOW_FREQ : AT45DB161D_CONTINUOUS_READ_HIGH_FREQ );
	HAL_SPI_Transmit(&hspi2, &Data, 1, 100);
 
    /* Address (page | offset)  */
    //spi_transfer((uint8_t)(page >> 6));
	Data = page >> 6;
	HAL_SPI_Transmit(&hspi2, &Data, 1, 100);
    //spi_transfer((uint8_t)((page << 2) | (offset >> 8)));
	Data = ((page << 2) | (offset >> 8));
	HAL_SPI_Transmit(&hspi2, &Data, 1, 100);
    //spi_transfer((uint8_t)(offset & 0xff));
	Data = offset & 0xff;
	HAL_SPI_Transmit(&hspi2, &Data, 1, 100);
  Data = 0x00;  
    if (!low) HAL_SPI_Transmit(&hspi2, &Data, 1, 100);		//{ spi_transfer(0x00); }
		

}
 
 
/** 
 * Read the content of one of the SRAM data buffers (in low or high speed mode).
 * @param bufferNum Buffer to read (1 or 2)
 * @param offset Starting byte within the buffer
 * @param low If true the read operation will be performed in low speed mode (and in high speed mode if it's false).
 **/
void AT45_BufferRead(uint8_t bufferNum, uint16_t offset, uint8_t low)
{
	uint8_t Data;
    HAL_GPIO_WritePin(GPIOD, FLASH_CS_Pin, GPIO_PIN_SET);//DF_CS_inactive;    /* Make sure to toggle CS signal in order */
    HAL_GPIO_WritePin(GPIOD, FLASH_CS_Pin, GPIO_PIN_RESET);//DF_CS_active;      /* to reset Dataflash command decoder     */
 
    /* Send opcode */
    if(bufferNum == 1)
    {
        //spi_transfer(low ? AT45DB161D_BUFFER_1_READ_LOW_FREQ :
        //                   AT45DB161D_BUFFER_1_READ);
			Data = (low ? AT45DB161D_BUFFER_1_READ_LOW_FREQ : AT45DB161D_BUFFER_1_READ);
			HAL_SPI_Transmit(&hspi2, &Data, 1, 100);
    }
    else
    {
        //spi_transfer(low ? AT45DB161D_BUFFER_2_READ_LOW_FREQ :
        //                   AT45DB161D_BUFFER_2_READ);
			Data = (low ? AT45DB161D_BUFFER_2_READ_LOW_FREQ : AT45DB161D_BUFFER_2_READ);
			HAL_SPI_Transmit(&hspi2, &Data, 1, 100);
 
    }
    
    /* 14 "Don't care" bits */
    //spi_transfer(0x00);
		Data = 0x00;
		HAL_SPI_Transmit(&hspi2, &Data, 1, 100);
    /* Rest of the "don't care" bits + bits 8,9 of the offset */
    //spi_transfer((uint8_t)(offset >> 8));
		Data = offset >> 8;
		HAL_SPI_Transmit(&hspi2, &Data, 1, 100);
    /* bits 7-0 of the offset */
    //spi_transfer((uint8_t)(offset & 0xff));
		Data = offset & 0xff;
		HAL_SPI_Transmit(&hspi2, &Data, 1, 100);
}
 
/** 
 * Write data to one of the SRAM data buffers. Any further call to
 * spi_tranfer will return bytes contained in the data buffer until
 * a low-to-high transition is detected on the CS pin. If the end of
 * the data buffer is reached, the device will wrap around back to the
 * beginning of the buffer. 
 * @param bufferNum Buffer to read (1 or 2)
 * @param offset Starting byte within the buffer
 **/
void AT45_BufferWrite(uint8_t bufferNum, uint16_t offset)
{
	uint8_t Data;
    HAL_GPIO_WritePin(GPIOD, FLASH_CS_Pin, GPIO_PIN_SET);//DF_CS_inactive;    /* Make sure to toggle CS signal in order */
    HAL_GPIO_WritePin(GPIOD, FLASH_CS_Pin, GPIO_PIN_RESET);//DF_CS_active;      /* to reset Dataflash command decoder     */
 
    //spi_transfer( (bufferNum == 1) ? AT45DB161D_BUFFER_1_WRITE :
    //                                 AT45DB161D_BUFFER_2_WRITE);
	Data = ((bufferNum == 1) ? AT45DB161D_BUFFER_1_WRITE : AT45DB161D_BUFFER_2_WRITE);
	HAL_SPI_Transmit(&hspi2, &Data, 1, 100);
    
    /* 14 "Don't care" bits */
    //spi_transfer(0x00);
	Data = 0x00;
	HAL_SPI_Transmit(&hspi2, &Data, 1, 100);
	
    /* Rest of the "don't care" bits + bits 8,9 of the offset */
    //spi_transfer((uint8_t)(offset >> 8));
	Data = offset >> 8;
	HAL_SPI_Transmit(&hspi2, &Data, 1, 100);
    /* bits 7-0 of the offset */
    //spi_transfer((uint8_t)(offset & 0xff));
	Data = offset & 0xff;
	HAL_SPI_Transmit(&hspi2, &Data, 1, 100);
}
 
/**
 * Transfer data from buffer 1 or 2 to main memory page.
 * @param bufferNum Buffer to use (1 or 2)
 * @param page Page where the content of the buffer will transfered
 * @param erase If set the page will be first erased before the buffer transfer.
 * @note If erase is equal to zero, the page must have been previously erased using one of the erase command (Page or Block Erase).
 **/
void AT45_BufferToPage(uint8_t bufferNum, uint16_t page, uint8_t erase)
{
	uint8_t Data;
    HAL_GPIO_WritePin(GPIOD, FLASH_CS_Pin, GPIO_PIN_SET);//DF_CS_inactive;    /* Make sure to toggle CS signal in order */
    HAL_GPIO_WritePin(GPIOD, FLASH_CS_Pin, GPIO_PIN_RESET);//DF_CS_active;      /* to reset Dataflash command decoder     */
 
    /* Opcode */
    if(erase)
    {
        //spi_transfer( (bufferNum == 1) ? AT45DB161D_BUFFER_1_TO_PAGE_WITH_ERASE :
        //                                 AT45DB161D_BUFFER_2_TO_PAGE_WITH_ERASE);
			Data = ((bufferNum == 1) ? AT45DB161D_BUFFER_1_TO_PAGE_WITH_ERASE : AT45DB161D_BUFFER_2_TO_PAGE_WITH_ERASE);
			HAL_SPI_Transmit(&hspi2, &Data, 1, 100);
    }
    else
    {
        //spi_transfer( (bufferNum == 1) ? AT45DB161D_BUFFER_1_TO_PAGE_WITHOUT_ERASE :
        //                                 AT45DB161D_BUFFER_2_TO_PAGE_WITHOUT_ERASE);
			Data = ((bufferNum == 1) ? AT45DB161D_BUFFER_1_TO_PAGE_WITHOUT_ERASE : AT45DB161D_BUFFER_2_TO_PAGE_WITHOUT_ERASE);
			HAL_SPI_Transmit(&hspi2, &Data, 1, 100);
    }
    
    /*
     * 3 address bytes consist of :
     *     - 2 don&#65533;ft care bits
     *     - 12 page address bits (PA11 - PA0) that specify the page in 
     *       the main memory to be written
     *     - 10 don&#65533;ft care bits
     */
    //spi_transfer((uint8_t)(page >> 6));
		Data = page >> 6;
		HAL_SPI_Transmit(&hspi2, &Data, 1, 100);
    //spi_transfer((uint8_t)(page << 2));
		Data = page << 2;
		HAL_SPI_Transmit(&hspi2, &Data, 1, 100);
    //spi_transfer(0x00);
		Data = 0x00;
		HAL_SPI_Transmit(&hspi2, &Data, 1, 100);
    
    //DF_CS_inactive;  /* Start transfer */
    //DF_CS_active;    /* If erase was set, the page will first be erased */
		
		HAL_GPIO_WritePin(GPIOD, FLASH_CS_Pin, GPIO_PIN_SET);//DF_CS_inactive;    /* Make sure to toggle CS signal in order */
    HAL_GPIO_WritePin(GPIOD, FLASH_CS_Pin, GPIO_PIN_RESET);//DF_CS_active;      /* to reset Dataflash command decoder     */
 
    /* Wait for the end of the transfer */
      while(!(AT45_ReadStatusRegister() & READY_BUSY))
      {}
}
 
/**
 * Transfer a page of data from main memory to buffer 1 or 2.
 * @param page Main memory page to transfer
 * @param buffer Buffer (1 or 2) where the data will be written
 **/
void AT45_PageToBuffer(uint16_t page, uint8_t bufferNum)
{
	uint8_t Data;
    HAL_GPIO_WritePin(GPIOD, FLASH_CS_Pin, GPIO_PIN_SET);//DF_CS_inactive;    /* Make sure to toggle CS signal in order */
    HAL_GPIO_WritePin(GPIOD, FLASH_CS_Pin, GPIO_PIN_RESET);//DF_CS_active;      /* to reset Dataflash command decoder     */
 
    /* Send opcode */
    //spi_transfer((bufferNum == 1) ? AT45DB161D_TRANSFER_PAGE_TO_BUFFER_1 :
    //                                AT45DB161D_TRANSFER_PAGE_TO_BUFFER_2);
	Data = ((bufferNum == 1) ? AT45DB161D_TRANSFER_PAGE_TO_BUFFER_1 : AT45DB161D_TRANSFER_PAGE_TO_BUFFER_2);
	HAL_SPI_Transmit(&hspi2, &Data, 1, 100);
 
    /*
     * 3 address bytes consist of :
     *     - 2 don&#65533;ft care bits
     *     - 12 page address bits (PA11 - PA0) that specify the page in 
     *       the main memory to be written
     *     - 10 don&#65533;ft care bits
     */
    //spi_transfer((uint8_t)(page >> 6));
	Data = page >> 6;
	HAL_SPI_Transmit(&hspi2, &Data, 1, 100);
    //spi_transfer((uint8_t)(page << 2));
	Data = page << 2;
	HAL_SPI_Transmit(&hspi2, &Data, 1, 100);
    //spi_transfer(0x00);
	Data = 0x00;
	HAL_SPI_Transmit(&hspi2, &Data, 1, 100);
        
    //DF_CS_inactive;  /* Start page transfer */
    //DF_CS_active;
		
		HAL_GPIO_WritePin(GPIOD, FLASH_CS_Pin, GPIO_PIN_SET);//DF_CS_inactive;    /* Make sure to toggle CS signal in order */
    HAL_GPIO_WritePin(GPIOD, FLASH_CS_Pin, GPIO_PIN_RESET);//DF_CS_active;      /* to reset Dataflash command decoder     */
 
    /* Wait for the end of the transfer */
      while(!(AT45_ReadStatusRegister() & READY_BUSY))
      {}
}
 
/** 
 * Erase a page in the main memory array.
 * @param page Page to erase
 **/
void AT45_PageErase(uint16_t page)
{
	uint8_t Data;
    HAL_GPIO_WritePin(GPIOD, FLASH_CS_Pin, GPIO_PIN_SET);//DF_CS_inactive;    /* Make sure to toggle CS signal in order */
    HAL_GPIO_WritePin(GPIOD, FLASH_CS_Pin, GPIO_PIN_RESET);//DF_CS_active;      /* to reset Dataflash command decoder     */
 
    /* Send opcode */
    //spi_transfer(AT45DB161D_PAGE_ERASE);
	Data = AT45DB161D_PAGE_ERASE;
	HAL_SPI_Transmit(&hspi2, &Data, 1, 100);
    
    /*
     * 3 address bytes consist of :
     *     - 2 don&#65533;ft care bits
     *     - 12 page address bits (PA11 - PA0) that specify the page in 
     *       the main memory to be written
     *     - 10 don&#65533;ft care bits
     */
    //spi_transfer((uint8_t)(page >> 6));
	Data = page >> 6;
	HAL_SPI_Transmit(&hspi2, &Data, 1, 1000);
    //spi_transfer((uint8_t)(page << 2));
	Data = page << 2;
	HAL_SPI_Transmit(&hspi2, &Data, 1, 1000);
    //spi_transfer(0x00);
	Data = 0x00;
	HAL_SPI_Transmit(&hspi2, &Data, 1, 1000);
        
    //DF_CS_inactive;  /* Start block erase */
    //DF_CS_active;
		
		HAL_GPIO_WritePin(GPIOD, FLASH_CS_Pin, GPIO_PIN_SET);//DF_CS_inactive;    /* Make sure to toggle CS signal in order */
    HAL_GPIO_WritePin(GPIOD, FLASH_CS_Pin, GPIO_PIN_RESET);//DF_CS_active;      /* to reset Dataflash command decoder     */
 
    /* Wait for the end of the block erase operation */
      while(!(AT45_ReadStatusRegister() & READY_BUSY))
      {}
}
 
/**
 * Erase a block of eight pages at one time.
 * @param block Index of the block to erase
 **/
void AT45_BlockErase(uint16_t block)
{
	uint8_t Data;
    HAL_GPIO_WritePin(GPIOD, FLASH_CS_Pin, GPIO_PIN_SET);//DF_CS_inactive;    /* Make sure to toggle CS signal in order */
    HAL_GPIO_WritePin(GPIOD, FLASH_CS_Pin, GPIO_PIN_RESET);//DF_CS_active;      /* to reset Dataflash command decoder     */
 
    /* Send opcode */
    //spi_transfer(AT45DB161D_BLOCK_ERASE);
	Data = AT45DB161D_BLOCK_ERASE;
	HAL_SPI_Transmit(&hspi2, &Data, 1, 100);
    
    /*
     * 3 address bytes consist of :
     *     - 2 don&#65533;ft care bits
     *     - 9 block address bits (PA11 - PA3)
     *     - 13 don&#65533;ft care bits
     */
    //spi_transfer((uint8_t)(block >> 3));
	Data = block >> 3;
	HAL_SPI_Transmit(&hspi2, &Data, 1, 100);
    //spi_transfer((uint8_t)(block << 5));
	Data = block << 5;
	HAL_SPI_Transmit(&hspi2, &Data, 1, 100);
    //spi_transfer(0x00);
	Data = 0x00;
	HAL_SPI_Transmit(&hspi2, &Data, 1, 100);
        
    //DF_CS_inactive;  /* Start block erase */
    //DF_CS_active;
		
		HAL_GPIO_WritePin(GPIOD, FLASH_CS_Pin, GPIO_PIN_SET);//DF_CS_inactive;    /* Make sure to toggle CS signal in order */
    HAL_GPIO_WritePin(GPIOD, FLASH_CS_Pin, GPIO_PIN_RESET);//DF_CS_active;      /* to reset Dataflash command decoder     */
 
    /* Wait for the end of the block erase operation */
      while(!(AT45_ReadStatusRegister() & READY_BUSY))
      {}
}
 
/** 
 * Erase a sector in main memory. There are 16 sector on the
 * at45db161d and only one can be erased at one time.
 * @param sector Sector to erase (1-15)
 **/
void AT45_SectoreErase(uint8_t sector)
{
	uint8_t Data;
    HAL_GPIO_WritePin(GPIOD, FLASH_CS_Pin, GPIO_PIN_SET);//DF_CS_inactive;    /* Make sure to toggle CS signal in order */
    HAL_GPIO_WritePin(GPIOD, FLASH_CS_Pin, GPIO_PIN_RESET);//DF_CS_active;      /* to reset Dataflash command decoder     */
 
    /* Send opcode */
    //spi_transfer(AT45DB161D_SECTOR_ERASE);
	Data = AT45DB161D_SECTOR_ERASE;
	HAL_SPI_Transmit(&hspi2, &Data, 1, 100);
    
    /*
     * 3 address bytes consist of :
     */
    if((sector == 0x0A) || (sector == 0x0B))
    {
        /*
         *  - 11 don&#65533;ft care bits
         *  - 
         *  - 12 don&#65533;ft care bits
         */
        //spi_transfer(0x00);
			Data = 0x00;
			HAL_SPI_Transmit(&hspi2, &Data, 1, 100);
        //spi_transfer(((sector & 0x01) << 4));
			Data = (sector & 0x01) << 4;
			HAL_SPI_Transmit(&hspi2, &Data, 1, 100);
        //spi_transfer(0x00);
			Data = 0x00;
			HAL_SPI_Transmit(&hspi2, &Data, 1, 100);
    }
    else
    {
        /*
         *  - 2 don't care bits 
         *  - 4 sector number bits
         *  - 18 don't care bits 
         */
        //spi_transfer(sector << 1);
			Data = sector << 1;
			HAL_SPI_Transmit(&hspi2, &Data, 1, 100);
        //spi_transfer(0x00);
			Data = 0x00;
			HAL_SPI_Transmit(&hspi2, &Data, 1, 100);
        //spi_transfer(0x00);
			//Data = 
			HAL_SPI_Transmit(&hspi2, &Data, 1, 100);
    }
                
    //DF_CS_inactive;  /* Start block erase */
    //DF_CS_active;
		
		HAL_GPIO_WritePin(GPIOD, FLASH_CS_Pin, GPIO_PIN_SET);//DF_CS_inactive;    /* Make sure to toggle CS signal in order */
    HAL_GPIO_WritePin(GPIOD, FLASH_CS_Pin, GPIO_PIN_RESET);//DF_CS_active;      /* to reset Dataflash command decoder     */
 
    /* Wait for the end of the block erase operation */
      while(!(AT45_ReadStatusRegister() & READY_BUSY))
      {}
}
 
/** 
 * Erase the entire chip memory. Sectors proteced or locked down will
 * not be erased.
 **/
void AT45_ChipErase()
{
	uint8_t Data;
    HAL_GPIO_WritePin(GPIOD, FLASH_CS_Pin, GPIO_PIN_SET);//DF_CS_inactive;    /* Make sure to toggle CS signal in order */
    HAL_GPIO_WritePin(GPIOD, FLASH_CS_Pin, GPIO_PIN_RESET);//DF_CS_active;      /* to reset Dataflash command decoder     */
 
    /* Send chip erase sequence */
    //spi_transfer(AT45DB161D_CHIP_ERASE_0);
	Data = AT45DB161D_CHIP_ERASE_0;
	HAL_SPI_Transmit(&hspi2, &Data, 1, 100);
    //spi_transfer(AT45DB161D_CHIP_ERASE_1);
	Data = AT45DB161D_CHIP_ERASE_1;
	HAL_SPI_Transmit(&hspi2, &Data, 1, 100);
    //spi_transfer(AT45DB161D_CHIP_ERASE_2);
	Data = AT45DB161D_CHIP_ERASE_2;
	HAL_SPI_Transmit(&hspi2, &Data, 1, 100);
    //spi_transfer(AT45DB161D_CHIP_ERASE_3);
	Data = AT45DB161D_CHIP_ERASE_3;
	HAL_SPI_Transmit(&hspi2, &Data, 1, 100);
                
   // DF_CS_inactive;  /* Start chip erase */
   // DF_CS_active;
	
		HAL_GPIO_WritePin(GPIOD, FLASH_CS_Pin, GPIO_PIN_SET);//DF_CS_inactive;    /* Make sure to toggle CS signal in order */
    HAL_GPIO_WritePin(GPIOD, FLASH_CS_Pin, GPIO_PIN_RESET);//DF_CS_active;      /* to reset Dataflash command decoder     */
 
    /* Wait for the end of the chip erase operation */
      while(!(AT45_ReadStatusRegister() & READY_BUSY))
      {}
}
 
/**
 * This a combination of Buffer Write and Buffer to Page with
 * Built-in Erase.
 * @note You must call EndAndWait in order to start transfering data from buffer to page
 * @param page Page where the content of the buffer will transfered
 * @param offset Starting byte address within the buffer
 * @param bufferNum Buffer to use (1 or 2)
 * @warning UNTESTED
 **/
void AT45_BeginPageWriteThroughBuffer(uint16_t page, uint16_t offset, uint8_t bufferNum)
{
	uint8_t Data;
		HAL_GPIO_WritePin(GPIOD, FLASH_CS_Pin, GPIO_PIN_SET);//DF_CS_inactive;    /* Make sure to toggle CS signal in order */
    HAL_GPIO_WritePin(GPIOD, FLASH_CS_Pin, GPIO_PIN_RESET);//DF_CS_active;      /* to reset Dataflash command decoder     */
 
    /* Send opcode */
    //spi_transfer((bufferNum == 1) ? AT45DB161D_PAGE_THROUGH_BUFFER_1 :
    //                                AT45DB161D_PAGE_THROUGH_BUFFER_2);
	Data = ((bufferNum == 1) ? AT45DB161D_PAGE_THROUGH_BUFFER_1 : AT45DB161D_PAGE_THROUGH_BUFFER_2);
	HAL_SPI_Transmit(&hspi2, &Data, 1, 100);
 
    /* Address */
    //spi_transfer((uint8_t)(page >> 6));
	Data = page >> 6;
	HAL_SPI_Transmit(&hspi2, &Data, 1, 100);
    //spi_transfer((uint8_t)((page << 2) | (offset >> 8)));
	Data = ((page << 2) | (offset >> 8));
	HAL_SPI_Transmit(&hspi2, &Data, 1, 100);
    //spi_transfer((uint8_t)offset);
	Data = offset;
	HAL_SPI_Transmit(&hspi2, &Data, 1, 100);
}
 
/**
 * Perform a low-to-high transition on the CS pin and then poll
 * the status register to check if the dataflash is busy.
 **/
void AT45_EndAndWait(void)
{
  //  DF_CS_inactive;  /* End current operation */
  //  DF_CS_active;    /* Some internal operation may occur
  //                    * (buffer to page transfer, page erase, etc... ) */
	
		HAL_GPIO_WritePin(GPIOD, FLASH_CS_Pin, GPIO_PIN_SET);//DF_CS_inactive;    /* Make sure to toggle CS signal in order */
    HAL_GPIO_WritePin(GPIOD, FLASH_CS_Pin, GPIO_PIN_RESET);//DF_CS_active;      /* to reset Dataflash command decoder     */
 
    /* Wait for the chip to be ready */
      while(!(AT45_ReadStatusRegister() & READY_BUSY))
      {}
      
   // DF_CS_inactive;  /* Release SPI Bus */ 
		HAL_GPIO_WritePin(GPIOD, FLASH_CS_Pin, GPIO_PIN_SET);
}
 
/**
 * Compare a page of data in main memory to the data in buffer 1 or 2.
 * @param page Page to test
 * @param bufferNum Buffer number
 * @return
 *        - 1 if the page and the buffer contains the same data
 *         - 0 else
 * @warning UNTESTED
 **/
int8_t AT45_ComparePageToBuffer(uint16_t page, uint8_t bufferNum)
{
    uint8_t status; uint8_t Data;
    
		HAL_GPIO_WritePin(GPIOD, FLASH_CS_Pin, GPIO_PIN_SET);//DF_CS_inactive;    /* Make sure to toggle CS signal in order */
    HAL_GPIO_WritePin(GPIOD, FLASH_CS_Pin, GPIO_PIN_RESET);//DF_CS_active;      /* to reset Dataflash command decoder     */
 
    /* Send opcode */
    //spi_transfer((bufferNum == 1) ? AT45DB161D_COMPARE_PAGE_TO_BUFFER_1 :
    //                                AT45DB161D_COMPARE_PAGE_TO_BUFFER_2);
	Data = ((bufferNum == 1) ? AT45DB161D_COMPARE_PAGE_TO_BUFFER_1 : AT45DB161D_COMPARE_PAGE_TO_BUFFER_2);
	HAL_SPI_Transmit(&hspi2, &Data, 1, 100);
    
    /* Page address */
    //spi_transfer((uint8_t)(page >> 6));
	Data = page >> 6;
	HAL_SPI_Transmit(&hspi2, &Data, 1, 100);
    //spi_transfer((uint8_t)(page << 2));
	Data = page << 2;
	HAL_SPI_Transmit(&hspi2, &Data, 1, 100);
    //spi_transfer(0x00);
	Data = 0x00;
	HAL_SPI_Transmit(&hspi2, &Data, 1, 100);
    
  //  DF_CS_inactive;  /* Start comparaison */
  //  DF_CS_active;
	
		HAL_GPIO_WritePin(GPIOD, FLASH_CS_Pin, GPIO_PIN_SET);//DF_CS_inactive;    /* Make sure to toggle CS signal in order */
    HAL_GPIO_WritePin(GPIOD, FLASH_CS_Pin, GPIO_PIN_RESET);//DF_CS_active;      /* to reset Dataflash command decoder     */
 
    /* Wait for the end of the comparaison and get the result */
      while(!((status = AT45_ReadStatusRegister()) & READY_BUSY))
      {}
          
//      return ((status & COMPARE) == COMPARE);
      return ((status & COMPARE) ? 0 : 1);
}
 
/**
 * Put the device into the lowest power consumption mode.
 * Once the device has entered the Deep Power-down mode, all
 * instructions are ignored except the Resume from Deep
 * Power-down command.
 * @warning UNTESTED
 **/
void AT45_DeepPowerDown(void)
{
	uint8_t Data;
  HAL_GPIO_WritePin(GPIOD, FLASH_CS_Pin, GPIO_PIN_SET);//DF_CS_inactive;    /* Make sure to toggle CS signal in order */
  HAL_GPIO_WritePin(GPIOD, FLASH_CS_Pin, GPIO_PIN_RESET);//DF_CS_active;      /* to reset Dataflash command decoder     */
    
    /* Send opcode */
    //spi_transfer(AT45DB161D_DEEP_POWER_DOWN);
	Data = AT45DB161D_DEEP_POWER_DOWN;
	HAL_SPI_Transmit(&hspi2, &Data, 1, 100);
    
    /* Enter Deep Power-Down mode */
  //  DF_CS_inactive;
	
	HAL_GPIO_WritePin(GPIOD, FLASH_CS_Pin, GPIO_PIN_SET);
    
    /* Safety delay */
	HAL_Delay(100);
    //wait_ms(100);
}
 
/**
 * Takes the device out of Deep Power-down mode.
 **/
void AT45_ResumeFromDeepPowerDown()
{
	uint8_t Data;
    HAL_GPIO_WritePin(GPIOD, FLASH_CS_Pin, GPIO_PIN_SET);//DF_CS_inactive;    /* Make sure to toggle CS signal in order */
    HAL_GPIO_WritePin(GPIOD, FLASH_CS_Pin, GPIO_PIN_RESET);//DF_CS_active;      /* to reset Dataflash command decoder     */
    
    /* Send opcode */
    //spi_transfer(AT45DB161D_RESUME_FROM_DEEP_POWER_DOWN);
	Data = AT45DB161D_RESUME_FROM_DEEP_POWER_DOWN;
		HAL_SPI_Transmit(&hspi2, &Data, 1, 100);
    
    /* Resume device */
   // DF_CS_inactive;
	
	HAL_GPIO_WritePin(GPIOD, FLASH_CS_Pin, GPIO_PIN_SET);
    
    /* The CS pin must stay high during t_RDPD microseconds before the device
     * can receive any commands.
     * On the at45db161D t_RDPD = 35 microseconds. But we will wait 100
     * (just to be sure). */
		HAL_Delay(100);
   // wait_ms(100);
}
 

void Read_AT45_ReadManufacturer(void)
{
	AT45_ReadManufacturerAndDeviceID(id);
}
/** **/
