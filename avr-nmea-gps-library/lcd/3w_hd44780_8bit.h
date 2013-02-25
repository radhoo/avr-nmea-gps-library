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

#include <avr/io.h>
#include "../timeout.h"
#include <util/delay.h>



class HD44780_3W_8BIT {
	static const int LCD_LINES				 = 4;	 // number of visible lines of the display
	static const int LCD_DISP_LENGTH		 = 20;   // visibles characters per line of the display
	static const int LCD_START_LINE1  		 = 0x00; // DDRAM address of first char of line 1
	static const int LCD_START_LINE2  		 = 0x40; // DDRAM address of first char of line 2
	static const int LCD_START_LINE3  		 = 0x14; // DDRAM address of first char of line 3
	static const int LCD_START_LINE4  		 = 0x54; // DDRAM address of first char of line 4
	static const int LCD_CGRAM				 = 0x6;  // DB6: set CG RAM address
	static const int LCD_DDRAM             	 = 0x7;   // DB7: set DD RAM address
	
	// instruction register bit positions, see HD44780U data sheet
	static const int LCD_CMD_HOME            = 0x02; // cursor home 
	static const int LCD_CMD_ENTRY_DEC       = 0x04; // decrement, no shift 
	static const int LCD_CMD_ENTRY_DEC_SHIFT = 0x05; // decrement with shift 
	static const int LCD_CMD_ENTRY_INC       = 0x06; // increment, no shift 
	static const int LCD_CMD_ENTRY_INC_SHIFT = 0x07; // increment with shift 
	static const int LCD_CMD_CLEAR           = 0x01; // clear, cursor home 
	static const int LCD_CMD_OFF             = 0x08; // display off 
	static const int LCD_CMD_ON              = 0x0C; // display on, cursor off 
	static const int LCD_CMD_ON_BLINK        = 0x0D; // display on, cursor off, blink char 
	static const int LCD_CMD_ON_CURSOR       = 0x0E; // display on, cursor on 
	static const int LCD_CMD_ON_CURSOR_BLINK = 0x0F; // display on, cursor on, blink char 
	static const int LCD_CMD_CURSOR_LEFT     = 0x10; // decrement cursor (left) 
	static const int LCD_CMD_CURSOR_RIGHT    = 0x14; // increment cursor (right) 
	static const int LCD_CMD_SHIFT_LEFT      = 0x18; // shift (left) 
	static const int LCD_CMD_SHIFT_RIGHT     = 0x1C; // shift (right)
	// function set: set interface data length and number of display lines
	static const int LCD_FUNCTION_4BIT_1LINE = 0x20; // 4-bit interface, single line, 5x7 dots
	static const int LCD_FUNCTION_4BIT_2LINE = 0x28; // 4-bit interface, dual line,   5x7 dots
	static const int LCD_FUNCTION_8BIT_1LINE = 0x30; // 8-bit interface, single line, 5x7 dots
	static const int LCD_FUNCTION_8BIT_2LINE = 0x38; // 8-bit interface, dual line,   5x7 dots
	
	private:
	volatile uint8_t	*m_pportRsData, m_dqrsdata, 
						*m_pportCLK, m_dqclk,
						*m_pportE, m_dqe;
	uint8_t m_nCurrentLine;
	
	void lcd_init();
	void lcd_load_byte(uint8_t b);
	void lcd_toggle_e(void);
	void lcd_send_cmd(void);
	void lcd_send_char(void);
	
	//
	volatile uint8_t* Port2DDR(volatile uint8_t *port) {
		return port - 1;
	}
	
	public:
	void Init(volatile uint8_t  *portRsData, uint8_t  dq_rs_data, 
			volatile uint8_t  *portCLK, uint8_t dq_clk, 
			volatile uint8_t  *portE, uint8_t dq_e);
	
	// content display
	void lcd_char(const char c);
	void lcd_string(char *text);
	void lcd_string_format(char *format,...);
	
	// cursor
	void lcd_cursor_home();
	void lcd_cursor_gotoxy(uint8_t line, uint8_t pos);
	void lcd_cursor_setline(uint8_t line);
	
	// clean
	void lcd_clrscr();
};



