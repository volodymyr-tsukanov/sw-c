/*
    simple basics showcase for an AT91SAM7X256 microcontroller.
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
#include <targets/AT91SAM7X256.h>

#define PIOA 0  //portA
#define PIOB 3 //portB
  #define AUDIO_OUT 19
  #define LCD_BL 20
  #define SW1 24
  #define SW2 25

typedef enum {
  PORT_A,
  PORT_B
} port_name_t;
  
void delay(int ms) __attribute__((section(".fast")));
void delay(int ms) {
  volatile int i;
  for (i = 3000 * ms; i > 0; i--) {
    __asm__("nop");
  }
}

void PIO_enable(port_name_t port){
  switch(port){
    case PORT_A:
      PMC_PCER |= (1 << PIOA);
      break;
    case PORT_B:
      PIOB_PER |= (1 << PIOB);
      break;
  }
}
void PIO_enable_register(port_name_t port, int id){
  switch(port){
    case PORT_A:
      PIOA_PER |= (1 << id);
      break;
    case PORT_B:
      PIOB_PER |= (1 << id);
      break;
  }
}
void PIO_DDR(port_name_t port, int id){
  switch(port){
    case PORT_A:
      PIOA_OER |= (1 << id);
      break;
    case PORT_B:
      PIOB_OER |= (1 << id);
      break;
  }
}
void PIO_PORT(port_name_t port, int id, int value_high){
  switch(port){
    case PORT_A:
      if(value_high) PIOA_SODR = (1<<id);
      else PIOA_ODR |= (1 << id);
      break;
    case PORT_B:
      if(value_high) PIOB_SODR |= (1<<id);
      else PIOB_ODR |= (1 << id);
      break;
  }
}


int main (void){
  int stan_lcd = 1;

  PIO_enable(PORT_B);
  PIO_enable_register(PORT_B, LCD_BL);
  PIO_DDR(PORT_B, LCD_BL);
  PIO_PORT(PORT_B, LCD_BL, 1);

//PIO_enable_register(PORT_B, SW1);
  PIOB_ODR |= (1<<SW1);
 //PIO_enable_register(PORT_B, SW2);
  PIOB_ODR |= (1<<SW2);

  PIO_enable_register(PORT_B, AUDIO_OUT);
  PIO_DDR(PORT_B, AUDIO_OUT);

  while(1){
    if ((PIOB_ODSR & (1<<SW1)) == 0){  //is SW1 down
      stan_lcd = ~stan_lcd;
      PIO_PORT(PORT_B, LCD_BL, stan_lcd);
    }

    if ((PIOB_ODSR & (1<<SW2)) == 0){  // is SW2 down
      PIO_PORT(PORT_B, LCD_BL, 1);
    } else {  // lcd clear
      PIO_PORT(PORT_B, LCD_BL, 0);
    }
  }
}
