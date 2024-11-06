/*
    default definitions, types, includes for neilA
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
#ifndef DEFAULTS_H
#define DEFAULTS_H

#define F_CPU 4000000	//CPU freq(HZ) for delay

#define GAME_UPTDELAY 50	//update delay in ms
//KeyPad inputs
#define GAME_INPUT_A 1
#define GAME_INPUT_UP 2
#define GAME_INPUT_B 3
#define GAME_INPUT_X 5
#define GAME_INPUT_DOWN 8
#define GAME_PLAYER_STARTPOINT 0x02
#define GAME_OBSTACLES_MIN 5
#define GAME_OBSTACLES_MAX 14
#define GAME_OBSTACLES_STARTPOINT 0x0F	//15
#define GAME_OBSTACLES_ENDPOINT 0x2A	//42

#include <avr/io.h>
#include <util/delay.h>
#include <stdbool.h>


typedef enum {
	PORT_A,
	PORT_B,
	PORT_C,
	PORT_D
} port_name_t;

#endif  //DEFAULTS_H
