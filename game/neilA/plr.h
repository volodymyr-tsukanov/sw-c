/*
    player interface
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
#ifndef PLAYER_H
#define PLAYER_H

#define PLR_ANIM_STATE_MAX 7
#define PLR_ANIM_COUNTER_STEP 5

#include "dft.h"
#include "lcd.h"


uint8_t plr_pos;
uint8_t plr_anim_state;
uint8_t plr_anim_counter;


static inline uint8_t plr_anim_get(uint8_t state){
	const char* states = "@QS$D0O";
	return states[state];
}


// Game defaults
static inline void plr_init(){
	plr_pos = GAME_PLAYER_STARTPOINT;
	plr_anim_state = 3;
}

static inline void plr_update(){
	/*if(plr_anim_counter%PLR_ANIM_COUNTER_STEP == 0){
		lcd_set_cursor_direct(plr_pos);
		lcd_print(plr_anim_get(plr_anim_state));

		++plr_anim_state;
		if(plr_anim_state >= PLR_ANIM_STATE_MAX)
			plr_anim_state = 0;
	}
	++plr_anim_counter;*/

	lcd_set_cursor_direct(plr_pos);
	lcd_print(plr_anim_get(plr_anim_state));
}


#endif  //PLAYER_H
