/*
    neilA is a simple game for an AVR ATMega32 microcontroller + Hd44780 LCD.
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
#include "dft.h"
//hardware adapters
#include "lcd.h"
#include "kpd.h"
//libs
#include "alc.h"
#include "rnd.h"
//game interfaces
#include "lvl.h"
//debug
//#include "dbg.h"


int main(void) {
	port_name_t lcd_p = PORT_B, kpd_p = PORT_A;
	const uint8_t cc_o_crossed[8] = {
		0b01110,
		0b11011,
		0b10101,
		0b10101,
		0b11011,
		0b10101,
		0b11011,
		0b01110
	};
	const uint8_t cc_s[8] = {
		0b00000,
		0b01111,
		0b10000,
		0b01110,
		0b00001,
		0b11110,
		0b00000,
		0b00000
	};
	const uint8_t cc_s2[8] = {
		0b00000,
		0b00110,
		0b01000,
		0b00100,
		0b00010,
		0b01100,
		0b00000,
		0b00000
	};
	
	// INIT
	  // Hardware init
	lcd_init(lcd_p);
	lcd_define_customChar(GAME_CG_CROSS,cc_o_crossed);
	lcd_define_customChar(GAME_CLASS_PLAYER_CG_SHRINK_1,cc_s);
	lcd_define_customChar(GAME_CLASS_PLAYER_CG_SHRINK_2,cc_s2);
	kpd_init(kpd_p);
	  // Game init
	lvl_init();
	lvl_start();
	  //Lib init
	rnd_init();

	while (1){
		lvl_update();
		_delay_ms(GAME_UPTDELAY);
	}

	return 0;
}
