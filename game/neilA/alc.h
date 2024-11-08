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


static inline void* alc_array_new_fill(const uint8_t length, const size_t type_size){
	return calloc(length, type_size);
}
static inline void* alc_array_new(const uint8_t length, const size_t type_size){
	return malloc(length * type_size);
}

static inline void* alc_array_resize(void* arr, const uint8_t length, const size_t type_size){
	return realloc(arr, length * type_size);
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
