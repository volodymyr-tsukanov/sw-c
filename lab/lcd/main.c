/*
    simple Hd44780 LCD program for an AVR ATMega32 microcontroller .
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
#define F_CPU 4000000	//proc HZ for delay

#define LCD_PIN_EN 0
#define LCD_PIN_RS 1
#define LCD_CMD_CLEAR 0x01
#define LCD_CMD_RETURN_HOME 0x02
#define LCD_CMD_ENTRY_MODE 0x04
#define LCD_CMD_DISPLAYCONTROL 0x08
#define LCD_DISPLAY_ON 0x04
#define LCD_DISPLAY_OFF 0x00
#define LCD_DISPLAY_CURSOR_ON 0x02
#define LCD_DISPLAY_CURSOR_OFF 0x00
#define LCD_DISPLAY_BLINK_ON 0x01
#define LCD_DISPLAY_BLINK_OFF 0x00
#define LCD_CMD_FUNCTIONSET 0x20
#define LCD_4BMODE 0x00
#define LCD_8BMODE 0x10
#define LCD_1LINE 0x00
#define LCD_2LINE 0x08
#define LCD_5x8DOTS 0x00
#define LCD_5x10DOTS 0x04
#define LCD_CMD_CONFIGURE_ENTRY_MODE 0x04
#define LCD_CMD_SET_DECREMENT_ON_ENTRY 0x00	//was 0<<1
#define LCD_CMD_SET_INCREMENT_ON_ENTRY 0x02
#define LCD_CMD_SET_SHIFT_COURSOR_ON_ENTRY 0x00
#define LCD_CMD_SET_SHIFT_WINDOW_ON_ENTRY 0x01
#define LCD_SET_DDRAM_ADDR 0x80
#define LCD_DDRAM_ROW_OFFSET 0x40
#define LCD_SET_CGRAM_ADDR 0x40	//0b01000000
#define LCD_CGRAM_BASE 0x00
#define LCD_CGRAM_C1 0
#define LCD_ROWS 2
#define LCD_COLS 16

#define KPD_ROWS 4
#define KPD_COLS 4

#include <avr/io.h>
#include <util/delay.h>
#include <stdbool.h>

typedef enum {
	PORT_A,
	PORT_B,
	PORT_C,
	PORT_D
} port_name_t;


volatile uint8_t* lcd_ddr;
volatile uint8_t* lcd_port;

volatile uint8_t* kpd_ddr;
volatile uint8_t* kpd_pin;
volatile uint8_t* kpd_port;


void lcd_set_port_lower_nibble(uint8_t data){
	*lcd_port = (*lcd_port & 0x0F) | ((data << 4) & 0xF0);
}
void lcd_set_port_upper_nibble(uint8_t data){
	*lcd_port = (*lcd_port & 0x0F) | (data & 0xF0);
}
void lcd_transmit(){
	*lcd_port |= (1 << LCD_PIN_EN);   // EN = 1 to latch data
	_delay_us(1);
	*lcd_port &= ~(1 << LCD_PIN_EN);  // EN = 0

	_delay_us(200);
}

// Function to send a command to the LCD
void lcd_command(uint8_t cmd) {
	lcd_set_port_upper_nibble(cmd);
	*lcd_port &= ~(1 << LCD_PIN_RS);  // RS = 0 (command)
	lcd_transmit();

	lcd_set_port_lower_nibble(cmd);
	lcd_transmit();

	_delay_ms(2);
}
void lcd_print(uint8_t data) {
	lcd_set_port_upper_nibble(data);
	*lcd_port |= (1 << LCD_PIN_RS);  // RS = 1 (data)
	lcd_transmit();

	lcd_set_port_lower_nibble(data);
	lcd_transmit();

	_delay_ms(2);
}
void lcd_print_string(char* string, uint8_t length){
	for(uint8_t ind = 0; ind < length; ++ind){
		lcd_print(string[ind]);
	}
}

void lcd_clear() {
	lcd_command(LCD_CMD_CLEAR);
	_delay_ms(2);
}
void lcd_move_cursor(uint8_t row, uint8_t col) {
	if(row > 1 || col > 15) return;	// 2x16 lcd
	uint8_t address = row*LCD_DDRAM_ROW_OFFSET + col;
	lcd_command(LCD_SET_DDRAM_ADDR | address);
}

void lcd_define_customChar(uint8_t address, const uint8_t custom_char[8]){
	address &= 0x07;
	lcd_command(LCD_SET_CGRAM_ADDR | (address<<3));	//przesuniête 0 127
	for(uint8_t ind = 0; ind < 8; ++ind){
		lcd_print(custom_char[ind]);
	}
	lcd_command(LCD_CMD_RETURN_HOME);
	_delay_ms(2);
}

void lcd_init(port_name_t port_name) {
	switch (port_name){
		case PORT_A:
		lcd_ddr = &DDRA;
		lcd_port = &PORTA;
		break;
		case PORT_B:
		lcd_ddr = &DDRB;
		lcd_port = &PORTB;
		break;
		case PORT_C:
		lcd_ddr = &DDRC;
		lcd_port = &PORTC;
		break;
		case PORT_D:
		lcd_ddr = &DDRD;
		lcd_port = &PORTD;
		break;
	}

	*lcd_ddr = 0xFF; 	//portB as output
	_delay_ms(20);

	// Initialize in 4-bit mode
	lcd_command(LCD_CMD_RETURN_HOME);
	lcd_command(LCD_CMD_FUNCTIONSET | LCD_2LINE | LCD_5x8DOTS);	//Function set: 2 lines, 5x8 dots
	lcd_command(LCD_CMD_DISPLAYCONTROL | LCD_DISPLAY_ON | LCD_DISPLAY_CURSOR_OFF);	//display ON, cursor OFF
	lcd_command(LCD_CMD_CONFIGURE_ENTRY_MODE | LCD_CMD_SET_INCREMENT_ON_ENTRY | LCD_CMD_SET_SHIFT_COURSOR_ON_ENTRY);	//Entry mode set: Increment cursor

	lcd_clear();

	//CGRAM
	const uint8_t customChar1[8] = {
		0b00011111,
		0b00011011,
		0b00010101,
		0b00010101,
		0b00010101,
		0b00010101,
		0b00011011,
		0b00011111
	};
	lcd_define_customChar(LCD_CGRAM_C1, customChar1);
}


bool kpd_is_key_pressed(uint8_t row, uint8_t column){	//keypad indexing from 0
	*kpd_port = ~(1<<(column+4));
	_delay_ms(4);
	return ((*kpd_pin & (1<<row)) == 0);
}
bool kpd_is_key_pressed_indexed(uint8_t index){
	return kpd_is_key_pressed((index-1)/KPD_COLS, (index-1)%KPD_COLS);
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



int main(void) {
	port_name_t lcd_p = PORT_B, kpd_p = PORT_A;
	lcd_init(lcd_p);
	kpd_init(kpd_p);

	lcd_print('0');

	while (1) {
		if(kpd_is_key_pressed(0,3)){	//A
			lcd_move_cursor(0,0);
			lcd_print_string("Volodymyr",9);
			lcd_move_cursor(1,8);
			lcd_print_string("Tsukanov",8);
			_delay_ms(700);
		}
		if(kpd_is_key_pressed(1,3)){	//B
			lcd_clear();
			_delay_ms(700);
		}
		if(kpd_is_key_pressed(2,3)){	//C
			lcd_move_cursor(1,0);
			lcd_print(LCD_CGRAM_C1);
			_delay_ms(700);
		}
	}

	return 0;
}
