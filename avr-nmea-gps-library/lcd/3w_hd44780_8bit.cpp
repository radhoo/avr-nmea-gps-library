/*
    File:       3w_HD44780_8bit.cpp
    Version:    0.1.0
    Date:       Feb. 21, 2013
	License:	GPL v2
    
	C header file for HD44780 LCD module using a 74HCT164 serial in, parallel
	out shift register to operate the LCD in 8-bit mode.  Example schematic
	using atmega128 and 4x20 LCD available here: 
	http://www.pocketmagic.net/?p=3785
    
	* Based on http://www.scienceprog.com/interfacing-lcd-to-atmega-using-two-wires
	* and http://www.micahcarrick.com/avr-3-wire-hd44780-lcd-interface-avr-gcc.html
	 
    ****************************************************************************
    Copyright (C) 2013 Radu Motisan  <radu.motisan@gmail.com>

	http://www.pocketmagic.net

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
    ****************************************************************************
 */


#include "3w_hd44780_8bit.h"
#include <stdio.h>

void HD44780_3W_8BIT::Init(volatile uint8_t  *portRsData, uint8_t  dq_rs_data,
							volatile uint8_t  *portCLK, uint8_t dq_clk, volatile uint8_t  *portE, uint8_t dq_e) {
	// save globals
	m_pportRsData = portRsData;
	m_dqrsdata = dq_rs_data;
	m_pportCLK = portCLK;
	m_dqclk = dq_clk;
	m_pportE = portE;
	m_dqe = dq_e;
	
	// init data and clk ports
	*m_pportRsData &= ~(1<<m_dqrsdata); //set pins to LOW
	*m_pportCLK &= ~(1<<m_dqclk); //set pins to LOW
	*m_pportE &= ~(1<<m_dqe); //set pins to LOW
	
	*Port2DDR(m_pportRsData) |= (1<<m_dqrsdata);//Enable pins as outputs
	*Port2DDR(m_pportCLK) |= (1<<m_dqclk);//Enable pins as outputs
	*Port2DDR(m_pportE) |= (1<<m_dqe);//Enable pins as outputs
	
	// init LCD
	lcd_init();
}

/*
 * Loads a byte into the shift register (74'164).  Does NOT load into the LCD.
 *
 * Parameters:
 *      b        The byte to load into the '164.
*/
void HD44780_3W_8BIT::lcd_load_byte(uint8_t b)
{
	// make sure clock is low 
	*m_pportCLK &= ~(1<<m_dqclk);
	
	for(int i=0; i<8; i++) {
		// loop through bits 	
		if (b & 0x80) 
			*m_pportRsData |=(1<<m_dqrsdata); // this bit is high 
		else 
			*m_pportRsData &= ~(1<<m_dqrsdata); //this bit is low 
		// next bit
		b = b << 1;
		// pulse the the shift register clock *
		*m_pportCLK |= (1<<m_dqclk);
		*m_pportCLK &= ~(1<<m_dqclk);
	}
}

/*
 * Strobes the E signal on the LCD to "accept" the byte in the '164.  The RS
 * line determines wheter the byte is a character or a command.
*/
void HD44780_3W_8BIT::lcd_toggle_e(void)
{
    /* strobe E signal */
    *m_pportE |= (1<<m_dqe);
    _delay_us(450); 
    *m_pportE &= ~(1<<m_dqe);
}


/*
 * Loads the byte in the '164 shift register into the LCD as a command. The
 * '164 should already be loaded with the data using lcd_load_byte().
*/
void HD44780_3W_8BIT::lcd_send_cmd(void)
{
    /* Data in '164 is a command, so RS must be low (0) */
    *m_pportRsData &= ~(1<<m_dqrsdata);
    lcd_toggle_e();
    _delay_us(40);
}

/*
 * Loads the byte in the '164 shift register into the LCD as a character. The
 * '164 should already be loaded with the data using lcd_load_byte().
*/
void HD44780_3W_8BIT::lcd_send_char(void)
{
    /* Data in '164 is a character, so RS must be high (1) */
    *m_pportRsData |= (1<<m_dqrsdata);
    lcd_toggle_e();
    _delay_us(40);
}

/*
 * Content display
 * Loads the byte into the shift register and then sends it to the LCD as a char
 * Parameters:
 *      c               The byte (character) to display
*/
void HD44780_3W_8BIT::lcd_char(const char c)
{
	if (c=='\n') {
		if (m_nCurrentLine >= LCD_LINES - 1)
			lcd_cursor_setline(0);
		else
			lcd_cursor_setline(m_nCurrentLine+1);
	} else {
		lcd_load_byte(c);
		lcd_send_char();
	}		
}

/*
 * Content display
 * Calls lcd_char to sent a complete string to the LCD
 */
void HD44780_3W_8BIT::lcd_string(char *text)
{
	char c;
	while ( (c = *text++) )  lcd_char(c);
}

/*
 * Content display
 * Shows a formatted string on the LCD
 */
void HD44780_3W_8BIT::lcd_string_format(char *szFormat, ...)
{
	char szBuffer[256]; //in this buffer we form the message
	int NUMCHARS = sizeof(szBuffer) / sizeof(szBuffer[0]);
	int LASTCHAR = NUMCHARS - 1;
	va_list pArgs;
	va_start(pArgs, szFormat);
	vsnprintf(szBuffer, NUMCHARS - 1, szFormat, pArgs);
	va_end(pArgs);
	
	lcd_string(szBuffer);
}

/*
 * Moves the cursor to the home position.
*/
void HD44780_3W_8BIT::lcd_cursor_home()
{
	m_nCurrentLine = 0;
	lcd_load_byte(LCD_CMD_HOME);
    lcd_send_cmd();
}

/*
 * Moves the cursor to the specified position.
 * Parameters:
 *      line            Line (row)
 *      pos             Position on that line (column)
*/
void HD44780_3W_8BIT::lcd_cursor_gotoxy(uint8_t line, uint8_t pos)
{
	m_nCurrentLine = line;
    lcd_load_byte(line+pos);
    lcd_send_cmd();
}

// PURPOSE:  Move cursor on specified line
void HD44780_3W_8BIT::lcd_cursor_setline(uint8_t line)
{
	uint8_t addressCounter = 0;
	switch(line)
	{
		case 0: addressCounter = LCD_START_LINE1; break;
		case 1: addressCounter = LCD_START_LINE2; break;
		case 2: addressCounter = LCD_START_LINE3; break;
		case 3: addressCounter = LCD_START_LINE4; break;
		default:addressCounter = LCD_START_LINE1; break;
	}
	m_nCurrentLine = line;
	

	lcd_load_byte((1<<LCD_DDRAM)+addressCounter);
	lcd_send_cmd();
}


void HD44780_3W_8BIT::lcd_clrscr() {
	m_nCurrentLine = 0;
	lcd_load_byte(LCD_CMD_CLEAR);
	lcd_send_cmd();
}

/*
 * Initializes the LCD.  Should be called during the initialization of the 
 * program.
*/
void HD44780_3W_8BIT::lcd_init()
{
	m_nCurrentLine = 0;
    /* 20ms delay while LCD powers on */
    _delay_ms(16);	   
        
    /* Write 0x30 to LCD and wait 5 mS for the instruction to complete */
    lcd_load_byte( 0x30);
    lcd_send_cmd();
    _delay_ms(5);
        
    /* Write 0x30 to LCD and wait 160 uS for instruction to complete */
    lcd_load_byte( 0x30);
    lcd_send_cmd();
    _delay_us(120);
        
    /* Write 0x30 AGAIN to LCD and wait 160 uS */
    lcd_load_byte( 0x30);
    lcd_send_cmd();
    _delay_us(120);
        
    /* Set function and wait 40uS */
    lcd_load_byte(LCD_FUNCTION_8BIT_2LINE); //function
    lcd_send_cmd();
        
    /* Turn off the display and wait 40uS */
    lcd_load_byte(LCD_CMD_OFF);    
    lcd_send_cmd();
        
    /* Clear display and wait 1.64mS */
    lcd_load_byte(LCD_CMD_CLEAR);    
    lcd_send_cmd();
    _delay_ms(2);
        
    /* Set entry mode and wait 40uS */
    lcd_load_byte(LCD_CMD_ENTRY_INC);    //entry mode
    lcd_send_cmd();
        
    /* Turn display back on and wait 40uS */
    lcd_load_byte(LCD_CMD_ON);    
    lcd_send_cmd();
};
