/*
    Hd44780 LCD adapter for an AVR ATMega32 microcontroller.
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
#ifndef LCD_HD44780_H
#define LCD_HD44780_H

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
#define LCD_DDRAM_BASE 0x80
	#define LCD_DDRAM_ROW_OFFSET 0x40
#define LCD_CGRAM_BASE 0x40
#define LCD_ROWS 2
#define LCD_COLS 16

#include "dft.h"
#include "alc.h"


volatile uint8_t* lcd_ddr;
volatile uint8_t* lcd_port;

uint8_t lcd_cursor_pos;


static inline void lcd_set_port_lower_nibble(uint8_t data){
	*lcd_port = (*lcd_port & 0x0F) | ((data << 4) & 0xF0);
}
static inline void lcd_set_port_upper_nibble(uint8_t data){
	*lcd_port = (*lcd_port & 0x0F) | (data & 0xF0);
}
static inline void lcd_transmit(){
	*lcd_port |= (1 << LCD_PIN_EN);   // EN = 1 to latch data
	_delay_us(1);
	*lcd_port &= ~(1 << LCD_PIN_EN);  // EN = 0
	_delay_us(200);
}

static inline void lcd_command(uint8_t cmd) {
	lcd_set_port_upper_nibble(cmd);
	*lcd_port &= ~(1 << LCD_PIN_RS);  // RS = 0 (command)
	lcd_transmit();

	lcd_set_port_lower_nibble(cmd);
	lcd_transmit();

	_delay_ms(2);
}
static inline void lcd_print(uint8_t data) {
	lcd_set_port_upper_nibble(data);
	*lcd_port |= (1 << LCD_PIN_RS);  // RS = 1 (data)
	lcd_transmit();

	lcd_set_port_lower_nibble(data);
	lcd_transmit();

	_delay_ms(2);
}
static inline void lcd_print_string(char* string, uint8_t length){
	for(uint8_t ind = 0; ind < length; ++ind){
		lcd_print(string[ind]);
	}
}
static inline void lcd_print_decimal(uint8_t number){
	if(number == 0){
		lcd_print('0');
		return;
	}
	uint8_t bufferRealSize = 0;
	uint8_t* buffer = (uint8_t*) alc_array_tmp(3,sizeof(uint8_t));
	while(number > 0){
		buffer[3-(++bufferRealSize)] = number%10;
		number /= 10;
	}
	for(uint8_t ind = 3-bufferRealSize; ind < 3; ind++)
		lcd_print('0'+buffer[ind]);
	alc_array_delete(buffer);
}
static inline void lcd_print_decimal_32(uint32_t number){
	if(number == 0){
		lcd_print('0');
		return;
	}
	uint8_t bufferRealSize = 0;
	uint8_t* buffer = (uint8_t*) alc_array_tmp(10,sizeof(uint8_t));
	while(number > 0){
		buffer[10-(++bufferRealSize)] = number%10;
		number /= 10;
	}
	for(uint8_t ind = 10-bufferRealSize; ind < 3; ind++)
		lcd_print('0'+buffer[ind]);
	alc_array_delete(buffer);
}

static inline void lcd_clear() {
	lcd_command(LCD_CMD_CLEAR);
	_delay_ms(2);
	lcd_cursor_pos = 0;
}
static inline void lcd_set_cursor_direct(uint8_t curs) {
	lcd_cursor_pos = curs;
	lcd_command(LCD_DDRAM_BASE | lcd_cursor_pos);
}
static inline void lcd_set_cursor(uint8_t row, uint8_t col) {
	if(row >= LCD_ROWS || col >= LCD_COLS) return;	// 2x16 lcd
	lcd_set_cursor_direct(row*LCD_DDRAM_ROW_OFFSET + col);
}

static inline bool lcd_define_customChar(uint8_t address, const uint8_t custom_char[8]){
	if(address > 8) return false;	//up to 8 custom chars
	lcd_command(LCD_CGRAM_BASE | (address<<3));	//offset by 127
	for(uint8_t ind = 0; ind < 8; ++ind){
		lcd_print(custom_char[ind]);
	}
	lcd_command(LCD_CMD_RETURN_HOME);
	_delay_ms(2);
	return true;
}

static inline void lcd_init(port_name_t port_name) {
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
		// Hard init
	lcd_set_port_lower_nibble(LCD_CMD_RETURN_HOME);
	_delay_us(300);
	lcd_set_port_lower_nibble(LCD_CMD_RETURN_HOME);
	_delay_us(300);
	lcd_set_port_lower_nibble(LCD_CMD_RETURN_HOME);
	_delay_us(300);

		// Soft init
	lcd_command(LCD_CMD_RETURN_HOME);
	lcd_command(LCD_CMD_FUNCTIONSET | LCD_4BMODE | LCD_2LINE | LCD_5x8DOTS);	//Function set: 2 lines, 5x8 dots
	lcd_command(LCD_CMD_DISPLAYCONTROL | LCD_DISPLAY_ON | LCD_DISPLAY_CURSOR_OFF);	//display ON, cursor OFF
	lcd_command(LCD_CMD_CONFIGURE_ENTRY_MODE | LCD_CMD_SET_INCREMENT_ON_ENTRY | LCD_CMD_SET_SHIFT_COURSOR_ON_ENTRY);	//Entry mode set: Increment cursor

	lcd_clear();
}

#endif  //LCD_HD44780_H
