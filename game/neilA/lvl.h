/*
    neilA general level interface, updates all elements
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
#ifndef LEVEL_H
#define LEVEL_H

#define LVL_STATUS_RUN 0
#define LVL_STATUS_PAUSE 1
#define LVL_STATUS_LOOSE 13
#define LVL_STATUS_DESTROYED 19

#define LVL_SCORE_STEP 77	//more = slower ; needs to be first
#define LVL_COMPLICATE_STEP 487	//tied to lvl_score


#include "dft.h"
#include "alc.h"
#include "lcd.h"
#include "kpd.h"


uint8_t lvl_score;
uint16_t lvl_score_counter;
uint8_t lvl_status;


static inline uint8_t lvl_get_score(){
	return lvl_score;
}
static inline uint8_t lvl_get_difficulty_multiplier(){	//from 1 to 4
	uint8_t difficulty = lvl_score / 64;
	if(difficulty == 0) difficulty = 1;
	return difficulty;
}

static inline bool lvl_is_visible(uint8_t pos){	//check if pos is visible on map
	if(pos > LCD_DDRAM_ROW_OFFSET){	//row 2
		pos -= LCD_DDRAM_ROW_OFFSET;
	}
	return pos <= LCD_COLS;
}

static inline void lvl_report_kill_player(){
	lvl_status = LVL_STATUS_LOOSE;
	lcd_set_cursor(0,5);
	lcd_print_string("SCORE: ",7);
	lcd_print_decimal(lvl_score);
	lcd_print(' ');
	lcd_set_cursor(1,2);
	lcd_print_string("X to restart",12);
}

// Game defaults
#include "map.h"
#include "plr.h"
static inline void lvl_init(){
	plr_init();
	map_init();
}
static inline void lvl_destroy(){
	map_destroy();
	lvl_status = LVL_STATUS_DESTROYED;
}

static inline void lvl_start(){
	uint8_t ind = 0;
	lvl_score = 1;
	lvl_score_counter = 1;	//to wait 1 cycle before score increases
	lvl_status = LVL_STATUS_RUN;

	//START spawn
	for( ; ind < GAME_OBSTACLES_MIN; ind++){
		map_obj_new(GAME_CLASS_OBSTACLE);
	}
}

static inline void lvl_update(){
	switch (lvl_status){
	case LVL_STATUS_RUN:
		//CHECK input
		if(kpd_is_key_pressed_indexed(GAME_INPUT_UP))
			plr_action_up();
		else if(kpd_is_key_pressed_indexed(GAME_INPUT_DOWN))
			plr_action_down();
		else if(kpd_is_key_pressed_indexed(GAME_INPUT_LEFT))
			plr_action_left();
		else if(kpd_is_key_pressed_indexed(GAME_INPUT_RIGHT))
			plr_action_right();

		//UPDATE components
		plr_update();
		map_update();

		//POST-UPDATE
		if(lvl_score_counter%LVL_SCORE_STEP == 0){
			++lvl_score;
			/*lcd_set_cursor(1,0);	//DEBUG
			lcd_print_decimal(lvl_score);
			lcd_set_cursor(0,0);
			lcd_print_decimal(lvl_get_difficulty_multiplier());*/
		}
		if(lvl_score_counter%LVL_COMPLICATE_STEP == 0){	//COMPLICATE
			map_complicate();
			/*lcd_set_cursor(0,0);	//DEBUG
			lcd_print(1);*/
		}
		++lvl_score_counter;
		break;
	case LVL_STATUS_LOOSE:
		//CHECK input
		if(kpd_is_key_pressed_indexed(GAME_INPUT_X)){
			lvl_destroy();
			_delay_ms(100);
			lcd_clear();
			lvl_init();
			lvl_start();
		}
		break;
	default:
		break;
	}
}

#endif  //LEVEL_H
