/*
    File:       pir.cpp
    Version:    0.1.0
    Date:       Feb. 21, 2013
	License:	GPL v2
    
	PIR sensor code
    
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



#include "pir.h"

void PIR::Init( volatile uint8_t  *ddr, volatile  uint8_t  *pin, uint8_t  dq) {
	// save globals
	m_pddr = ddr;
	m_ppin = pin;
	m_dq = dq;
	// set for input
	*m_pddr &=~(1<<m_dq);
	// set high
	//PIR_PORT|=(1<<PIR_DQ)
}

//returns 0 if no movement detected, else returns positive
uint8_t PIR::Get() {
	return (0!= (*m_ppin & (1<<m_dq)));
}
