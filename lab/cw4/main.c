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
#define F_CPU 4000000	//częstotliwość procesora dla delay

#define LCD_PIN_EN 0
#define LCD_PIN_RS 1
#define LCD_CMD_CLEAR 0x01
#define LCD_CMD_RETURN_HOME 0x02
#define LCD_CMD_ENTRY_MODE 0x04
#define LCD_CMD_DISPLAY_ON 0x0C
#define LCD_CMD_FUNCTION_SET 0x20
#define LCD_CMD_CONFIGURE_ENTRY_MODE (1<<2)
	#define LCD_CMD_SET_DECREMENT_ON_ENTRY (0<<1)
	#define LCD_CMD_SET_INCREMENT_ON_ENTRY (1<<1)
	#define LCD_CMD_SET_SHIFT_COURSOR_ON_ENTRY (0<<0)
	#define LCD_CMD_SET_SHIFT_WINDOW_ON_ENTRY (1<<0)


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


void lcd_set_instruction_transmission_mode(){	//wyczyść bit portu ustawiający rodzaj transmisji.
	*lcd_port &= ~(1 << LCD_PIN_RS);	//RS = 0
}
void lcd_set_data_transmsision_mode(){	//ustaw bit portu ustawiający rodzaj transmisji.
	*lcd_port |= (1 << LCD_PIN_RS);	//RS = 1
}
void lcd_begin_transmission(){	//ustaw odpowiedni bit portu włączając transmisję.
	*lcd_port &= ~(1 << LCD_PIN_EN);	//EN = 0
}
void lcd_end_transmission(){	//wyczyść odpowiedni bit portu wyłączając transmisję.
	*lcd_port |= (1 << LCD_PIN_EN);	//EN = 1
}
void lcd_set_port_lower_nibble(uint8_t data){	//wyczyść 4 bity portu z danymi i ustaw na wyjście niskie bity danych.
	*lcd_port &= 0xF0;	//wyczyść
	*lcd_port |= data << 4;	//ustaw niskie
}
void lcd_set_port_upper_nibble(uint8_t data){	//wyczyść 4 bity portu z danymi i ustaw na wyjście wysokie bity danych.
	*lcd_port &= 0x0F;	//wyczyść
	*lcd_port |= data & 0xF0;	//ustaw wysokie
}

void lcd_write_byte(uint8_t data){	//rozpocznij transmisję, prześlij najpierw wysokie bity, zakończ transmisję i powtórz dla niskich bitów. Pamiętaj o dodaniu opóźnienia.
	lcd_begin_transmission();
	lcd_set_port_upper_nibble(data);
	lcd_end_transmission();
	_delay_ms(4);
	lcd_begin_transmission();
	lcd_set_port_lower_nibble(data);
	lcd_end_transmission();
	_delay_ms(4);
}
void lcd_write_command(uint8_t command){	//ustaw odpowiedni tryb transmisji i prześlij komendę. Dzięki wcześniejszemu mapowaniu bitów instrukcji sterownika można teraz wykonać komendy w następujący sposób:
	lcd_set_instruction_transmission_mode();
	lcd_write_byte(command);
}
void lcd_write_character(uint8_t character){		//ustaw odpowiedni tryb transmisji i prześlij znak w postaci bajtu.
	lcd_set_data_transmsision_mode();
	lcd_write_byte(character);
}
void lcd_return_home(){	//wykonaj komendę powrotu kursora na pozycję początkową.
	lcd_write_command(LCD_CMD_RETURN_HOME);
	_delay_ms(4);
}
void lcd_clear(){	//wykonaj komendę czyszczenia ekranu.
	lcd_write_command(LCD_CMD_CLEAR);
	_delay_ms(4);
}
void lcd_move_coursor_to(uint8_t row, uint8_t column){	//wykonaj komendę zmiany adresu komórki DDRAM (czyli adresu kursora)
	uint8_t curs_addr = (column-1) + 0x40*(row-1);
	lcd_write_command(curs_addr | 0b10000000);
}
void lcd_write_string(uint8_t* data, uint8_t length){	//wysyłaj dane dla całego łańcucha znaków.
	for(int i = 0; i < length; i++)
		lcd_write_character(data[i]);
}
void lcd_clear_charcters(uint8_t row, uint8_t start_column, uint8_t n_characters){	//przejdź kursorem do odpowiedniego miejsca i napisz n razy znak spacji.
	lcd_move_coursor_to(row, start_column);
	for(uint8_t i = 0; i < n_characters; i++)
		lcd_write_character(0);
}

void lcd_init(port_name_t port_name){
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

	// Set LCD pins as output
	*lcd_ddr |= (1 << LCD_PIN_EN) | (1 << LCD_PIN_RS);	// Enable EN and RS pins
	*lcd_ddr |= 0xF0; // data pins are upper 4 bits

	_delay_ms(25);

	// Function Set
	//lcd_write_command(LCD_CMD_FUNCTION_SET | 1);	//1, 3
	//lcd_write_command(LCD_CMD_FUNCTION_SET | (1 << 2)); // 8b, 1ln
	_delay_ms(4);

	// Display ON
	lcd_write_command(LCD_CMD_DISPLAY_ON);

	lcd_clear();

	// Entry Mode
	lcd_write_command(LCD_CMD_ENTRY_MODE | LCD_CMD_CONFIGURE_ENTRY_MODE | LCD_CMD_SET_INCREMENT_ON_ENTRY);
}


int main(void)
{
	port_name_t lcd_port = PORT_B;
	uint8_t row = 1, col = 1;

    lcd_init(lcd_port);
	for(int i = 0; i < 256; i++){
		lcd_write_character(i); _delay_ms(100);
	}
}
