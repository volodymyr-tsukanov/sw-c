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

#define PLR_STATUS_UP 1
#define PLR_STATUS_UPGOING 2
#define PLR_STATUS_DOWN 4
#define PLR_STATUS_DOWNGOING 8

#define PLR_ANIM_STATE_MAX 14
#define PLR_ANIM_STATE_MAIN 0
#define PLR_ANIM_FLIP_SPEED 11	//adjust to PLR_ANIM_FLIP_STEP

#define PLR_ACTION_FLIP_STEP 17	//less = faster


#include "dft.h"
#include "lcd.h"


uint8_t plr_pos;
uint8_t plr_status;
uint8_t plr_anim_state;
uint8_t plr_anim_counter;


static inline uint8_t plr_anim_get(const uint8_t state){
	const char* states = "$SI|/-\\-||IS$";
	return states[state >= PLR_ANIM_STATE_MAX ? PLR_ANIM_STATE_MAX-1 : state];	//for overflow resolving
}

static inline void plr_action_up(){
	if(plr_status == PLR_STATUS_DOWN){
		plr_anim_counter = 1;
		plr_anim_state = PLR_ANIM_STATE_MAX;
		plr_status = PLR_STATUS_UPGOING;
	} else {

	}
}

static inline void plr_action_down(){
	if(plr_status == PLR_STATUS_UP){
		plr_anim_counter = 1;
		plr_anim_state = 0;
		plr_status = PLR_STATUS_DOWNGOING;
	} else {

	}
}


// Game defaults
static inline void plr_init(){
	plr_pos = GAME_PLAYER_STARTPOINT;
	plr_status = PLR_STATUS_UP;
	plr_anim_state = PLR_ANIM_STATE_MAIN;
}

static inline void plr_update(){
	switch (plr_status){
	case PLR_STATUS_DOWNGOING:
		if(plr_anim_counter%PLR_ACTION_FLIP_STEP == 0){
			lcd_set_cursor_direct(plr_pos);
			lcd_print(' ');	//clears previous pos
			plr_pos += LCD_DDRAM_ROW_OFFSET;
			plr_status = PLR_STATUS_DOWN;
		} else {
			if(plr_anim_counter%PLR_ANIM_FLIP_SPEED){
				lcd_set_cursor_direct(plr_pos);
				lcd_print(plr_anim_get(plr_anim_state++));
			}
			++plr_anim_counter;
		}
		break;
	case PLR_STATUS_UPGOING:
		if(plr_anim_counter%PLR_ACTION_FLIP_STEP == 0){
			lcd_set_cursor_direct(plr_pos);
			lcd_print(' ');	//clears previous pos
			plr_pos -= LCD_DDRAM_ROW_OFFSET;
			plr_status = PLR_STATUS_UP;
		}  else {
			if(plr_anim_counter%PLR_ANIM_FLIP_SPEED){
				lcd_set_cursor_direct(plr_pos);
				lcd_print(plr_anim_get(--plr_anim_state));
			}
			++plr_anim_counter;
		}
		break;
	default:	//covers UP & DOWN
		lcd_set_cursor_direct(plr_pos);
		lcd_print(plr_anim_get(PLR_ANIM_STATE_MAIN));
		break;
	}
}


#endif  //PLAYER_H
