/*
 * CW3
 *
 * Created: 17.10.2024 08:27:59
 * Author : Volodymyr Tsukanov
 */
#define F_CPU 4000000	//częstotliwość procesora dla delay

#include <avr/io.h>
#include <util/delay.h>

typedef enum {
	PORT_A,
	PORT_B,
	PORT_C,
	PORT_D,
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
void calculus_feedback(port_name_t diod_port, uint8_t val){
	set_diodes(diod_port, val);
	_delay_ms(1700);
	set_diodes(diod_port, 0);
}
void calculus(port_name_t diod_port, uint8_t* status, int* memory){
	char key = get_key();

	if(key != 0){
		switch(*status){
			case 0:	//start
				*memory = key;
				*status = 1;
				calculus_feedback(diod_port, 0xFF);
			break;
			case 1:	//operation expected
				*status = key;
				calculus_feedback(diod_port, 0xFF);
				break;
			case 13:	//+ operation
				*memory += key;
				*status = 1;
				calculus_feedback(diod_port,1);
			break;
			case 14:	//- operation
				*memory -= key;
				*status = 1;
				calculus_feedback(diod_port,2);
			break;
			case 15:	//`* operation
				*memory *= key;
				*status = 1;
				calculus_feedback(diod_port,4);
			break;
			case 12:	//`/ operation
				*memory /= key;
				*status = 1;
				calculus_feedback(diod_port,6);
			break;
			default:	//= operation
				*status = 0;
				set_diodes(diod_port, *memory);
			break;
		}
	} else {
		set_diodes(diod_port, *memory);
	}
}


int main(void)
{
	port_name_t diod_port = PORT_D;

	DDRA = 0xF0;	//wierszy(PA0-3) na wejście, kolumny(PA4-7) na wyjście
	PORTA = 0x0F;
	uint8_t calc_sts = 0;
	int calc_mem = 0;

	while(1){
		calculus(diod_port, &calc_sts, &calc_mem);
	}
}
