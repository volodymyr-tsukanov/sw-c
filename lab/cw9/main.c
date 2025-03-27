/*
    simple LCD showcase for an AT91SAM7X256 microcontroller.
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
#include "pcf8833u8_lcd.h"

#define PIOA 0  //portA
#define PIOB 3 //portB
  #define JOYSTICK 8
  #define AUDIO_OUT 19
  #define LCD_BL 20
  #define SW1 24
  #define SW2 25
#define JOYSTICK_PUSH_MASK (1 << 8)
#define JOYSTICK_LEFT_MASK (1 << 7)
#define JOYSTICK_RIGHT_MASK (1 << 9)
#define JOYSTICK_UP_MASK (1 << 14)
#define JOYSTICK_DOWN_MASK (1 << 15)
#define LCD_BACKLIGHT_MASK (1 << 20)

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

// PIO
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
void PIO_register_enable(port_name_t port, int id){
  switch(port){
    case PORT_A:
      PIOA_PER |= (1 << id);
      break;
    case PORT_B:
      PIOB_PER |= (1 << id);
      break;
  }
}
void PIO_register_disable(port_name_t port, int id){
  switch(port){
    case PORT_A:
      PIOA_PDR |= (1 << id);
      break;
    case PORT_B:
      PIOB_PDR |= (1 << id);
      break;
  }
}
void PIO_register_output_enable(port_name_t port, int id){  //DDR OUT on
  switch(port){
    case PORT_A:
      PIOA_OER |= (1 << id);
      break;
    case PORT_B:
      PIOB_OER |= (1 << id);
      break;
  }
}
void PIO_register_output_disable(port_name_t port, int id){  //DDR OUT off
  switch(port){
    case PORT_A:
      PIOA_ODR |= (1 << id);
      break;
    case PORT_B:
      PIOB_ODR |= (1 << id);
      break;
  }
}
void PIO_register_output_set(port_name_t port, int id, int value_high){ //PORT
  switch(port){
    case PORT_A:
      if(value_high) PIOA_SODR = (1<<id);
      else PIOA_CODR |= (1 << id);
      break;
    case PORT_B:
      if(value_high) PIOB_SODR |= (1<<id);
      else PIOB_CODR |= (1 << id);
      break;
  }
}
unsigned int PIO_register_input_get(port_name_t port, int id){  //PIN
  switch(port){
    case PORT_A:
      return (PIOA_PDSR |= (1 << id)) == 0;
    case PORT_B:
      return (PIOB_PDSR |= (1 << id)) == 0;
  }
}


int main (void){
  PIO_enable(PORT_B); //PIOB on

  PIO_register_enable(PORT_B, AUDIO_OUT); //AUDIO on
  PIO_register_output_enable(PORT_B, AUDIO_OUT);

  // system clock to portA and B
  PMC_PCER = (1 << PIOA) | (1 << PIOB);
  // enable pins
  PIOA_PER = JOYSTICK_PUSH_MASK|JOYSTICK_LEFT_MASK|JOYSTICK_RIGHT_MASK|JOYSTICK_UP_MASK|JOYSTICK_DOWN_MASK;
  PIOA_ODR = JOYSTICK_PUSH_MASK|JOYSTICK_LEFT_MASK|JOYSTICK_RIGHT_MASK|JOYSTICK_UP_MASK|JOYSTICK_DOWN_MASK;

  PIO_register_enable(PORT_B, SW1);  //SW1 on
  PIO_register_output_disable(PORT_B, SW1);

  PIO_register_enable(PORT_B, SW2);  //SW1 on
  PIO_register_output_disable(PORT_B, SW1);

  InitLCD();  //LCD on
  Backlight(1); //backLight on
  LCDClearScreen();
  LCDSetRect(0,0, 130,130, 1,0);


  char buffer[32];
  int decimal_number = 5;
  int floating_point_number = 1.3611;
  while(1){
    if ((PIOA_PDSR & JOYSTICK_DOWN_MASK) == 0) {
      sprintf(buffer , "Volodymyr  %d %.3f", decimal_number,floating_point_number);
      LCDPutStr(buffer, 0,0, LARGE , GREEN , BLUE);
      sprintf(buffer , "        Tsukanov");
      LCDPutStr(buffer, 16,0, LARGE , GREEN , BLUE);
    }
    if ((PIOA_PDSR & JOYSTICK_LEFT_MASK) == 0) {
      LCDClearScreen();
    }
  }
}
