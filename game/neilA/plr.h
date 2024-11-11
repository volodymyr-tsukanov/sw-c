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
#define PLR_STATUS_DOWN 3
#define PLR_STATUS_DOWNGOING 4

#define PLR_ANIM_STATE_MAX 15
#define PLR_ANIM_STATE_MAIN 0
#define PLR_ANIM_STATE_TP 1
	#define PLR_ANIM_STATE_TP_HEAD 1
	#define PLR_ANIM_STATE_TP_SIZE 6
#define PLR_ANIM_STATE_SLIDEL 2
#define PLR_ANIM_STATE_SLIDER 3
	#define PLR_ANIM_STATE_SLIDE_HEAD 7
	#define PLR_ANIM_STATE_SLIDE_SWAP 10
	#define PLR_ANIM_STATE_SLIDE_SIZE 8

#define PLR_ACTION_FLIP_STEP 17	//less = faster
#define PLR_ACTION_MOVE_STEP 11

#define PLR_LIMIT_LEFT 0x02
#define PLR_LIMIT_RIGHT 0x0C


#include "dft.h"
#include "lcd.h"


uint8_t plr_pos;
uint8_t plr_status;
uint8_t plr_recoil;
uint8_t plr_anim_state;
uint8_t plr_anim_counter;


static inline uint8_t plr_anim_get(const uint8_t state){
	const char states[PLR_ANIM_STATE_MAX] = {
		'$',	//0 main
		GAME_CLASS_PLAYER_CG_SHRINK_2,	//1 tp head
		'0',
		'O',
		GAME_CG_CROSS,
		'*',
		' ',		//6 tp tail
		GAME_CLASS_PLAYER_CG_SHRINK_1,	//7 slide head
		GAME_CLASS_PLAYER_CG_SHRINK_2,
		'|',
		'-',	//10 slide swap
		'/',
		'|',
		GAME_CLASS_PLAYER_CG_SHRINK_2,
		GAME_CLASS_PLAYER_CG_SHRINK_1,	//14 slide tail
	};
	return states[state >= PLR_ANIM_STATE_MAX ? '$' : state];	//for state overflow resolving
}

static inline void plr_set_status(const uint8_t status){
	plr_anim_counter = 1;
	plr_status = status;
}
static inline void plr_set_anim_state(const uint8_t state){	//8 states possible
	plr_anim_state = (state<<5);	//set state to upper 3 bits
}

static inline void plr_animate(){
	uint8_t state = (plr_anim_state&0b00011111), anim_speed = plr_recoil;

	switch (plr_anim_state>>5){	//specify anim (masked state)
	case PLR_ANIM_STATE_TP:
		anim_speed /= PLR_ANIM_STATE_TP_SIZE;
		state += PLR_ANIM_STATE_TP_HEAD;
		if(state >= PLR_ANIM_STATE_TP_HEAD+PLR_ANIM_STATE_TP_SIZE)
			state = PLR_ANIM_STATE_TP_HEAD+PLR_ANIM_STATE_TP_SIZE-1;
		break;
	case PLR_ANIM_STATE_SLIDEL:
		anim_speed /= PLR_ANIM_STATE_SLIDE_SIZE;
		state += PLR_ANIM_STATE_SLIDE_HEAD;
		if(state >= PLR_ANIM_STATE_SLIDE_HEAD+PLR_ANIM_STATE_SLIDE_SIZE)
			state = PLR_ANIM_STATE_SLIDE_HEAD+PLR_ANIM_STATE_SLIDE_SIZE-1;
		break;
	case PLR_ANIM_STATE_SLIDER:
		anim_speed /= PLR_ANIM_STATE_SLIDE_SIZE;
		state += PLR_ANIM_STATE_SLIDE_HEAD;
		if(state >= PLR_ANIM_STATE_SLIDE_HEAD+PLR_ANIM_STATE_SLIDE_SIZE)
			state = PLR_ANIM_STATE_SLIDE_HEAD+PLR_ANIM_STATE_SLIDE_SIZE-1;
		if(state == PLR_ANIM_STATE_SLIDE_SWAP)
			++state;
		else if(state == PLR_ANIM_STATE_SLIDE_SWAP+1)
			--state;
		break;
	default:	//covers PLR_ANIM_STATE_MAIN
		state = PLR_ANIM_STATE_MAIN;
		break;
	}

	/*lcd_set_cursor(0,0);	//DEBUG
	lcd_print_decimal(plr_anim_state); lcd_print(' ');
	lcd_set_cursor(1,0);
	lcd_print_decimal(state); lcd_print(' '); lcd_print_decimal(anim_speed); lcd_print(' ');*/

	if(plr_anim_counter%anim_speed == 0){	//display anim
		lcd_set_cursor_direct(plr_pos);
		lcd_print(plr_anim_get(state));
		++plr_anim_state;
	}
	++plr_anim_counter;
}

static inline void plr_action_up(){
	if(plr_status == PLR_STATUS_DOWN){
		plr_recoil = PLR_ACTION_FLIP_STEP;
		plr_set_anim_state(PLR_ANIM_STATE_TP);
		plr_set_status(PLR_STATUS_UPGOING);
	}
}
static inline void plr_action_down(){
	if(plr_status == PLR_STATUS_UP){
		plr_recoil = PLR_ACTION_FLIP_STEP;
		plr_set_anim_state(PLR_ANIM_STATE_TP);
		plr_set_status(PLR_STATUS_DOWNGOING);
	}
}
static inline void plr_action_left(){
	if(plr_status == PLR_STATUS_DOWN || plr_status == PLR_STATUS_UP){
		if(plr_pos < PLR_LIMIT_LEFT || (plr_pos > LCD_DDRAM_ROW_OFFSET && plr_pos < LCD_DDRAM_ROW_OFFSET+PLR_LIMIT_LEFT)) return;
		lcd_set_cursor_direct(plr_pos--);
		lcd_print(' ');	//clears previous pos
		plr_recoil = PLR_ACTION_MOVE_STEP;
		plr_set_anim_state(PLR_ANIM_STATE_SLIDEL);
		plr_set_status(plr_status+1);	//status = *GOING
	}
}
static inline void plr_action_right(){
	if(plr_status == PLR_STATUS_DOWN || plr_status == PLR_STATUS_UP){
		if(plr_pos < PLR_LIMIT_RIGHT || (plr_pos > LCD_DDRAM_ROW_OFFSET && plr_pos < LCD_DDRAM_ROW_OFFSET+PLR_LIMIT_RIGHT)){
			lcd_set_cursor_direct(plr_pos++);
			lcd_print(' ');	//clears previous pos
			plr_recoil = PLR_ACTION_MOVE_STEP;
			plr_set_anim_state(PLR_ANIM_STATE_SLIDER);
			plr_set_status(plr_status+1);	//status = *GOING
		}
	}
}


// Game defaults
static inline void plr_init(){
	plr_pos = GAME_PLAYER_STARTPOINT;
	plr_set_status(PLR_STATUS_UP);
	plr_anim_state = PLR_ANIM_STATE_MAIN;
}

static inline void plr_update(){
	switch (plr_status){
	case PLR_STATUS_DOWNGOING:
		if(plr_anim_counter%plr_recoil == 0){
			lcd_set_cursor_direct(plr_pos);
			if(plr_pos < LCD_DDRAM_ROW_OFFSET) plr_pos += LCD_DDRAM_ROW_OFFSET;
			plr_set_status(PLR_STATUS_DOWN);
		} else {
			plr_animate();
		}
		break;
	case PLR_STATUS_UPGOING:
		if(plr_anim_counter%plr_recoil == 0){
			lcd_set_cursor_direct(plr_pos);
			if(plr_pos > LCD_DDRAM_ROW_OFFSET) plr_pos -= LCD_DDRAM_ROW_OFFSET;
			plr_set_status(PLR_STATUS_UP);
		}  else {
			plr_animate();
		}
		break;
	default:	//covers UP & DOWN
		lcd_set_cursor_direct(plr_pos);
		lcd_print(plr_anim_get(PLR_ANIM_STATE_MAIN));
		break;
	}
}


#endif  //PLAYER_H
