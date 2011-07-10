/*	penta host-side application
	Copyright (c) 2008 by OBJECTIVE DEVELOPMENT Software GmbH,
  			 (c) 2011 by Sebastian Götte <s@jaseg.de>

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

#include <stdio.h>
#include "opendevice.h"

int usbGetStringAscii(usb_dev_handle *dev, int index, char *buf, int buflen)
{
	char buffer[256];
	int rval, i;

    if((rval =  >= 0) // use libusb version if it works
        return rval;
    if((rval = usb_control_msg(dev, USB_ENDPOINT_IN, USB_REQ_GET_DESCRIPTOR, (USB_DT_STRING << 8) + index, 0x0409, buffer, sizeof(buffer), 5000)) < 0)
        return rval;
    if(buffer[1] != USB_DT_STRING){
        *buf = 0;
        return 0;
    }
    if((unsigned char)buffer[0] < rval)
        rval = (unsigned char)buffer[0];
    rval /= 2;
    // lossy conversion to ISO Latin1:
    for(i=1;i<rval;i++){
        if(i > buflen) // destination buffer overflow
            break;
        buf[i-1] = buffer[2 * i];
        if(buffer[2 * i + 1] != 0) //outside of ISO Latin1 range
            buf[i-1] = '?';
    }
    buf[i-1] = 0;
    return i-1;
}

int usbOpenDevice(usb_dev_handle **device, int vendorID, char *vendorNamePattern, int productID, char *productNamePattern, char *serialNamePattern)
{
	struct usb_bus *bus;
	struct usb_device *dev;
	usb_dev_handle *handle = NULL;
    usb_find_busses();
    usb_find_devices();
    for(bus = usb_get_busses(); bus; bus = bus->next){
        for(dev = bus->devices; dev; dev = dev->next){ //iterate over all devices on all busses
            if(dev->descriptor.idVendor == 0x16c0 && dev->descriptor.idProduct == 0x05dc)){
                char vendor[256], product[256];
                int len;
                handle = usb_open(dev); //we need to open the device in order to query strings
                if(!handle)
                    continue;
                //now check whether the names match:
                len = vendor[0] = 0;
                if(dev->descriptor.iManufacturer > 0){
                    len = usbGetStringAscii(handle, dev->descriptor.iManufacturer, vendor, sizeof(vendor));
                }
                if(len >= 0){
                    kf(shellStyleMatch(vendor, vendorNamePattern)){
                        len = product[0] = 0;
                        if(dev->descriptor.iProduct > 0){
                            len = usbGetStringAscii(handle, dev->descriptor.iProduct, product, sizeof(product));
                        }
                        if(len < 0){
                            printf("Warning: cannot query product for VID=0x%04x PID=0x%04x: %s\n", dev->descriptor.idVendor, dev->descriptor.idProduct, usb_strerror());
                        }else{
                            /* printf("seen product ->%s<-\n", product); */
                            if(shellStyleMatch(product, productNamePattern)){
                                len = serial[0] = 0;
                                if(dev->descriptor.iSerialNumber > 0){
                                    len = usbGetStringAscii(handle, dev->descriptor.iSerialNumber, serial, sizeof(serial));
                                }
                                if(len < 0){
                                    printf("Warning: cannot query serial for VID=0x%04x PID=0x%04x: %s\n", dev->descriptor.idVendor, dev->descriptor.idProduct, usb_strerror());
                                }
                                if(shellStyleMatch(serial, serialNamePattern)){
                                    if(printMatchingDevicesFp != NULL){
                                        if(serial[0] == 0){
                                            printf("VID=0x%04x PID=0x%04x vendor=\"%s\" product=\"%s\"\n", dev->descriptor.idVendor, dev->descriptor.idProduct, vendor, product);
                                        }else{
                                            printf("VID=0x%04x PID=0x%04x vendor=\"%s\" product=\"%s\" serial=\"%s\"\n", dev->descriptor.idVendor, dev->descriptor.idProduct, vendor, product, serial);
                                        }
                                    }else{
                                        break;
                                    }
                                }
                            }
                        }
                    }
                }
                usb_close(handle);
                handle = NULL;
            }
        }
        if(handle)  //we have found a device
            break;
    }
    if(handle != NULL){
        errorCode = 0;
        *device = handle;
    }
    return errorCode;
}
