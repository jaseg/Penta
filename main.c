
/* 	Penta controller firmware v0.0.0.0.0.0.1\alpha0-1
	Copyright (C) 2011 by Sebastian Götte <s@jaseg.de>

	This program is free software: you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation, either version 3 of the License, or
	(at your option) any later version.

	This program is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include <avr/io.h>
#include <util/delay.h>

#include "usbconfig.h"

#define LED_RED_DDR		DDRC
#define LED_RED_PORT	PORTC
#define LED_RED_PIN		0 //Verify.
#define LED_GREEN_DDR	DDRC
#define LED_GREEN_PORT	PORTC
#define LED_GREEN_PIN	2 //Verify.

#define SWITCH_DDR		DDRD
#define SWITCH_INPUT	PIND
#define SWITCH_PIN		0

#define MOTOR_DDR		DDRD
#define MOTOR_PORT		PORTD
#define MOTOR_PIN		1

#define DEBOUNCE_TIME	200

uint8_t old_switch_state;
uint8_t switch_wait_time;

int main(void)
{
	//Device initialization
	LED_RED_DDR |= 1<<LED_RED_PIN;
	LED_GREEN_DDR |= 1<<LED_GREEN_PIN;
	MOTOR_DDR |= 1<<MOTOR_PIN;
	old_switch_state = 0;
	switch_wait_time = 0;
	//USB Initialization
	
	for(;;){
		//switch poll
		if(SWITCH_INPUT & (1<<SWITCH_PIN)){

		}else{
			
		}
		//USB poll
		_delay_ms(1);
	}
	return 0;
}

