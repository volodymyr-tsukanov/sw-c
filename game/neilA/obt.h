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

#define OBT_SPEED_MAX 6
#define OBT_SPEED_STEP 21	//less = faster obstacles
#define OBT_ANIM_STATE_MAX 7
#define OBT_ANIM_COUNTER_STEP 7	//must be less than OBT_STEP

#include "dft.h"
#include "lcd.h"
//game interfaces
#include "lvl.h"
#include "map.h"
#include "plr.h"


static inline uint8_t obt_anim_get(const uint8_t state){
	const char* states = "@QS#D0O";
	return states[state];
}

static inline void obt_draw(obj_t* obstacle){
	if(lvl_is_visible(obstacle->pos)){	//occlusion culling
		lcd_set_cursor_direct(obstacle->pos);
		lcd_print(obt_anim_get(obstacle->anim_state));
		lcd_print(' ');	//clears previous pos

		obstacle->anim_state = obstacle->anim_state < OBT_ANIM_STATE_MAX-1 ? obstacle->anim_state+1 : 0;
	}
}


// Game defaults
static inline void obt_init(obj_t* obstacle){
	obstacle->speed = rnd_range(0, OBT_SPEED_MAX);
	obstacle->anim_state = rnd_range(0,OBT_ANIM_STATE_MAX);
	obstacle->step_counter = 0;
	obstacle->pos = map_take_free_place();
	if(rnd_lcg() > 127)	//50% chance
		obstacle->pos += LCD_DDRAM_ROW_OFFSET;
}

static inline void obt_update(obj_t* obstacle){
	uint8_t step = OBT_SPEED_STEP - obstacle->speed - lvl_get_difficulty_multiplier();

	if(obstacle->step_counter%OBT_ANIM_COUNTER_STEP == 0){	//animation
		obt_draw(obstacle);
	}
	if(obstacle->step_counter%step == 0){	//step
		if(obstacle->pos == 0 || obstacle->pos == LCD_DDRAM_ROW_OFFSET){	//out of map
			lcd_set_cursor_direct(obstacle->pos);
			lcd_print(' ');lcd_print(' ');	//hides current obstacle
			obt_init(obstacle);
		} else if(obstacle->pos == plr_pos){	//player collision
			lvl_report_kill_player();
		} else{
			obstacle->pos -= 1;
		}
	}
	obstacle->step_counter += 1;
}

#endif  //OBSTACLES_H
