/*
    USART interface for an AVR ATMega32 microcontroller .
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
#ifndef USART_H
#define USART_H

#define BAUD_RATE 9600	// b/s
#define BAUDRATE_PRESCALER (((F_CPU / (BAUD_RATE * 16UL))) - 1)


/*- Resets the status register,
  - Sets the upper and lower byte of the baud rate register for data transmission speed,
  - Enables data receiving and transmitting,
  - Enables interrupts triggered by data reception,
  - Enables the 8N1 operating mode (8 data bits, no parity, 1 stop bit). */
static inline void usart_init(){
	UBRRH = (BAUDRATE_PRESCALER >> 8);	//write oldest byte
	UBRRL = BAUDRATE_PRESCALER;	//write youngest byte
	
	//UCSRA = 0x00	no change
	UCSRB = (1<<RXCIE)|(1<<RXEN)|(1<<TXEN);
	UCSRC = (1<<URSEL)|(1<<UCSZ0)|(1<<UCSZ1);
}


static inline uint8_t usart_receive_char(){
	while ((UCSRA & (1 << RXC)) == 0);	// wait till data is received
	return (UDR);	// return the byte
}

static inline void usart_transmit_char(uint8_t character){
	while (!(UCSRA & (1<<UDRE)));	// wait for empty transmit buffer
	UDR = character;
}
static inline void usart_transmit_string(uint8_t* string, uint32_t length){
	for(uint8_t ind = 0; ind < length; ind++){
		usart_transmit_char(string[ind]);
	}
}

#endif //USART_H
