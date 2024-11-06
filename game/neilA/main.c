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


int main(void) {
	port_name_t lcd_p = PORT_B, kpd_p = PORT_A;
	
	// INIT
	  // Hardware init
	lcd_init(lcd_p);
	kpd_init(kpd_p);
	  // Lib init
	//rnd_init();
	  // Game init
	lvl_init();
	lvl_start();

	while (1){
		lvl_update();
		_delay_ms(GAME_UPTDELAY);
	}

	return 0;
}
