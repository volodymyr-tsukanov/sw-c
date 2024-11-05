/*
    keypad adapter for an AVR ATMega32 microcontroller.
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
#ifndef KEYPAD_H
#define KEYPAD_H

#define KPD_ROWS 4
#define KPD_COLS 4

#include "alc.h"


volatile uint8_t* kpd_ddr;
volatile uint8_t* kpd_pin;
volatile uint8_t* kpd_port;


static inline bool kpd_is_key_pressed(uint8_t row, uint8_t column){	//keypad indexing from 0
	*kpd_port = ~(1<<(column+4));
	_delay_ms(4);
	return ((*kpd_pin & (1<<row)) == 0);
}

static inline char kpd_get_key(){
	uint8_t res = 0, c, r;
	for(c = 0; c < KPD_COLS; c++){
		for(r = 0; r < KPD_ROWS; r++){
			if(kpd_is_key_pressed(r,c)){
				if(res == 0) res = KPD_ROWS*r + (c+1);
				else return 0xFF;
			}
		}
	}
	return res;
}
static inline uint8_t kpd_get_keys(char* Okeys){	//returns count of pressed keys, alters $1 with key indexes
	uint8_t count = 0, c, r;
	alc_array_delete(Okeys);
	Okeys = (char*) alc_array_new(KPD_ROWS*KPD_COLS,sizeof(char)) ;
	for(c = 0; c < 4; c++){
		for(r = 0; r < 4; r++){
			if(kpd_is_key_pressed(r,c)){
				Okeys[count++] = KPD_ROWS*r + (c+1);
			}
		}
	}
	return count;
}

static inline void kpd_init(port_name_t port_name){
	switch (port_name){
		case PORT_A:
			kpd_ddr = &DDRA;
			kpd_pin = &PINA;
			kpd_port = &PORTA;
			break;
		case PORT_B:
			kpd_ddr = &DDRB;
			kpd_pin = &PINB;
			kpd_port = &PORTB;
			break;
		case PORT_C:
			kpd_ddr = &DDRC;
			kpd_pin = &PINC;
			kpd_port = &PORTC;
			break;
		case PORT_D:
			kpd_ddr = &DDRD;
			kpd_pin = &PIND;
			kpd_port = &PORTD;
			break;
	}
	
	*kpd_ddr = 0xF0;	//rows=in, cols=out
	*kpd_port = 0x0F;
}

#endif  //KEYPAD_H
