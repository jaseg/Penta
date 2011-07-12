/* 	Penta host implementation
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

#include <unistd.h>
#include <sys/time.h>

#include "opendevice.h"
#include "../requests.h" //TODO relative path: not so nice.

int main(int argc, char** argv)
{
	usb_dev_handle* penta;
	usb_init();
	int ec = 0;
	if((ec = usbOpenDevice(&penta, 0x16c0, "s@jaseg.de", 0x05dc, "Penta", "*", 0, stdout))){ //Ugh. Double reference.
		printf("Cannot open device handle (error code %d).\n", ec);
		return 2;
	}
	int state = 0;
	unsigned long int last_position = 0UL;
	unsigned long int start_time = 0UL;
	double last_speed = 0.0;
	while(1){
		int transmitted = 0;
		char data[8];
		if((transmitted = usb_interrupt_read(penta, 1, data, sizeof(data), 0)) < 0){
			printf("Error receiving interrupt data (error code %d).\n", ec);
			//return 1;
		}
		//printf("Received interrupt data: %x\n", data[0]);
		if(data[0] == PENTA_INT_BUTTON_PRESSED){
			//printf("Button pressed.\n");
			state++;
			state &= 0x0001;
			if(state){
				//(presumably) starting the motor
				last_position = 0UL;
				last_speed = 0.0;
				struct timeval start_time_tv;
				gettimeofday(&start_time_tv, 0);
				start_time = start_time_tv.tv_sec*1000000UL+start_time_tv.tv_usec;
				printf("Started at %lu\n", start_time);
				if((ec = usb_control_msg(penta, 0x40, PENTA_REQ_SET_MOTOR, state, 0, 0, 0, 1000))){
					printf("Problems with a motor control transfer (error code %d).\n", ec);
					return 1;
				}
			}
		}else{
			//printf("Position encoder triggered\n");
			struct timeval new_position_tv;
			gettimeofday(&new_position_tv, 0);
			unsigned long int new_position = new_position_tv.tv_sec*1000000UL+new_position_tv.tv_usec;
			if(last_position != 0){
				double speed = 1.0/(new_position-last_position);
				printf("Speed at %lu: %f\n", new_position, speed);
				if(last_speed != 0.0){
					double accel = (speed-last_speed)/(new_position-last_position);
					double mass_factor = 1.0/accel;
					printf("\tAcceleration: %f\n", accel);
					printf("\tMass factor: %f\n", mass_factor);
				}
				last_speed = speed;
			}
			last_position = new_position;
			//Kludge ahead.
			if((ec = usb_control_msg(penta, 0x40, PENTA_REQ_SET_MOTOR, state, 0, 0, 0, 1000))){
				printf("Problems with a motor control transfer (error code %d).\n", ec);
				return 1;
			}
		}
		//sleep(3);
		/*if((ec = usb_control_msg(penta, 0x40, PENTA_REQ_SET_MOTOR, 0, 0, 0, 0, 1000))){
			printf("Problems with a motor control transfer (error code %d).\n", ec);
			return 1;
		}*/
		//sleep(3);
	}
	usb_close(penta);
	return 0;
}


