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
#include <avr/interrupt.h>
#include <util/delay.h>

#include "../requests.h" //TODO bad style.
#include "usbdrv.h"

#define LED_RED_DDR		DDRC
#define LED_RED_PORT	PORTC
#define LED_RED_PIN		0 //Verify.
#define LED_GREEN_DDR	DDRC
#define LED_GREEN_PORT	PORTC
#define LED_GREEN_PIN	2 //Verify.

#define SWITCH_INPUT	PIND
#define SWITCH_PIN		0

#define MOTOR_DDR		DDRD
#define MOTOR_PORT		PORTD
#define MOTOR_PIN		1

#define POSITION_PORT	PORTC
#define POSITION_INPUT	PINC
#define POSITION_PIN	5

#define DEBOUNCE_TIME	200

uint8_t old_input_state; //87654321
						 // 1 - button
						 // 2 - position sensor
uint8_t switch_wait_time;

usbMsgLen_t usbFunctionSetup(uchar data[8])
{
	usbRequest_t* rq = (usbRequest_t*) data;
	switch(rq->bRequest){
		case PENTA_REQ_SET_LED:
			LED_RED_PORT &= ~(1<<LED_RED_PIN);
			LED_GREEN_PORT &= ~(1<<LED_GREEN_PIN);
			if(rq->wValue.bytes[0]&0x01)
				LED_RED_PORT |= 1<<LED_RED_PIN;
			if(rq->wValue.bytes[0]&0x02)
				LED_GREEN_PORT |= 1<<LED_GREEN_PIN;
			break;
		case PENTA_REQ_SET_MOTOR:
			MOTOR_PORT &= ~(1<<MOTOR_PIN);
			if(rq->wValue.bytes[0])
				MOTOR_PORT |= 1<<MOTOR_PIN;
			break;
	}
	return USB_NO_MSG; //Is this correct? FIXME
}

int main(void)
{
	static uint8_t intout[1]; //Interrupt data
	//Device initialization
	LED_RED_DDR |= 1<<LED_RED_PIN;
	LED_GREEN_DDR |= 1<<LED_GREEN_PIN;
	MOTOR_DDR |= 1<<MOTOR_PIN;
	POSITION_PORT |= 1<<POSITION_PIN; //Enable internal pullup
	old_input_state = 0;
	switch_wait_time = DEBOUNCE_TIME;
	//USB Initialization
	usbInit();
	sei();
	//Infinitely eternal main loop.
	for(;;){
		//If one poll shows a change and the interrupt queue is not yet empty
		//said change will be ignored so the interrupt is sent when the queue is
		//empty *and* the active state of the input continues.
		//switch poll
		if(switch_wait_time == DEBOUNCE_TIME){
			if(!(SWITCH_INPUT & (1<<SWITCH_PIN)) && old_input_state & 0x01){ //Catches falling edges.
				//Action!
				if(usbInterruptIsReady()){
					intout[0] = PENTA_INT_BUTTON_PRESSED;
					usbSetInterrupt(intout, 1);
					old_input_state = (old_input_state & 0xFE) | ((SWITCH_INPUT & (1<<SWITCH_PIN))?1:0);
				}
				//else: do we have a problem? TODO
				switch_wait_time = 0;
			}else{
				old_input_state = (old_input_state & 0xFE) | ((SWITCH_INPUT & (1<<SWITCH_PIN))?1:0);
			}
		}else{
			switch_wait_time++;
		}
		//Position sensor poll
		if(!(POSITION_INPUT & (1<<POSITION_PIN)) && old_input_state & 0x02){
			if(usbInterruptIsReady()){
				intout[0] = PENTA_INT_POSITION_REACHED;
				usbSetInterrupt(intout, 1);
				old_input_state = (old_input_state & 0xFD) | ((POSITION_INPUT & (1<<SWITCH_PIN))?2:0);
			}
		}else{
			old_input_state = (old_input_state & 0xFD) | ((POSITION_INPUT & (1<<POSITION_PIN))?2:0);
		}
		usbPoll();
		_delay_ms(1);
	}
	return 0;
}

