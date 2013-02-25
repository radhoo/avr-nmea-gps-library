/*
    File:       uart.cpp
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


#include "uart.h"
#include <stdio.h>
#include <string.h>


/*
 * To receive interrupt results, use:
 ISR(USART0_RX_vect){
	 volatile char c = UDR0; //Read the value out of the UART buffer
 }
 */

/*
 * AVR UART registers configured for Atmega128. For Atmega8 or others with a single UART interface, comment the XXX1 registers and flags (second line)
 */
volatile unsigned char *uart_regs[2][6] = {
	{&UCSR0A, &UCSR0B, &UCSR0C, &UDR0, &UBRR0H, &UBRR0L}, // uart 0
	{&UCSR1A, &UCSR1B, &UCSR1C, &UDR1, &UBRR1H, &UBRR1L}  // uart 1
};
int uart_flags[2][8] = {
	{ RXCIE0, UDRE0, RXC0, RXEN0, TXEN0, UCSZ00, UCSZ01, TXC0}, // uart 0
	{ RXCIE1, UDRE1, RXC1, RXEN1, TXEN1, UCSZ10, UCSZ11, TXC1}  // uart 1
};

/*
 * Initiates UART serial communication
 * uartindex: the UART index, for microcontrollers that have more than one (eg. atmega128 has too)
 * baud: an integer representing the baud rate
 * use_interrupt: receive data as interrupt, in ISR(USART0_RX_vect)/ISR(USART1_RX_vect) or by polling, using RecvPoll
 */
void UART::Init(int uartindex, int baud, bool use_interrupt) {
	m_index = uartindex;
	// save baud
	m_nBaudRate = baud;
	m_index = uartindex;
	
	//Configure the UART registers
	//RXCIE0 Enable interupt after receive
	//RXEN0 Reciever enabled
	//TXEN0 Transmit enabled
	*uart_regs[m_index][1] = (1<<uart_flags[m_index][3]) | (1<<uart_flags[m_index][4]);
	
	//UCSZ00 and UCSZ01 use 8 bit character size
	*uart_regs[m_index][2] = (1<<uart_flags[m_index][5]) | (1<<uart_flags[m_index][6]);

	//Set the baud rate
	m_nBaudRateRegister = F_CPU/16/baud-1;
	*uart_regs[m_index][4] = ((m_nBaudRateRegister>>8)&0x00FF);
	*uart_regs[m_index][5] = (m_nBaudRateRegister&0x00FF);
	
	// Set interrupts if required
	if (use_interrupt) {
		*uart_regs[m_index][1] |= (1<<uart_flags[m_index][0]);
		sei();
	}		
}

/*
 * send one byte, b
 */
void UART::Send(unsigned char b) {
	// Wait until UDR ready
	while(!(*uart_regs[m_index][0] & (1 << uart_flags[m_index][1])));
	*uart_regs[m_index][3] = b;    // send character
}

/*
 * send a byte array (data) of given size (len)
 */
void UART::Send(const unsigned char *data, int len)
{
	if (len == -1) len = strlen((char *)data);
	// send bytes
	for (int i=0; i<len; i++) Send(data[i]);
	// wait for complete transfer
	while (!(*uart_regs[m_index][0] & (1<<uart_flags[m_index][7])));
}

/*
 * send a formated string
 */
void UART::Send(char *szFormat, ...)
{
	char szBuffer[512]; //in this buffer we form the message
	int NUMCHARS = sizeof(szBuffer) / sizeof(szBuffer[0]);
	int LASTCHAR = NUMCHARS - 1;
	va_list pArgs;
	va_start(pArgs, szFormat);
	vsnprintf(szBuffer, NUMCHARS - 1, szFormat, pArgs);
	va_end(pArgs);
	
	Send((const unsigned char *)szBuffer, -1);
}

/*
 * wait until one byte is received, and return: blocking function
 */
unsigned char UART::RecvPoll(){
	// Wait until UDR ready
	while (!(*uart_regs[m_index][0]  & (1 << uart_flags[m_index][2]))) ;
	return *uart_regs[m_index][3];
}	