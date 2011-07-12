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
	int state = 1;
	while(1){
		int transmitted = 0;
		char data[8];
		if((transmitted = usb_interrupt_read(penta, 1, data, sizeof(data), 0)) < 0){
			printf("Error receiving interrupt data (error code %d).\n", ec);
			//return 1;
		}
		//printf("Received interrupt data: %x\n", data[0]);
		if(data[0] == PENTA_INT_BUTTON_PRESSED){
			printf("Button pressed.\n");
			if((ec = usb_control_msg(penta, 0x40, PENTA_REQ_SET_MOTOR, state, 0, 0, 0, 1000))){
				printf("Problems with a motor control transfer (error code %d).\n", ec);
				return 1;
			}
		}else{
			printf("Position encoder triggered\n");
		}
		//sleep(3);
		/*if((ec = usb_control_msg(penta, 0x40, PENTA_REQ_SET_MOTOR, 0, 0, 0, 0, 1000))){
			printf("Problems with a motor control transfer (error code %d).\n", ec);
			return 1;
		}*/
		state++;
		state &= 0x0001;
		//sleep(3);
	}
	usb_close(penta);
	return 0;
}


