/*
    simple keypad calculator that uses an AVR ATMega32 microcontroller.
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
#define F_CPU 4000000	//CPU freq for delay

#define CLC_OPT_ADD 0b00010000	//16
#define CLC_OPT_SUB 0b00100000	//32
#define CLC_OPT_MUL 0b01000000	//64
#define CLC_OPT_DIV 0b10000000	//128
#define CLC_FDB_DEF 0xFF  //default
#define CLC_FDB_RES 0b01100110	//result
#define CLC_FDB_RST 0b01111110	//reset
#define CLC_FDB_EXC 0b10101010	//exception

#include <avr/io.h>
#include <util/delay.h>
#include <stdbool.h>

typedef enum {
	PORT_A,
	PORT_B,
	PORT_C,
	PORT_D
} port_name_t;


volatile uint8_t* kpd_ddr;
volatile uint8_t* kpd_pin;
volatile uint8_t* kpd_port;


void set_diodes(port_name_t port, uint8_t number){	//dla wybranego portu
	switch(port){
		case PORT_B:
			DDRB = 0xFF;
			PORTB = number;
			break;
		case PORT_C:
			DDRC = 0xFF;
			PORTC = number;
		break;
		case PORT_D:
			DDRD = 0xFF;
			PORTD = number;
		break;
		default:
			DDRA = 0xFF;
			PORTA = number;
		break;
	}
}


void kpd_init(port_name_t port_name){
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
bool kpd_is_key_pressed(uint8_t row, uint8_t column){	//keypad indexing from 0
	*kpd_port = ~(1<<(column+4));
	_delay_ms(4);
	return ((*kpd_pin & (1<<row)) == 0);
}
char kpd_get_key(){
	uint8_t res = 0, k, w;
	for(k = 0; k < 4; k++){
		for(w = 0; w < 4; w++){
			if(kpd_is_key_pressed(w,k)){
				if(res == 0) res = 4*w + (k+1);
				else return 0xFF;
			}
		}
	}
	return res;
}

void calculus_feedback(port_name_t diod_port, uint8_t value){
	set_diodes(diod_port, value);
	_delay_ms(700);
	set_diodes(diod_port, 0);
}
void calculus_option_set_lower(uint8_t* option, uint8_t value){
	*option &= 0xF0;
	*option |= (value & 0x0F);
}
uint8_t calculus_option_get_lower(uint8_t* option){
	return *option & 0x0F;
}
void calculus_option_set_upper(uint8_t* option, uint8_t value){
	*option &= 0x0F;
	*option |= (value & 0xF0);
}
uint8_t calculus_option_get_upper(uint8_t* option){
	return *option & 0xF0;
}
void calculus_input_num(uint8_t* buffer, uint8_t* option, char key){
	calculus_feedback(PORT_B, *buffer * 10);
	*buffer = key + (*buffer)*10;
}
void calculus_operate(port_name_t diod_port, uint8_t* buffer, uint8_t* option, uint8_t* memory){
	switch(calculus_option_get_upper(option)){
		case 0:	// start
			*memory += *buffer;
			*buffer = 0;
			calculus_feedback(diod_port, CLC_FDB_DEF);
		break;
		case CLC_OPT_ADD:	// +
			*memory += *buffer;
			*buffer = 0;
			*option = 0;
		break;
		case CLC_OPT_SUB:	// -
			*memory -= *buffer;
			*buffer = 0;
			*option = 0;
		break;
		case CLC_OPT_MUL:	// *
			*memory *= *buffer;
			*buffer = 0;
			*option = 0;
		break;
		case CLC_OPT_DIV:	// /
			*memory /= *buffer;
			*buffer = 0;
			*option = 0;
		break;
		default:	//exception
			calculus_feedback(diod_port, CLC_FDB_EXC);
			*buffer = 0;
			*option = 0;
		break;
	}
}
void calculus(port_name_t diod_port, uint8_t* buffer, uint8_t* option, uint8_t* memory){
	char key = kpd_get_key();

	if(key != 0){
		switch(key){
			case 10:	//0 input
				calculus_input_num(buffer, option, 0);
			break;
			case 11:	//reset
				*buffer = 0;
				*option = 0;
				*memory = 0;
				calculus_feedback(diod_port, CLC_FDB_RST);
			break;
			case 12:	//`/ operation
				calculus_operate(diod_port, buffer, option, memory);
				calculus_option_set_upper(option,CLC_OPT_DIV);
			break;
			case 13:	//+ operation
				calculus_operate(diod_port, buffer, option, memory);
				calculus_option_set_upper(option,CLC_OPT_ADD);
			break;
			case 14:	//- operation
				calculus_operate(diod_port, buffer, option, memory);
				calculus_option_set_upper(option,CLC_OPT_SUB);
			break;
			case 15:	//`* operation
				calculus_operate(diod_port, buffer, option, memory);
				calculus_option_set_upper(option,CLC_OPT_MUL);
			break;
			case 16:	//= operation
				calculus_feedback(diod_port, CLC_FDB_RES);
				calculus_operate(diod_port, buffer, option, memory);
			break;
			default:
				calculus_input_num(buffer, option, key);
			break;
		}
	} else {
		set_diodes(diod_port, *memory);
		set_diodes(PORT_B, *option);		//debug
	}
}


int main(void)
{
	port_name_t diod_p = PORT_D, kpd_p = PORT_A;
	uint8_t calc_buf = 0, calc_opt = 0, calc_mem = 0;
	
	kpd_init(kpd_p);

	while(1){
		calculus(diod_p, &calc_buf, &calc_opt, &calc_mem);
	}
}
