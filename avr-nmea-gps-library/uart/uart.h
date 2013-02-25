/*
    File:       uart.h
    Version:    0.1.0
    Date:       Feb. 23, 2013
	License:	GPL v2
    
	UART interrupt driven serial communication class file for atmega128
    
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
#include <avr/interrupt.h>
#include <avr/signal.h>
#include "../timeout.h"


//volatile static voidFuncPtru08 UartRxFunc;
//typedef void (*tUARTReceiverFunc)(unsigned char);

class UART {
	
	private:
		int m_index, //0 for UART0, 1 for UART1 and so on
			m_nBaudRate,
			m_nBaudRateRegister;
	//static tUARTReceiverFunc m_UARTReceiverFunc;
	public:
	
	/*
	 * Initiates UART serial communication
	 * uartindex: the UART index, for microcontrollers that have more than one (eg. atmega128 has too)
	 * baud: an integer representing the baud rate
	 * use_interrupt: receive data as interrupt, in ISR(USART0_RX_vect)/ISR(USART1_RX_vect) or by polling, using RecvPoll
	 */
	void Init(int uartindex, int baud, bool use_interrupt );
	
	/*
	 * send one byte, b
	 */
	void Send(unsigned char b);
	
	/*
	 * send a byte array (data) of given size (len)
	 */
	void Send(const unsigned char *data, int len);
	
	/*
	 * send a formated string
	 */
	void Send(char *szFormat, ...);
	
	/*
	 * wait until one byte is received, and return: blocking function
	 */
	unsigned char RecvPoll();
	
	};
