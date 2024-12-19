/*
    simple TC timer showcase for an AT91SAM7X256 microcontroller.
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
#include <stdbool.h>

#define PIOA 0  //portA
#define PIOB 3 //portB
  #define JOYSTICK 8
  #define AUDIO_OUT 19
  #define LCD_BL 20
  #define SW1 24
  #define SW2 25
#define TC0 12
  #define TC_CLKEN 0
  #define TC_CLKDIS 1
  #define TC_SWTRG 2
  #define TC_CPCTRG 14
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
void PIO_register_enable(port_name_t port, unsigned int id){
  switch(port){
    case PORT_A:
      PIOA_PER |= (1 << id);
      break;
    case PORT_B:
      PIOB_PER |= (1 << id);
      break;
  }
}
void PIO_register_disable(port_name_t port, unsigned int id){
  switch(port){
    case PORT_A:
      PIOA_PDR |= (1 << id);
      break;
    case PORT_B:
      PIOB_PDR |= (1 << id);
      break;
  }
}
void PIO_register_output_enable(port_name_t port, unsigned int id){  //DDR OUT on
  switch(port){
    case PORT_A:
      PIOA_OER |= (1 << id);
      break;
    case PORT_B:
      PIOB_OER |= (1 << id);
      break;
  }
}
void PIO_register_output_disable(port_name_t port, unsigned int id){  //DDR OUT off
  switch(port){
    case PORT_A:
      PIOA_ODR |= (1 << id);
      break;
    case PORT_B:
      PIOB_ODR |= (1 << id);
      break;
  }
}
void PIO_register_output_set(port_name_t port, unsigned int id, bool value_high){ //PORT
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
bool PIO_register_input_get(port_name_t port, unsigned int id){  //PIN
  switch(port){
    case PORT_A:
      return (PIOA_PDSR & (1 << id)) == 0;
    case PORT_B:
      return (PIOB_PDSR & (1 << id)) == 0;
  }
}


int main (void){
  PIO_enable(PORT_B); //PIOB on

  PIO_register_enable(PORT_B,LCD_BL); //LCD on
  PIO_register_output_enable(PORT_B,LCD_BL);

  PIO_register_enable(PORT_B,SW1);  //SW1 on
  PIO_register_output_disable(PORT_B,SW1);

  PIO_register_enable(PORT_B,SW2);  //SW1 on
  PIO_register_output_disable(PORT_B,SW1);


  //ctl_global_interrupts_enable();
  ctl_unmask_isr(TC0);
  PMC_PCER = (1<<PIOB) | (1<<TC0);  //system clock to TC0
  TC0_CCR |= (1<<TC_CLKDIS); //disable TC0

  TC0_CMR |= (1<<2);  //preskaler = timer5 (dzielnik 1024)
  TC0_CMR |= (1<<TC_CPCTRG);  // RC Compare Trigger Enable

  TC0_RC = 46875; //(1024/Fcpu)*Rc

  //TC0_CCR |= ~(1<<TC_CLKDIS); //start TC0
  TC0_CCR |= (1<<TC_CLKEN);
  TC0_CCR |= (1<<TC_SWTRG);

  TC0_IER |= 1;


  while(1){
    if (PIO_register_input_get(PORT_B,SW1)){
        // Action for SW1 pressed
        PIO_register_output_set(PORT_B,LCD_BL, true);
    }
    if (PIO_register_input_get(PORT_B,SW2)){
        // Action for SW2 pressed
        PIO_register_output_set(PORT_B,LCD_BL, false);
    }

    if((TC0_SR&(1<<4)) == 0){
      PIO_register_output_set(PORT_B,LCD_BL, false);
    }
  }
}


/*
# Timer TC
## Timer Channel Control  `TC_CCR[0...2]`
• CLKEN: Counter Clock Enable Command
0 = disable
1 = enables the clock if  `CLKDIS != 1`
• CLKDIS: Counter Clock Disable Command
0 = No effect.
1 = disable the clock.
• SWTRG: Software Trigger Command
0 = No effect.
1 = A software trigger is performed: the counter is reset and the clock is started.

## TC Block Mode Register  `TC_BMR`
[32.6.2 p391]
### modes:  `TC_CMR`
- Capture Mode [32.6.4 p393]
- Waveform Mode [32.6.5 p395]

## Counter Value `TC_CV[0..2]`
CV contains the counter value in real time. *read-only*

## TC Interrupt Enable Register  `TC_IER0...2`
• COVFS: Counter Overflow
0 = No effect.
1 = Enables the Counter Overflow Interrupt.
• LOVRS: Load Overrun
0 = No effect.
1 = Enables the Load Overrun Interrupt.
• CPAS: RA Compare
0 = No effect.
1 = Enables the RA Compare Interrupt.
• CPBS: RB Compare
0 = No effect.
1 = Enables the RB Compare Interrupt.
• CPCS: RC Compare
0 = No effect.
1 = Enables the RC Compare Interrupt.
• LDRAS: RA Loading
0 = No effect.
1 = Enables the RA Load Interrupt.
• LDRBS: RB Loading
0 = No effect.
1 = Enables the RB Load Interrupt.
• ETRGS: External Trigger
0 = No effect.
1 = Enables the External Trigger Interrupt.

## TC Status Register `TC_SR[0...2]`
• COVFS: Counter Overflow Status
0 = No counter overflow has occurred since the last read of the Status Register.
1 = A counter overflow has occurred since the last read of the Status Register.
• LOVRS: Load Overrun Status
0 = Load overrun has not occurred since the last read of the Status Register or WAVE = 1.
1 = RA or RB have been loaded at least twice without any read of the corresponding register since the last read of the Status Register, if WAVE = 0.
• CPAS: RA Compare Status
0 = RA Compare has not occurred since the last read of the Status Register or WAVE = 0.
1 = RA Compare has occurred since the last read of the Status Register, if WAVE = 1.
• CPBS: RB Compare Status
0 = RB Compare has not occurred since the last read of the Status Register or WAVE = 0.
1 = RB Compare has occurred since the last read of the Status Register, if WAVE = 1.
• CPCS: RC Compare Status
0 = RC Compare has not occurred since the last read of the Status Register.
1 = RC Compare has occurred since the last read of the Status Register.
• LDRAS: RA Loading Status
0 = RA Load has not occurred since the last read of the Status Register or WAVE = 1.
1 = RA Load has occurred since the last read of the Status Register, if WAVE = 0.
• LDRBS: RB Loading Status
0 = RB Load has not occurred since the last read of the Status Register or WAVE = 1.
1 = RB Load has occurred since the last read of the Status Register, if WAVE = 0.
• ETRGS: External Trigger Status
0 = External trigger has not occurred since the last read of the Status Register.
1 = External trigger has occurred since the last read of the Status Register
• CLKSTA: Clock Enabling Status
0 = Clock is disabled.
1 = Clock is enabled.
• MTIOA: TIOA Mirror
0 = TIOA is low. If WAVE = 0, this means that TIOA pin is low. If WAVE = 1, this means that TIOA is driven low.
1 = TIOA is high. If WAVE = 0, this means that TIOA pin is high. If WAVE = 1, this means that TIOA is driven high.
• MTIOB: TIOB Mirror
0 = TIOB is low. If WAVE = 0, this means that TIOB pin is low. If WAVE = 1, this means that TIOB is driven low.
1 = TIOB is high. If WAVE = 0, this means that TIOB pin is high. If WAVE = 1, this means that TIOB is driven high.
*/
