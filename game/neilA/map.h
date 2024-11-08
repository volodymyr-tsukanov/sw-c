/*
    map container, includes obstacles, enemies, player
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
#ifndef MAP_H
#define MAP_H

#define MAP_FREE_DIST 5	//distance between cells

#include "dft.h"
#include "alc.h"
#include "rnd.h"
//game interfaces
#include "lvl.h"


uint8_t map_obj_size, map_obj_index, map_free_pos_size, map_free_pos_index, map_id_index;
uint8_t* map_free_pos_pool;
obj_t* map_obj_pool;


static inline bool map_find_free_places(uint8_t from, uint8_t to){	//place = row
	bool valid;
	uint8_t count = 0, ind, free_dist = MAP_FREE_DIST-lvl_get_difficulty_multiplier(), iterations = 0;
	map_free_pos_index = 0;
	map_free_pos_size = (to-from)/free_dist - 1;
	map_free_pos_pool = (uint8_t*) alc_array_new(map_free_pos_size,sizeof(uint8_t));
	while (count < map_free_pos_size){
		uint8_t pos = rnd_range(from, to);
		valid = true;
		for(ind = 0; ind < count; ind++){
			uint8_t dist = pos > map_free_pos_pool[ind] ? pos-map_free_pos_pool[ind] : map_free_pos_pool[ind]-pos;
			if(dist < free_dist){
				valid = false;
				break;
			}
		}
		if(valid){
			map_free_pos_pool[count++] = pos;
		}
		if(++iterations > 100) return false;
	}
	return true;
}

static inline uint8_t map_take_free_place(){	//returns position and removes places from free
	if(map_free_pos_index == map_free_pos_size)
		map_find_free_places(GAME_OBSTACLES_STARTPOINT,GAME_MAP_ENDPOINT);
	return map_free_pos_size > 0 ? map_free_pos_pool[map_free_pos_index++] : 0;
}


#include "obt.h"

static inline bool map_obj_new(uint8_t class_id){
	if(map_obj_index >= map_obj_size) return false;
	switch (class_id){
	case GAME_CLASS_OBSTACLE:
		map_obj_pool[map_obj_index].id = (GAME_CLASS_OBSTACLE<<12) + (++map_id_index);
		obt_init(&map_obj_pool[map_obj_index++]);
		break;
	default:
		break;
	}
	return true;
}


// Game defaults
static inline void map_init(){
	map_id_index = 0;
	map_obj_index = 0;
	map_obj_size = GAME_OBSTACLES_MAX + 1;	//1 is player
	map_obj_pool = (obj_t*) alc_array_new(map_obj_size,sizeof(obj_t));
	map_find_free_places(GAME_OBSTACLES_STARTPOINT,GAME_MAP_ENDPOINT);
}
static inline void map_destroy(){
	map_obj_size = 0;
	map_obj_pool = alc_array_delete(map_obj_pool);
	map_free_pos_pool = alc_array_delete(map_free_pos_pool);
}

static inline void map_update(){
	uint8_t ind = 0;
	//UPDATE components
	for( ; ind < map_obj_size; ind++){
		uint8_t class_id = map_obj_pool[ind].id >> 12;
		switch (class_id){
		case GAME_CLASS_OBSTACLE:
			obt_update(&map_obj_pool[ind]);
			break;
		default:
			break;
		}
	}
}

static inline void map_complicate(){
	map_obj_new(GAME_CLASS_OBSTACLE);
}

#endif  //MAP_H
