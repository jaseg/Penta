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
	if(usbOpenDevice(&penta, 0x16c0, "s@jaseg.de", 0x05dc, "Penta", "*", NULL, stdout)){ //Ugh. Double reference.
		printf("Cannot open device handle.\n");
		return 1;
	}
	usb_control_msg(penta, 0, PENTA_REQ_SET_LED, 1, 0, 0, 0, 1000);
	usleep(500);
	usb_control_msg(penta, 0, PENTA_REQ_SET_LED, 2, 0, 0, 0, 1000);
	usleep(500);
	usb_control_msg(penta, 0, PENTA_REQ_SET_LED, 3, 0, 0, 0, 1000);
	usleep(500);
	usb_control_msg(penta, 0, PENTA_REQ_SET_LED, 0, 0, 0, 0, 1000);
	usb_close(penta);
	return 0;
}


