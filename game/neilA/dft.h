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