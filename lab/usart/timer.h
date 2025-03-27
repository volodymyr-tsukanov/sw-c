/*
    TIMER0 interface for an AVR ATMega32 microcontroller .
    Copyright (C)  2024  volodymyr-tsukanov

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <https://www.gnu.org/licenses/>.
*/
#ifndef TIMER_H
#define TIMER_H
#include "usart.h"

uint8_t timer_sharedVar;

static inline void timer_init(){
	TCCR0 |= (1<<CS02);
	TIMSK |= (1<<TOIE0);
	
	timer_sharedVar = 0;
}

ISR(TIMER0_OVF_vect){
	PORTA = usart_receive_char();
}

#endif // TIMER_H
