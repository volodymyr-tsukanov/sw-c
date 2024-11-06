/*
    allocator defines memory managment templates (arrays mostly) for AVR ATMega32 microcontroller.
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
#ifndef ALLOCATOR_H
#define ALLOCATOR_H

#include <stdlib.h>
#include "dft.h"


static inline void* alc_array_new(uint8_t length, size_t type_size){
	return calloc(length, type_size);
}
static inline void* alc_array_tmp(uint8_t length, size_t type_size){
	return malloc(length * type_size);
}

static inline bool alc_array_resize(void* arr, uint8_t length, size_t type_size){
	arr = realloc(arr, length * type_size);
	return arr != NULL;
}

static inline void* alc_array_delete(void* arr){	//eg. arr = <this-func>(arr)
	if(arr != NULL){
		free(arr);
		arr = NULL;
		return NULL;
	}
	return arr;
}


#endif	//ALLOCATOR_H
