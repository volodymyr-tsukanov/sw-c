/*
    random generator for neilA
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
#ifndef RANDOM_H
#define RANDOM_H

#define RND_STEP 13 //in ms
#define RND_INPUT_MIN 5 //min inputs for seed generation
#define RND_INPUT_MAX 16
#define RND_INPUT_DELAY 380 //in ms

#include "dft.h"
#include "kpd.h"
#include "lcd.h"


uint32_t rnd_seed;


static inline void rnd_reseed(){
	const uint32_t a = 11679053;
	const uint32_t c = 1528009391;
	rnd_seed = a * rnd_seed + c;
}
static inline uint8_t rnd_lcg(){
	rnd_reseed();
	return rnd_seed;
}
static inline uint16_t rnd_lcg_16(){
	rnd_reseed();
	return rnd_seed;
}
static inline uint32_t rnd_lcg_32(){
	rnd_reseed();
	return rnd_seed;
}

static inline uint8_t rnd_adc(){
	return 0;
}

static inline uint8_t rnd_range(const uint8_t _from, const uint8_t _to){
	return _from + (rnd_lcg()%_to);
}


static inline void rnd_init(){  //uses GAME_INPUTs (UP,DOWN,X)
	uint8_t i = 0, status = 0;
	uint16_t buffer = 0;

	lcd_clear();
	lcd_print_string("|=-Game setup-=|", 16);
	lcd_set_cursor(1,0);

	rnd_seed = 0;
	while(i < RND_INPUT_MAX){
		if(kpd_is_key_pressed_indexed(GAME_INPUT_UP)){
			rnd_seed += buffer;
			buffer = 0;
			++status;
			++i;
			lcd_print('+');
			_delay_ms(RND_INPUT_DELAY);
		} else if(kpd_is_key_pressed_indexed(GAME_INPUT_X)){
			rnd_seed += buffer*4;
			buffer = 0;
			status = 9;
			++i;
			lcd_print('*');

			if(i >= RND_INPUT_MIN) break;

			_delay_ms(RND_INPUT_DELAY);
		} else if(kpd_is_key_pressed_indexed(GAME_INPUT_DOWN)){
			rnd_seed -= buffer;
			buffer = 0;
			status = 0;
			++i;
			lcd_print('-');
			_delay_ms(RND_INPUT_DELAY);
		} else {
			switch(status){
				case 0:
					++buffer;
				break;
				default:
					buffer += status * i;
				break;
			}
		}

		_delay_ms(RND_STEP);
	}
	lcd_clear();
}

#endif  //RANDOM_H
