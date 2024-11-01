/*
 * CW3
 *
 * Created: 17.10.2024 08:27:59
 * Author : volodymyr-tsukanov
 */
#define F_CPU 4000000	//częstotliwość procesora dla delay

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

typedef enum {
	PORT_A,
	PORT_B,
	PORT_C,
	PORT_D
} port_name_t;


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

int is_key_pressed_o(uint8_t row, uint8_t column){	//dla portA
	PORTA = ~(1<<(column+3));
	_delay_ms(4);
	uint8_t x = ~PINA & 0x0F;
	return x == row;
}
int is_key_pressed(uint8_t row, uint8_t column){	//dla portA
	PORTA = ~(1<<(column+4));
	_delay_ms(3);
	return ((PINA & (1<<row)) == 0);
}
char get_key(){	//dla portA
	DDRA = 0xF0;	//wierszy(PA0-3) na wejście, kolumny(PA4-7) na wyjście
	PORTA = 0x0F;
	uint8_t res = 0, k, w;

	for(k = 0; k < 4; k++){
		for(w = 0; w < 4; w++){
			if(is_key_pressed(w,k)){
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
	char key = get_key();

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
	port_name_t diod_port = PORT_D;

	DDRA = 0xF0;	//wierszy(PA0-3) na wejście, kolumny(PA4-7) na wyjście
	PORTA = 0x0F;
	uint8_t calc_buf = 0, calc_opt = 0, calc_mem = 0;

	while(1){
		calculus(diod_port, &calc_buf, &calc_opt, &calc_mem);
	}
}
