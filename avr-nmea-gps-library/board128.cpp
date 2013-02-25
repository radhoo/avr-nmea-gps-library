/*
    File:       board128.cpp
    Version:    0.1.0
    Date:       Feb. 21, 2013
	License:	GPL v2
    
	atmega128 board code
    
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
#include "timeout.h"
#include <util/delay.h>
#include "led.h"
#include "pir.h"
#include "lcd/3w_hd44780_8bit.h"
#include "uart/uart.h"
#include "nmea/nmea.h"

// atmega128 has two leds connected to PA0 and PA1
LED led1, led2;

PIR pir1;

//HD44780 lcd;
HD44780_3W_8BIT lcd;
NMEA nmea;
UART	uart0, // gps module
		uart1; // bluetooth module

//USART0 RX interrupt this code is executed when we recieve a character
ISR(USART0_RX_vect){
	volatile char c = UDR0; //Read the value out of the UART buffer
	nmea.fusedata(c);
}

//USART1 RX interrupt this code is executed when we recieve a character
ISR(USART1_RX_vect){
	volatile char c = UDR1; //Read the value out of the UART buffer
	if (c == '1') _delay_ms(10000); // pause execution when we send char '1' for debugging
}


int main(void)
{
	// init leds
	led1.Init(&DDRA, &PORTA, 0); //DDRA, PORTA, PA0
	led2.Init(&DDRA, &PORTA, 1); //DDRA, PORTA, PA1
	led1.Set(0);led2.Set(1);
	
	// init serial link
	uart0.Init(0,4800, true); //uart0: GPS
	uart1.Init(1,9600, true); //uart1: Bluetooth
	
	// init pir sensor
	//pir1.Init(&DDRC, &PINC, 0);pir1.Get();

	lcd.Init(&PORTC,PC0,	// data/rs
			 &PORTC, PC1,	// clk
			 &PORTC, PC2);  // lcd e

	// show logo, wait a little then go with the main loop
	lcd.lcd_cursor_home();
	lcd.lcd_string("Mega128+NMEA GPS\npocketmagic.net   ");
	_delay_ms(4000);
	
	int i = 0;
    while(1)
    {
		i++;
		
		lcd.lcd_cursor_home();
		if (nmea.isdataready()) {
			// use index to change display on every iteration: easy way to show more content on a small screen
			if (i%2 == 0) { // show latitude, number of sats, longitude, altitude
					lcd.lcd_string_format("%f SATs:%d    \n%f A:%dm    ", 
					nmea.getLatitude(), nmea.getSatellites(),
					 nmea.getLongitude(), (int)nmea.getAltitude());
			} else { //show speed, bearing, time and date
					lcd.lcd_string_format("S:%2.2f B:%2.2f    \n%02d:%02d %02d-%02d-%02d    ",
					nmea.getSpeed(), nmea.getBearing(),
					nmea.getHour(),nmea.getMinute(),
					nmea.getDay(),nmea.getMonth(),nmea.getYear());
			}				 
		} else {
			lcd.lcd_string("Waiting for GPS  \nfix. No Data.       ");
		}
		
		led1.Toggle();
		led2.Toggle();
        _delay_ms(2000);
    }
}