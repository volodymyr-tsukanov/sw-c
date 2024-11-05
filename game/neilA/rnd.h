/*
    random generator for neilA
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
#ifndef RANDOM_H
#define RANDOM_H

#define RND_STEP 42 //in ms
#define RND_INPUT_MIN 9 //min inputs for seed generation
#define RND_INPUT_MAX 19

#include "dft.h"
#include "alc.h"
#include "kpd.h"
#include "lcd.h"


uint8_t rnd_seed;


static inline void rnd_init(){  //uses GAME_INPUTs (UP,DOWN,X)
    uint8_t i = 0, buffer = 0, status = 0;
    char* msgBuffer = "/=-Game setup-=\\";

    lcd_clear();
    lcd_print(msgBuffer, 16);
    alc_array_delete(msgBuffer);
    lcd_set_cursor(1,0);

    rnd_seed = 0;
    while(i++ < RND_INPUT_MAX){
        if(kpd_is_key_pressed_indexed(GAME_INPUT_UP)){
            rnd_seed += buffer;
            buffer = 0;
            ++status;
        } else if(kpd_is_key_pressed_indexed(GAME_INPUT_DOWN)){
            rnd_seed -= buffer;
            buffer = 0;
            status = 0;
        } else {
            switch(status){
                case 0:
                    ++buffer;
                break;
                default:
                    buffer += status * RND_STEP;
                break;
            }
            _delay_ms(RND_STEP);
        }

        if(kpd_is_key_pressed_indexed(GAME_INPUT_X) && i >= RND_INPUT_MIN) break;
    }
}

#endif  //RANDOM_H
