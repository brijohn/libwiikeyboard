/*-------------------------------------------------------------

usbkeyboard.c -- Usb keyboard support(boot protocole)

Copyright (C) 2008, 2009
DAVY Guillaume davyg2@gmail.com

This software is provided 'as-is', without any express or implied
warranty.  In no event will the authors be held liable for any
damages arising from the use of this software.

Permission is granted to anyone to use this software for any
purpose, including commercial applications, and to alter it and
redistribute it freely, subject to the following restrictions:

1.	The origin of this software must not be misrepresented; you
must not claim that you wrote the original software. If you use
this software in a product, an acknowledgment in the product
documentation would be appreciated but is not required.

2.	Altered source versions must be plainly marked as such, and
must not be misrepresented as being the original software.

3.	This notice may not be removed or altered from any source
distribution.

-------------------------------------------------------------*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <malloc.h>

#include <gccore.h>
#include <ogc/usb.h>

#include <usbkeyboard.h>

#define	HEAP_SIZE 4096

static s32 hId = -1;

//Check if two device are equal
bool devEqual(device dev1,device dev2)
{
	return (dev1.vid==dev2.vid && dev1.pid==dev2.pid);
}

//Call when the keyboard is disconnected
s32 _disconnect(s32 retval,void* data)
{
	keyboard *key = (keyboard*) data;
	key->connect = false;
	USBKeyboard_event ev;
	ev.type = USBKEYBOARD_DISCONNECTED;
	u8 i;
	for (i=0;i<key->numCB;i++)
		(*key->cb[i]) (ev,key->cbData[i]);
	return 1;
}

//init the ioheap
s32 USBKeyboard_Initialize()
{
	if(hId > 0)
		return 0;

	hId = iosCreateHeap(HEAP_SIZE);

	if(hId < 0)
		return IPC_ENOHEAP;

	return IPC_OK;
}

//Destroy the io heap
s32 USBKeyboard_Deinitialize()
{
	s32 retval;
	retval = iosDestroyHeap(hId);
	hId = -1;
	return retval;
}

//Search all the keyboard connected to the wii usb port
//Thanks to Sven Peter usbstorage support
s32 USBKeyboard_Find(device (*devs)[DEVLIST_MAXSIZE])
{
	u8 *buffer;
	u8 dummy;
	u8 i;
	u16 vid, pid;

	buffer = memalign(32, DEVLIST_MAXSIZE << 3);
	if(buffer == NULL)
		return -1;
	memset(buffer, 0, DEVLIST_MAXSIZE << 3);

	if (USB_GetDeviceList("/dev/usb/oh0", buffer, DEVLIST_MAXSIZE, 0, &dummy) < 0)
	{
		free(buffer);
		return -2;
	}

	u8 num=0;
	for(i = 0; i < DEVLIST_MAXSIZE; i++)
	{
		memcpy(&vid, (buffer + (i << 3) + 4), 2);
		memcpy(&pid, (buffer + (i << 3) + 6), 2);
		
		if ((vid==0)||(pid==0))
			continue;
		
		s32 fd=0;
		if (USB_OpenDevice("oh0",vid,pid,&fd)<0)
			continue;

		u32 iConf, iInterface;
		usb_devdesc udd;
		usb_configurationdesc *ucd;
		usb_interfacedesc *uid;
		USB_GetDescriptors(fd, &udd);
		for(iConf = 0; iConf < udd.bNumConfigurations; iConf++)
		{
			ucd = &udd.configurations[iConf];		
			for(iInterface = 0; iInterface < ucd->bNumInterfaces; iInterface++)
			{
				uid = &ucd->interfaces[iInterface];
				if ( (uid->bInterfaceClass == USB_CLASS_HID) && (uid->bInterfaceSubClass == USB_SUBCLASS_BOOT) && (uid->bInterfaceProtocol== USB_PROTOCOL_KEYBOARD))
				{
					(*devs)[num].vid = vid;
					(*devs)[num].pid = pid;
					num++;
					break;
				}
			}
		}
		USB_FreeDescriptors(&udd);
		USB_CloseDevice(&fd);
	}
	return num;
}

//Open a keyboard from his pid and vid that you can have with USBKeyboard_find
s32 USBKeyboard_Open(keyboard *key,device dev)
{
	if (USB_OpenDevice("oh0",dev.vid,dev.pid,&key->fd)<0)
		return -1;

	u32 iConf, iInterface, iEp;
	usb_devdesc udd;
	usb_configurationdesc *ucd;
	usb_interfacedesc *uid;
	usb_endpointdesc *ued;

	//Search a interrupt endPoint thanks to the usb descriptor
	USB_GetDescriptors(key->fd, &udd);
	for(iConf = 0; iConf < udd.bNumConfigurations; iConf++)
	{
		ucd = &udd.configurations[iConf];
		for(iInterface = 0; iInterface < ucd->bNumInterfaces; iInterface++)
		{
			uid = &ucd->interfaces[iInterface];
			if ( (uid->bInterfaceClass == USB_CLASS_HID) && (uid->bInterfaceSubClass == USB_SUBCLASS_BOOT) && (uid->bInterfaceProtocol== USB_PROTOCOL_KEYBOARD))
			{
				for(iEp = 0; iEp < uid->bNumEndpoints; iEp++)
				{
					ued = &uid->endpoints[iEp];
					if (ued->bmAttributes != USB_ENPOINT_INTERRUPT)
 						continue;
					if (!(ued->bEndpointAddress & USB_ENDPOINT_IN))
						continue;
					key->ep = ued->bEndpointAddress;
					key->ep_size = ued->wMaxPacketSize;
					key->configuration = ucd->bConfigurationValue;
					key->interface = uid->bInterfaceNumber;
					key->altInterface = uid->bAlternateSetting;
					goto found;
				}
			}
		}
	}
	USB_FreeDescriptors(&udd);
	return -2;
	
found:
	USB_FreeDescriptors(&udd);
	
	u8 conf;
	
	if(USB_GetConfiguration(key->fd, &conf) < 0)
		return -3;
	if(conf != key->configuration && USB_SetConfiguration(key->fd, key->configuration) < 0)
		return -4;
	if(key->altInterface != 0 && USB_SetAlternativeInterface(key->fd, key->interface, key->altInterface) < 0)
		return -5;
	
	if (USBKeyboard_Get_Protocol(key)!=0)
	{
		if (USBKeyboard_Set_Protocol(key, 0)<0)
		{
			USBKeyboard_Close(key);
			return -6;
		}
		if (USBKeyboard_Get_Protocol(key)==1)
		{
			USBKeyboard_Close(key);
			return -7;
		}
	}
	
	key->leds = 0;
	key->dev = dev;
	key->numCB=0;
	key->connect = true;
	if (USB_DeviceRemovalNotifyAsync(key->fd,&_disconnect,key)<0)
	{
		USBKeyboard_Close(key);
		return -8;
	}

	return 1;
}

//Close the device
s32 USBKeyboard_Close(keyboard *key)
{
	return USB_CloseDevice(&(key->fd));
}

//Get the protocol, 0=bout protocol and 1=report protocol
s32 USBKeyboard_Get_Protocol(keyboard *key)
{
	s32 protocol;
	u8 *buffer = 0;
	buffer = iosAlloc(hId, 1);
	if (buffer == NULL)
		return -1;
	USB_WriteCtrlMsg(key->fd,USB_REQTYPE_GET,USB_REQ_GETPROTOCOL,0,0,1,buffer);
	protocol=*buffer;
	iosFree(hId, buffer);
	return protocol;
}

//Modifie the protocol, 0=bout protocol and 1=report protocol
s32 USBKeyboard_Set_Protocol(keyboard *key, u8 protocol)
{
	return USB_WriteCtrlMsg(key->fd,USB_REQTYPE_SET,USB_REQ_SETPROTOCOL,protocol,0,0,0);
}

//Get a input report from interrupt pipe
s32 USBKeyboard_Get_InputReport_Intr(keyboard *key)
{
	u8 *buffer = 0;
	buffer = iosAlloc(hId, 8);
	if (buffer == NULL)
		return -1;
	s32 ret = USB_ReadIntrMsg(key->fd,key->ep,8,buffer);
	memcpy(key->keyNew,buffer,8);
	iosFree(hId, buffer);
	return ret;
}

//Get a input report from control pipe
s32 USBKeyboard_Get_OutputReport_Ctrl(keyboard *key,u8 *leds)
{
	u8 *buffer = 0;
	buffer = iosAlloc(hId, 1);
	if (buffer == NULL)
		return -1;
	s32 ret = USB_WriteCtrlMsg(key->fd,USB_REQTYPE_GET,USB_REQ_GETREPORT, USB_REPTYPE_OUTPUT<<8 | 0,0,1,buffer);
	memcpy(leds,buffer,1);
	iosFree(hId, buffer);
	return ret;
}

//Set a input report to control pipe
s32 USBKeyboard_Set_OutputReport_Ctrl(keyboard *key)
{
	u8 *buffer = 0;
	buffer = iosAlloc(hId, 1);
	if (buffer == NULL)
		return -1;
	memcpy(buffer,&key->leds,1);
	s32 ret = USB_WriteCtrlMsg(key->fd,USB_REQTYPE_SET,USB_REQ_SETREPORT, USB_REPTYPE_OUTPUT<<8 | 0,0,1,buffer);
	iosFree(hId, buffer);
	return ret;
}

void USBKeyboard_SubmitEvent(keyboard *key, USBKeyboard_eventType type, u8 code)
{
	USBKeyboard_event ev;
	ev.type = type;
	ev.keyCode = code;
	u8 i;
	for (i=0;i<key->numCB;i++)
		(*key->cb[i]) (ev,key->cbData[i]);
}

//Get the pressed touch and call the cb fonction
s32 USBKeyboard_GetState(keyboard *key)
{
	u8 i;
	u8 bad_message[6] = {0x01, 0x01, 0x01, 0x01, 0x01, 0x01};
	if (USBKeyboard_Get_InputReport_Intr(key)<0)
		return -1;

	if (memcmp(key->keyNew + 2, bad_message, 6) == 0)
		return 0;

	if(key->keyNew[0] != key->oldState) {
		if ((key->keyNew[0] & 0x02) && !(key->oldState & 0x02)) {
			USBKeyboard_SubmitEvent(key, USBKEYBOARD_PRESSED, 0xe1);
		} else if ((key->oldState & 0x02) && !(key->keyNew[0] & 0x02)) {
			USBKeyboard_SubmitEvent(key, USBKEYBOARD_RELEASED, 0xe1);
		}

		if ((key->keyNew[0] & 0x20) && !(key->oldState & 0x20)) {
			USBKeyboard_SubmitEvent(key, USBKEYBOARD_PRESSED, 0xe5);
		} else if ((key->oldState & 0x20) && !(key->keyNew[0] & 0x20)) {
			USBKeyboard_SubmitEvent(key, USBKEYBOARD_RELEASED, 0xe5);
		}

		if ((key->keyNew[0] & 0x01) && !(key->oldState & 0x01)) {
			USBKeyboard_SubmitEvent(key, USBKEYBOARD_PRESSED, 0xe0);
		} else if ((key->oldState & 0x01) && !(key->keyNew[0] & 0x01)) {
			USBKeyboard_SubmitEvent(key, USBKEYBOARD_RELEASED, 0xe0);
		}

		if ((key->keyNew[0] & 0x10) && !(key->oldState & 0x10)) {
			USBKeyboard_SubmitEvent(key, USBKEYBOARD_PRESSED, 0xe4);
		} else if ((key->oldState & 0x10) && !(key->keyNew[0] & 0x10)) {
			USBKeyboard_SubmitEvent(key, USBKEYBOARD_RELEASED, 0xe4);
		}

		if ((key->keyNew[0] & 0x04) && !(key->oldState & 0x04)) {
			USBKeyboard_SubmitEvent(key, USBKEYBOARD_PRESSED, 0xe2);
		} else if ((key->oldState & 0x04) && !(key->keyNew[0] & 0x04)) {
			USBKeyboard_SubmitEvent(key, USBKEYBOARD_RELEASED, 0xe2);
		}

		if ((key->keyNew[0] & 0x40) && !(key->oldState & 0x40)) {
			USBKeyboard_SubmitEvent(key, USBKEYBOARD_PRESSED, 0xe6);
		} else if ((key->oldState & 0x40) && !(key->keyNew[0] & 0x40)) {
			USBKeyboard_SubmitEvent(key, USBKEYBOARD_RELEASED, 0xe6);
		}

		if ((key->keyNew[0] & 0x08) && !(key->oldState & 0x08)) {
			USBKeyboard_SubmitEvent(key, USBKEYBOARD_PRESSED, 0xe3);
		} else if ((key->oldState & 0x08) && !(key->keyNew[0] & 0x08)) {
			USBKeyboard_SubmitEvent(key, USBKEYBOARD_RELEASED, 0xe3);
		}

		if ((key->keyNew[0] & 0x80) && !(key->oldState & 0x80)) {
			USBKeyboard_SubmitEvent(key, USBKEYBOARD_PRESSED, 0xe7);
		} else if ((key->oldState & 0x80) && !(key->keyNew[0] & 0x80)) {
			USBKeyboard_SubmitEvent(key, USBKEYBOARD_RELEASED, 0xe7);
		}
	}
	for (i = 2; i < 8; i++)
	{
		if (key->keyOld[i] > 3 && memchr(key->keyNew + 2, key->keyOld[i], 6) == NULL)
		{
			USBKeyboard_SubmitEvent(key, USBKEYBOARD_RELEASED, key->keyOld[i]);
		}
		if (key->keyNew[i] > 3 && memchr(key->keyOld + 2, key->keyNew[i], 6) == NULL)
		{
			USBKeyboard_SubmitEvent(key, USBKEYBOARD_PRESSED, key->keyNew[i]);
		}
	}
	memcpy(key->keyOld, key->keyNew, 8);
	key->oldState = key->keyNew[0];
	return 0;
}

//Put on the led led
s32 USBKeyboard_PutOnLed(keyboard *key,USBKeyboard_led led)
{
	key->leds = key->leds | (1 << led );
	if (USBKeyboard_Set_OutputReport_Ctrl(key)<0)
		return -2;
	return 1;
}

//Put off the led led
s32 USBKeyboard_PutOffLed(keyboard *key,USBKeyboard_led led)
{
	key->leds = key->leds & (255 ^ (1 << led));
	if (USBKeyboard_Set_OutputReport_Ctrl(key)<0)
		return -2;
	return 1;
}

//Switch the led led, put on if off or put off if on
s32 USBKeyboard_SwitchLed(keyboard *key,USBKeyboard_led led)
{
	key->leds = key->leds ^ (1 << led);
	if (USBKeyboard_Set_OutputReport_Ctrl(key)<0)
		return -2;
	return 1;
}

//Add a callback function who will handlethe keyboard events
s32 USBKeyboard_Add_EventCB(keyboard* key,eventcallback cb, void* data)
{
	if (key->numCB==CBEVENT_MAXNUM)
		return -1;
	key->cb[key->numCB] = cb;
	key->cbData[key->numCB] = data;
	key->numCB++;
	return key->numCB;
}

//Add a callback function who will handlethe keyboard events
s32 USBKeyboard_Remove_EventCB(keyboard* key,u8 n)
{
	if (n>=CBEVENT_MAXNUM)
		return -1;
	u8 i;
	for (i=n;i<key->numCB-1;i++)
	{
		key->cb[i] = key->cb[i+1];
		key->cbData[i] = key->cbData[i+1];
	}
	key->numCB--;
	return key->numCB;
}

