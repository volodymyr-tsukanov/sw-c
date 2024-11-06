/*
    neilA general level interface (contains obstacles, enemies, player)
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

#include "dft.h"
#include "alc.h"
#include "lcd.h"


uint8_t lvl_score;
uint8_t lvl_status;
uint8_t* lvl_map;


static inline uint8_t lvl_get_difficulty_multiplier(){	//from 1 to 4
	uint8_t difficulty = lvl_score / 64;
	if(difficulty == 0) difficulty = 1;
	return difficulty;
}

static inline void lvl_report_kill_player(){
	lvl_status = LVL_STATUS_LOOSE;
}


//components
#include "plr.h"
#include "obt.h"
// Game defaults
static inline void lvl_init(){
	lvl_map = alc_array_new(GAME_MAP_SIZE,sizeof(uint8_t));

	//INIT components
	plr_init();
	obt_init();
}

static inline void lvl_start(){
	lvl_score = 1;
	lvl_status = LVL_STATUS_RUN;
}

static inline void lvl_update(){
	switch (lvl_status)
	{
	case LVL_STATUS_RUN:
		//UPDATE components
		plr_update();
		obt_update();
		break;
	default:
		break;
	}
}

static inline void lvl_destroy(){
	lvl_map = alc_array_delete(lvl_map);
}

#endif  //LEVEL_H
