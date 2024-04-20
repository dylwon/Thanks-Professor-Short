//***************************************************************************
//
// File Name : DOGM163WA driver.h
// Title :
// Date : 4/14/2024
// Version : 1.0
// Target MCU : AVR128DB48
// Target Hardware : AVR128DB48
// Author : Dylan Wong & Baron Mai
//
// This program configures SPI communication between the AVR128DB48 and the
// Display on Glass LCD screen (DOG LCD).
// The pins for SPI are listed below:
// PA4 for MOSI
// PA6 for SCK
// PA7 for /SS
// PA5 for MISO
// PC0 for RS of LCD
//
// Warnings :
// Restrictions : none
// Algorithms : none
// References :
//
// Revision History : Initial version
//
//
//**************************************************************************

#ifndef DOGM163WA_DRIVER_H_
#define DOGM163WA_DRIVER_H_
#include "messages.h"
#define F_CPU 4000000LU
#define SIZE 17
#include <stdlib.h>
#include <stdio.h>
#include <util/delay.h>
#include <string.h>

extern char *names[33];

char lcd0_buff[26][17]; 
char lcd1_buff[26][17];

int lcd0_row = 0, lcd1_row = 0;

//***************************************************************************
//
// Function Name : void lcd_spi_transmit_CMD (int LCD, unsigned char cmd)
// Date : 3/29/2024
// Version : 1.0
// Target MCU : AVR128DB48
// Target Hardware : AVR128DB48
// Author : Dylan Wong & Kenneth Short
// 
// This function transmits a command character to the specified DOG LCD. The steps are shown below:
// 1) Select the device by pulling the /SS0 or /SS1 line low
// 2) Pull the RS0 or RS1 line to a 0 for the DOG LCD to interpret the serial byte packet as a command
// 3) Transmit the serial byte by placing cmd into the SPI data register
// 4) Wait until the data transmission is complete by polling for the IF flag
// 5) De-select the device by pulling the /SS0 or /SS1 line high
//
// Warnings : none
// Restrictions : none
// Algorithms : none
// References : none
//
// Revision History : Initial version
//
//**************************************************************************
 
void lcd_spi_transmit_CMD (int LCD, unsigned char cmd) {
	if (LCD == 0) {
		VPORTB_OUT &= ~PIN0_bm; // /SS0 = 0 to select LCD0
		VPORTC_OUT &= ~PIN0_bm; // RS0 = 0 for command
	}
	else {
		VPORTB_OUT &= ~PIN1_bm; // /SS1 = 0 to select LCD1
		VPORTC_OUT &= ~PIN1_bm; // RS1 = 0 for command
	}
	
	SPI0.DATA = cmd;		//send command
	while(!(SPI0.INTFLAGS & SPI_IF_bm)) {}    // Wait until IF flag is set
		
	if (LCD == 0)
		VPORTB_OUT |= PIN0_bm; // /SS0 = 1 to de-select LCD0
	else
		VPORTB_OUT |= PIN1_bm; // /SS1 = 1 to de-select LCD1
}

//***************************************************************************
//
// Function Name : void lcd_spi_transmit_CMD (int LCD, unsigned char cmd)
// Date : 3/29/2024
// Version : 1.0
// Target MCU : AVR128DB48
// Target Hardware : AVR128DB48
// Author : Dylan Wong & Kenneth Short
//
// This function transmits a data byte to the specified DOG LCD. The steps are shown below:
// 1) Select the device by pulling the /SS0 or /SS1 line low
// 2) Pull the RS0 or RS1 line to a 0 for the DOG LCD to interpret the serial byte packet as a data
// 3) Transmit the serial byte by placing cmd into the SPI data register
// 4) Wait until the data transmission is complete by polling for the IF flag
// 5) De-select the device by pulling the /SS0 or /SS1 line high
//
// Warnings : none
// Restrictions : none
// Algorithms : none
// References : none
//
// Revision History : Initial version
//
//**************************************************************************

void lcd_spi_transmit_DATA (int LCD, unsigned char cmd) {
	if (LCD == 0) {
		VPORTB_OUT &= ~PIN0_bm; // /SS0 = 0 to select LCD0
		VPORTC_OUT |= PIN0_bm; // RS0 = 1 for data
	}
	else {
		VPORTB_OUT &= ~PIN1_bm; // /SS1 = 0 to select LCD1
		VPORTC_OUT |= PIN1_bm; // RS1 = 1 for data
	}
	
	SPI0.DATA = cmd;		//send command
	while(!(SPI0.INTFLAGS & SPI_IF_bm)) {}    // Wait until IF flag is set
	
	if (LCD == 0)
		VPORTB_OUT |= PIN0_bm; // /SS0 = 1 to de-select LCD0
	else
		VPORTB_OUT |= PIN1_bm; // /SS1 = 1 to de-select LCD1
}

//***************************************************************************
//
// Function Name : void init_spi_lcd (void)
// Date : 3/29/2024
// Version : 1.0
// Target MCU : AVR128DB48
// Target Hardware : AVR128DB48
// Author : Dylan Wong & Kenneth Short
//
// This function properly sets up the registers in the SPI module to enable serial communication
// between the AVR128DB48 and the DOG LCD. The steps for configuration are listed below:
// 1) Set up pin directions for MOSI, MISO, SCK, /SS0 /SS1, and RS0 RS1 pins
// 2) Sets AVR128DB48 as master, and enables SPI protocol
// 3) Enables SPI mode 3 (CPOL = 1, CPHA = 1) and sets data order to send MSB first
// 4) Pulls the /SS line to high to de-select the other peripherals, and initialize RS0 and RS1 to 0 to send commands
//
// Warnings : Ensure there's proper configuration of registers
// Restrictions : none
// Algorithms : none
// References : none
//
// Revision History : Initial version
//
//**************************************************************************

void init_spi_lcd (void) {
	// Generic clock generator 0, enabled at reset @ 4MHz, is used for peripheral clock
	
	// Pin Direction Configurations & Initializations for both LCDs
	VPORTA_DIR |= PIN4_bm | PIN6_bm; // PA4 is output for MOSI, PA5 is input for MISO, PA6 is output for SCK
	VPORTB_DIR |= PIN0_bm | PIN1_bm; // PB0 is output for /SS0 (SS for LCD0), PB1 is output for /SS1 (SS for LCD1)
	VPORTB_OUT |= PIN0_bm | PIN1_bm; // Idles /SS0 and /SS1 as high to de-select LCDs
	VPORTC_DIR |= PIN0_bm | PIN1_bm; // PC0 is output for RS0 of LCD0, PC1 is output for RS1 of LCD1
	
	// SPI Configuration
	SPI0.CTRLA |= SPI_MASTER_bm | SPI_ENABLE_bm; // Sets AVR128DB48 as master, and enables SPI protocol
	SPI0.CTRLB |= SPI_MODE_3_gc; // Enables SPI mode 3 (CPOL = 1, CPHA = 1) and Data order sends MSB first

	VPORTC_OUT &= ~(PIN0_bm | PIN1_bm);	// RS0 = 0 and RS1 = 0 for command sends
}

//***************************************************************************
//
// Function Name : void init_lcd_dog(void)
// Date : 3/29/2024
// Version : 1.0
// Target MCU : AVR128DB48
// Target Hardware : AVR128DB48
// Author : Dylan Wong & Kenneth Short
//
// This function sends serial bytes to both of the DOG LCD to configure its settings.
// Configuring the functionality of the display requires sending well timed
// serial packets in the correct order and with the right delays.
//
// Warnings : Ensure serial packets for initialization are sent in the right order, 
//			  and proper delays are used in between the sent packets.
// Restrictions : none
// Algorithms : lcd_spi_transmit_CMD
// References : none
//
// Revision History : Initial version
//
//**************************************************************************

void init_lcd_dog (void) {
	init_spi_lcd();		//Initialize MCU for SPI with both LCD displays
	
	for (uint8_t i = 0; i < 1; i++) {
		//start_dly_40ms:
		_delay_ms(40);	//40ms delay for command to be processed

		//func_set1:
		lcd_spi_transmit_CMD(i, 0x39);   // send function set #1
		_delay_us(30);	//26.3us delay for command to be processed


		//func_set2:
		lcd_spi_transmit_CMD(i, 0x39);	//send function set #2
		_delay_us(30);	//26.3us delay for command to be processed


		//bias_set:
		lcd_spi_transmit_CMD(i, 0x1E);	//set bias value.
		_delay_us(30);	//26.3us delay for command to be processed


		//power_ctrl:
		lcd_spi_transmit_CMD(i, 0x55);	//~ 0x50 nominal for 5V
		//~ 0x55 for 3.3V (delicate adjustment).
		_delay_us(30);	//26.3us delay for command to be processed


		//follower_ctrl:
		lcd_spi_transmit_CMD(i, 0x6C);	//follower mode on...
		_delay_ms(200);	//200ms delay for command to be processed


		//contrast_set:
		lcd_spi_transmit_CMD(i, 0x7F);	//~ 77 for 5V, ~ 7F for 3.3V
		_delay_us(30);	//26.3us delay for command to be processed


		//display_on:
		lcd_spi_transmit_CMD(i, 0x0c);	//display on, cursor off, blink off
		_delay_us(30);	//26.3us delay for command to be processed


		//clr_display:
		lcd_spi_transmit_CMD(i, 0x01);	//clear display, cursor home
		_delay_us(30);	//26.3us delay for command to be processed


		//entry_mode:
		lcd_spi_transmit_CMD(i, 0x06);	//clear display, cursor home
		_delay_us(30);	//26.3us delay for command to be processed
	}
	
}

//***************************************************************************
//
// Function Name : void update_lcd_dog(void)
// Date : 3/29/2024
// Version : 1.0
// Target MCU : AVR128DB48
// Target Hardware : AVR128DB48
// Author : Kenneth Short
//
// This function updates the text shown on the DOG LCD. This function uses both of the
// SPI transmit functions written above to send the proper information. First, an address
// is sent to let the DOG LCD know which line it's writing in. Then, a for-loop is used to transmit
// all of the bytes in the display buffers. This step is repeated for all of the 3 lines and buffers.
//
// Warnings : Ensure that the 3 line buffers are populated with 17 total characters
// Restrictions : none
// Algorithms : init_spi_lcd, lcd_spi_transmit_CMD, lcd_spi_transmit_DATA
// References : none
//
// Revision History : Initial version
//
//**************************************************************************

// Updates the LCD display lines 1, 2, and 3, using the
// contents of dsp_buff_1, dsp_buff_2, and dsp_buff_3, respectively.
void update_lcd_dog(void) {
/*
	init_spi_lcd();		//init SPI port for both LCDs.

	// send line 1 to the LCD module.
	lcd_spi_transmit_CMD(0, 0x80);	//init DDRAM addr-ctr
	_delay_us(30);
	for (int i = 0; i < 16; i++) {
		lcd_spi_transmit_DATA(0, dsp_buff1[i]);
		_delay_us(30);
	}

	_delay_us(30);
	for (int i = 0; i < 16; i++) {
		lcd_spi_transmit_DATA(dsp_buff2[i]);
		_delay_us(30);
	}
	
	_delay_us(30);
	for (int i = 0; i < 16; i++) {
		lcd_spi_transmit_DATA(dsp_buff3[i]);
		_delay_us(30);
	}
*/
	
}

//***************************************************************************
//
// Function Name : void insert_split_msg(char* message)
// Date : 4/19/2024
// Version : 1.0
// Target MCU : AVR128DB48
// Target Hardware : AVR128DB48
// Author : Kenneth Short
//
//
//
// Warnings : 
// Restrictions : none
// Algorithms : 
// References : none
//
// Revision History : Initial version
//
//************************************************************************** 
 
int sizeof_array(char* array) {
	int size = 0;
	while (array[size] != '\0') { size++; }
	return size;
}

int sizeof_matrix(char** matrix) {
	int size = 0;
	while (matrix[size] != NULL) { size++; }
	return size;
}

void insert_split_msg(char* message) {
	int LCD_select = 0;
	int size = sizeof_array(message);
	for (uint8_t i = 0, col = 0; i < size; i++) {
		
		if (!col && message[i] == ' ') // Skips any blank spaces at the beginning of each LCD display
			continue;
		else if (!LCD_select) // Puts character into left LCD
			lcd0_buff[lcd0_row][col++] = message[i];
		else if (LCD_select && col != 0 && !(col % 15) && message[i] != ' ' && message[i] != '\0' && message[i + 1] != ' ' && message[i + 1] != '\0') { // Moves any word that would get cut off on the right LCD to the left LCD
			lcd1_buff[lcd1_row][16] = '\0';
			while (lcd1_buff[lcd1_row][col - 1] != ' ' && lcd1_buff[lcd1_row][col - 1] != '\0') { lcd1_buff[lcd1_row][--col] = ' '; i--; }
			i--;
			LCD_select = !LCD_select;
			col = 0;	
			lcd1_row++;
			continue;
		}
		else // Puts character into right LCD
			lcd1_buff[lcd1_row][col++] = message[i];
		
		if (col != 0 && !(col % 16)) { // Triggers every 16 iterations
			if (!LCD_select)
				lcd0_buff[lcd0_row++][col] = '\0';
			else
				lcd1_buff[lcd1_row++][col] = '\0';
			LCD_select = !LCD_select;
			col = 0;
		}
	}
}

void insert_split_names(char** names) {
	int size = sizeof_matrix(names);
	int name_size = 0;
	for (int i = 0; i < size; i++) {
		int tmp_size = sizeof_array(names[i]);
		if (tmp_size> name_size) {
			name_size = tmp_size;
		}
	}
	int space;
	for (uint8_t i = 0; i < size; i++) {
		for (space = 0; space < name_size; space++) // Grabs the index of where the space
			if (names[i][space] == ' ') 
				break;
		
		for (uint8_t j = 0, k = 0; j < SIZE; j++) {
			if (j >= SIZE - space - 1) {
				lcd0_buff[lcd0_row][j] = names[i][k++];
				continue;
			}
			lcd0_buff[lcd0_row][j] = ' ';
		}
		lcd0_buff[lcd0_row++][SIZE - 1] = '\0';
		
		for (uint8_t j = 0; j < name_size - space; j++) {			
			lcd1_buff[lcd1_row][j] = names[i][space + j];
		}
		lcd1_buff[lcd1_row++][SIZE - 1] = '\0';
	}
}

void down_scroll_display() {
	
}

#endif