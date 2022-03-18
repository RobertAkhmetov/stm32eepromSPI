#include "flash.h"


char buff[528];
/*******************************************************************************************
Для вывода картинок в дисплей

char buff[528] и buff[a]  не нужны, использовались для отладки. Вместо них процедура вывода в дисплей

addres	- Адрес начала блока чтения
x				- координата по x
y				- координата по y
w				- ширина
h				- высота
*******************************************************************************************/
//==========================================================================================
void PrintBin(uint32_t addres, uint16_t x, uint16_t y, uint16_t w, uint16_t h)
{
	uint16_t ostatok1=0; uint16_t ostatok2=0; uint16_t kol_page=0;
	uint16_t page=0; uint16_t offset=0; uint32_t razmer=0; uint32_t temp=0;
	uint8_t flag_offset = 0; uint8_t flag_ostat2 = 0;
	uint8_t byte=0; uint16_t a_page=0;
	
	razmer = w * h; 															// количество байт															 
	razmer = razmer * 2;													// по два байта на цвет															 
	
	if(addres==0)																	// если адрес  0
	{																							// пример: страница 0 
		page = 0; offset = 0;												// смещения нет
	}
	else
	if(addres<PAGE_SIZES)													// если адрес внутри страницы 0
	{																							// пример: страница 0 addres 304
		page = 0;
		offset = PAGE_SIZES - addres;								// смещение составит 224 байта
	}
	else
	if(addres>PAGE_SIZES)
	{
		page = (uint16_t)(addres / PAGE_SIZES); 			// вычисляем страницу начала чтения 		
		temp = page * PAGE_SIZES; 										//																				
		offset = (uint16_t)(addres - temp);						// смещение внутри страницы							
	}
	
	if(offset != 0) 												
	{
		flag_offset = 1; 															// есть смещение, устанавливаем флаг
		ostatok1 = PAGE_SIZES - offset;								// количество байт до конца страницы  
	}else flag_offset = 0;
																									// пример: страница 0 
																									// 304 байта до конца страницы, блок = 153600 байта
	if(razmer<PAGE_SIZES)														// пример: razmer 352 байта, 0-я страница
	{		
		if(flag_offset == 1)
		{			//смещение есть
			if(razmer>ostatok1)	
			{																						// есть данные в следующей странице
				ostatok2 = razmer - ostatok1;
			}
			if(razmer<ostatok1)	
			{
				ostatok2 = 0;															// установим в 0, данных следующей страницы нет
				ostatok1 = razmer;
			}
		}
		if(flag_offset == 0)	
		{			//смещения нет		
			ostatok1 = razmer;
			flag_offset = 1;														// в данном случае установим флаг, для чтения
		}
	}
	else
	{
		kol_page = (uint16_t)(razmer  / PAGE_SIZES); 	// вычисляем количество страниц. пример:  153600/528=290 страниц
		temp = kol_page * PAGE_SIZES;									
		ostatok2 = (uint16_t)(razmer - temp); 				// возможный остаток байтов
		ostatok2 = ostatok2 - ostatok1;								// оставшиеся байты в следующей странице 
	}
			
	if(ostatok2!=0) 																// есть остаток, устанавливаем флаг																	
		flag_ostat2 = 1; else flag_ostat2 = 0;
//------------------------------------------------------------------------------------------------------------
	if(flag_offset)																	// есть смещение, считать оставшиеся байты до конца страницы
	{																								
		AT45_ReadMainMemoryPage( page, offset);				// установка страницы на чтение с смещением
		for(uint16_t a=0; a<ostatok1; a++)	
		{		// читаем по 2 байта из SPI до конца страницы или байты в остатке1, если размер блока маленький
			HAL_SPI_Receive(&hspi2, &byte, 1, 100);	buff[a] = byte;
			a++;			
			HAL_SPI_Receive(&hspi2, &byte, 1, 100);	buff[a] = byte;
		} page++;																				//переходим к следующей странице 
	}
	
	//_______________чтение_блока_из_страниц__________________________________________________________
		for(a_page=page; a_page<(page+kol_page); a_page++)
		{
			AT45_ReadMainMemoryPage( a_page, 0);				// установка страницы на чтение без смещения
			for(uint16_t a=0; a<PAGE_SIZES; a++)
			{		// читаем по 2 байта из SPI до конца страницы
				HAL_SPI_Receive(&hspi2, &byte, 1, 100);	buff[a] = byte;
				a++;
				HAL_SPI_Receive(&hspi2, &byte, 1, 100);	buff[a] = byte;
			}
		}
	//_________________________________________________________________________________________________
	if(flag_ostat2)																	// Если есть оставшиеся байты в следующей странице
	{
		AT45_ReadMainMemoryPage( a_page, 0);					// установка страницы на чтение без смещения 
		for(uint16_t a=0; a<ostatok2; a++)						// Считываем оставшиеся байты
		{		// читаем по 2 байта из SPI
			HAL_SPI_Receive(&hspi2, &byte, 1, 100);	buff[a] = byte;
			a++;
			HAL_SPI_Receive(&hspi2, &byte, 1, 100);	buff[a] = byte;
		}
	}	
	HAL_GPIO_WritePin(GPIOA, FLASH_CS_Pin, GPIO_PIN_SET);
	HAL_GPIO_WritePin(GPIOD, Led_Read_Pin, GPIO_PIN_SET);  // светодиод начала чтения
}
//==========================================================================================
/*******************************************************************************************
Datas 		- Блок данных для записи в чип
addres		- Адрес куда записывать
sizeblock - размер блока данных, в байтах
erase			- стирание страницы перед записью или нет,  1 - стирание, 0 - без стирания
********************************************************************************************/
void LoadFlash( uint8_t *Datas, uint32_t addres, uint32_t sizeblock, uint8_t erase)		// Запись подготовленных данных во флеш, erase=1 стирание/ erase=0 без стирания
{
	uint16_t ostatok1=0; uint16_t ostatok2=0; uint16_t kol_page=0;
	uint16_t page=0; uint16_t offset=0; uint32_t temp=0;
	uint8_t flag_offset = 0; uint8_t flag_ostat2 = 0;
	uint32_t bytes = 0; uint8_t Data = 0; uint32_t page_s=0;
	
	
	HAL_GPIO_WritePin(GPIOD, Led_Data_OK_Pin, GPIO_PIN_RESET);
	
	// вычислим страницу и возможное смещение куда записывать данные
	if(addres==0)																// если адрес  0
	{																						// пример: страница 0 
		page = 0; offset = 0;											// смещения нет
	}
	else
	if(addres<PAGE_SIZES)												// если адрес внутри страницы 0
	{																						// пример: страница 0 addres 304
		page = 0;
		offset = PAGE_SIZES - addres;							// смещение составит 224 байта
	}
	else
	if(addres>PAGE_SIZES)
	{
		page = (uint16_t)(addres / PAGE_SIZES); 	// страница начала записи
		temp = page * PAGE_SIZES; 									 
		offset = (uint16_t)(addres - temp);			// смещение в странице начала записи
	}
	//------------------------------------------------------------------------------------------------------------------
	if(offset!=0)  
	{
		flag_offset = 1;													// смещение есть, устанавливаем флаг
		ostatok1 = PAGE_SIZES - offset;						// количество байт до конца страницы. пример: 528-224= 304 до конца страницы 304
	} else flag_offset = 0;	
	
	// вычислим количесво страниц блока и возможный остаток
	if(sizeblock<PAGE_SIZES)												
	{
		if(flag_offset == 1)									// пример: razmer 352 байта, 0-я страница, смещение есть 224, это адрес ostatok1 304
		{			//смещение есть
			if(sizeblock>ostatok1)							// пример: до конца страницы 304 байта, razmer 352 байта
			{																		// вычислим оставшиеся байты для следующей страницы
				ostatok2 = sizeblock - ostatok1;	// 352-304=48 байт будут записаны в следующей странице
			}
			if(sizeblock<ostatok1)
			{
				ostatok2 = 0;											// установим в 0, данных для следующей страницы нет
				ostatok1 = sizeblock;
				
			}			
		}
		if(flag_offset == 0)									// пример: razmer 352 байта, 0-я страница, смещения нет
		{			//смещения нет
			ostatok1 = sizeblock;
			flag_offset = 1;										// в данном случае установим флаг, для записи
		}
	}
	else
	{
		kol_page = (uint16_t)(sizeblock / PAGE_SIZES); 		// количество страниц в блоке
		temp = kol_page * PAGE_SIZES; 								 
		ostatok2 = (uint16_t)(sizeblock - temp);					
		ostatok2 = ostatok2 - ostatok1;										// возможный остаток байтов
	}
	if(ostatok2!=0) 																		// есть остаток, устанавливаем флаг																	
		flag_ostat2 = 1; else flag_ostat2 = 0;
	//------------------------------------------------------------------------------------
	HAL_GPIO_WritePin(GPIOD, Led_Write_Pin, GPIO_PIN_SET);  // светодиод начала записи
	
	if(flag_offset)																			// есть смещение, записываем оставшиеся байты до конца страницы
	{																							
			AT45_PageToBuffer(page, 1);											// копируем страницу в буфер для модификации( дозаписи)		
			AT45_BufferWrite( 1, offset);
			for(uint16_t buf=offset; buf<(offset+ostatok1); buf++)			// дописываем байты
			{
				Data = Datas[bytes];													// копируем байт
				HAL_SPI_Transmit(&hspi2, &Data, 1, 100);			// передаём через SPI
				bytes ++;																			// следующий байт
			}			
			AT45_BufferToPage( 1, page, erase);							// записываем страницу обратно, erase=1 стираем страницу			
			page++;																					// переходим к следующей странице для записи
	}
	//_______________запись_блока_в_страницы__________________________________________________________
	for(page_s=page; page_s<(page+kol_page); page_s++)	// счётчик страниц,
	{
		AT45_BufferWrite( 1, 0);													// Запись данных в буфер1
		for(uint16_t buf=0;buf<PAGE_SIZES;buf++)					// Заполняем буфер
		{
			Data = Datas[bytes];														// копируем байт
			HAL_SPI_Transmit(&hspi2, &Data, 1, 100);				// передаём через SPI
			bytes ++;																				// следующий байт
		}		
		AT45_BufferToPage( 1, page_s, erase);							// Копируем из буфера1 в страницу, erase=1 стираем страницу
	}
	//________________________________________________________________________________________________
	if(flag_ostat2)																			// Если есть оставшиеся байты для следующей странице
	{
		AT45_BufferWrite( 1, 0);													// Запись данных в буфер1
		for(uint16_t buf=0; buf<ostatok2; buf++)					// записываем оставшиеся байты
		{
			Data = Datas[bytes];														// копируем байт
			HAL_SPI_Transmit(&hspi2, &Data, 1, 100);				// передаём через SPI
			bytes ++;																				// следующий байт
		}
		AT45_BufferToPage( 1, page_s, erase);							// Копируем из буфера1 в страницу, erase=1 стираем страницу
	}
	HAL_GPIO_WritePin(GPIOA, FLASH_CS_Pin, GPIO_PIN_SET);
	HAL_GPIO_WritePin(GPIOD, Led_Write_Pin, GPIO_PIN_RESET);  // гасим светодиод, конец записи
	HAL_GPIO_WritePin(GPIOD, Led_Data_OK_Pin, GPIO_PIN_SET);
}

//================================================================================================

