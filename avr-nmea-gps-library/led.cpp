/*
    File:       led.cpp
    Version:    0.1.0
    Date:       Feb. 21, 2013
	License:	GPL v2
    
	LED Class code, to allow simple use of leds controlled directly by I/O pins
    
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



#include "led.h"

/*void LED::Init(int reg, int port, int dq) {
	m_reg = reg;
	m_port = port;
	m_dq = dq;
	
	_SFR_IO8(m_reg) |=(1<<dq);
}
void LED::Set(int st) {
	led_state = st;
	if (st)
	_SFR_IO8(m_port) |= (1<<m_dq);
	else
	_SFR_IO8(m_port) &= ~(1<<m_dq);
}*/
void LED::Init( volatile uint8_t  *ddr, volatile  uint8_t  *port, uint8_t  dq) {
	// save globals
	m_pddr = ddr;
	m_pport = port;
	m_dq = dq;
	// set for output
	*m_pddr |=(1<<m_dq);
}

void LED::Set(uint8_t st) {
	m_ledstate = st;
	if (st)
		*m_pport |= (1<<m_dq);
	else
		*m_pport &= ~(1<<m_dq);
}


void LED::Toggle() {
	Set(1 - m_ledstate);
}