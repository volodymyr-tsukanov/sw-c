/*
    simple USART test program for an AVR ATMega32 microcontroller .
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
#define F_CPU 4000000UL //CPU freq

#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include "usart.h"
#include "timer.h"


int main(void)
{
	sei();	//global interrupts: on
	
	timer_init();
	usart_init();
	
	DDRA = 0xFF;

	
    while (1){
		usart_transmit_string("!Hello world!\n\r",16);
		_delay_ms(800);
    }
}
