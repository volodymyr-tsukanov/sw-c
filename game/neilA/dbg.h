/*
    debbuger for neilA (build excluded)
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
#ifndef DEBUG_H
#define DEBUG_H

#include "dft.h"
#include "alc.h"


bool loadRAMnoFree(int n){  //around 1950
    int i = 0;
    uint8_t* t = (uint8_t*) calloc(n, sizeof(uint8_t));
    if(t == NULL) return false;
    for(i = 0; i < n; i++)
        t[i] = 0xFF;
    t[n-1] = 99;
    lcd_print_decimal(t[n-1]);
    return true;
}
bool loadRAMfree(int n){
    int i = 0;
    uint8_t* t = (uint8_t*) calloc(n, sizeof(uint8_t));
    if(t == NULL) return false;
    for(i = 0; i < n; i++)
        t[i] = 0xFF;
    t[n-1] = 99;
    lcd_print_decimal(t[n-1]);
    free(t);
    return true;
}

bool loadRAMlibNoFree(int n){
    int i = 0;
    uint8_t* t = alc_array_new_fill(n,sizeof(uint8_t));
    if(t == NULL) return false;
    for(i = 0; i < n; i++)
        t[i] = 0xFF;
    t[n-1] = 99;
    lcd_print_decimal(t[n-1]);
    return true;
}
bool loadRAMlibFree(int n){
    int i = 0;
    uint8_t* t = alc_array_new_fill(n,sizeof(uint8_t));
    if(t == NULL) return false;
    for(i = 0; i < n; i++)
        t[i] = 0xFF;
    t[n-1] = 99;
    lcd_print_decimal(t[n-1]);
    t = alc_array_delete(t);
    return true;
}

#endif  //DEBUG_H
