/*
    obstacle (called "bug") interface
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
#ifndef OBSTACLES_H
#define OBSTACLES_H

#define OBT_SPEED_MAX 4
#define OBT_SPEED_STEP 13	//less = faster obstacles
#define OBT_POOL_INCREMENT_STEP 31	//tied to lvl_score
#define OBT_ANIM_STATE_MAX 7
#define OBT_ANIM_COUNTER_STEP 7	//must be less than OBT_STEP

#include "dft.h"
#include "alc.h"
#include "rnd.h"
#include "lcd.h"
//game interfaces
#include "lvl.h"

typedef struct {
	uint8_t pos;
	uint8_t speed;
	uint8_t anim_state;
	uint8_t step_counter;
} obt_t;


uint8_t obt_count;
obt_t* obt_pool;


static inline uint8_t obt_anim_get(uint8_t state){
	const char* states = "@QS#D0O";
	return states[state];
}

static inline void obt_draw(uint8_t index){
	if(lvl_is_visible(obt_pool[index].pos)){	//occlusion culling
		lcd_set_cursor_direct(obt_pool[index].pos);
		lcd_print(obt_anim_get(obt_pool[index].anim_state));
		lcd_print(' ');	//clears previous pos

		obt_pool[index].anim_state += 1;
		//CHECK animation state overflow
		if(obt_pool[index].anim_state >= OBT_ANIM_STATE_MAX)
			obt_pool[index].anim_state = 0;
	}
}

static inline void obt_create_new(){
	if(obt_count < GAME_OBSTACLES_MAX){
		obt_pool[obt_count].speed = rnd_range(0, OBT_SPEED_MAX);
		obt_pool[obt_count].anim_state = rnd_range(0,OBT_ANIM_STATE_MAX);
		obt_pool[obt_count].step_counter = 0;
		obt_pool[obt_count].pos = rnd_range(GAME_OBSTACLES_STARTPOINT, GAME_OBSTACLES_ENDPOINT);
		if(rnd_lcg() > 127)	//50% chance
			obt_pool[obt_count].pos += LCD_DDRAM_ROW_OFFSET;
		
		++obt_count;
	}
}
static inline void obt_recreate(uint8_t index){	//replaces old obstacle with new one in the pool
	obt_pool[index].speed = rnd_range(0, OBT_SPEED_MAX);
	obt_pool[index].anim_state = rnd_range(0,OBT_ANIM_STATE_MAX);
	obt_pool[index].step_counter = 0;
	obt_pool[index].pos = rnd_range(GAME_OBSTACLES_STARTPOINT, GAME_OBSTACLES_ENDPOINT);
	if(rnd_lcg() > 127)	//50% chance
		obt_pool[index].pos += LCD_DDRAM_ROW_OFFSET;

	//CHECK
	/*if(lvl_get_score()%OBT_POOL_INCREMENT_STEP == 0)
		obt_create_new();*/
}

static inline void obt_refresh(uint8_t index){	//update for pool element
	uint8_t step = OBT_SPEED_STEP - obt_pool[index].speed - lvl_get_difficulty_multiplier();

	if(obt_pool[index].step_counter%OBT_ANIM_COUNTER_STEP == 0){	//animation
		obt_draw(index);
	}
	if(obt_pool[index].step_counter%step == 0){	//step
		if(obt_pool[index].pos == 0 || obt_pool[index].pos == LCD_DDRAM_ROW_OFFSET){	//out of map
			lcd_set_cursor_direct(obt_pool[index].pos);
			lcd_print(' ');lcd_print(' ');	//hides current obstacle
			obt_recreate(index);
		} else if(obt_pool[index].pos == plr_pos){	//player collision
			lvl_report_kill_player();
		} else{
			obt_pool[index].pos -= 1;
		}
	}
	obt_pool[index].step_counter += 1;
}


// Game defaults
static inline void obt_init(){
	uint8_t i = 0;
	obt_count = GAME_OBSTACLES_MIN;
	obt_pool = (obt_t*) alc_array_tmp(GAME_OBSTACLES_MAX,sizeof(obt_t));

	for(i = 0; i < obt_count; i++){
		obt_recreate(i);
	}
}
static inline void obt_destroy(){
	obt_pool = alc_array_delete(obt_pool);
}

static inline void obt_update(){
	uint8_t i = 0;
	for(i = 0; i < obt_count; i++){
		obt_refresh(i);
	}
}

#endif  //OBSTACLES_H
