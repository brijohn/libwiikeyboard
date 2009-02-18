/*-------------------------------------------------------------

usbkeyboard.c -- Usb keyboard support(boot protocol)

Copyright (C) 2008, 2009
DAVY Guillaume davyg2@gmail.com
dhewg

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

#include <string.h>
#include <malloc.h>

#include <gccore.h>
#include <ogc/usb.h>

#include <wiikeyboard/usbkeyboard.h>

#define	HEAP_SIZE 4096
#define DEVLIST_MAXSIZE 8

struct ukbd {
	bool connected;
	u16 vid;
	u16 pid;

	s32 fd;
	
	u8 keyNew[8];
	u8 keyOld[8];
	u8 oldState;

	u8 leds;
	
	eventcallback cb;

	u8 configuration;
	u32 interface;
	u32 altInterface;

	u8 ep;
	u32 ep_size;
};

static s32 hId = -1;
static struct ukbd *_kbd;

//Callback when the keyboard is disconnected
static s32 _disconnect(s32 retval, void *data)
{
	_kbd->connected = false;

	if (!_kbd->cb)
		return 1;

	USBKeyboard_event ev;
	ev.type = USBKEYBOARD_DISCONNECTED;

	_kbd->cb(ev);

	return 1;
}

static void USBKeyboard_SubmitEvent(USBKeyboard_eventType type, u8 code)
{
	if (!_kbd->cb)
		return;

	USBKeyboard_event ev;
	ev.type = type;
	ev.keyCode = code;

	_kbd->cb(ev);
}

//init the ioheap
s32 USBKeyboard_Initialize(void)
{
	if (hId > 0)
		return 0;

	hId = iosCreateHeap(HEAP_SIZE);

	if (hId < 0)
		return IPC_ENOHEAP;

	return IPC_OK;
}

//Destroy the io heap
s32 USBKeyboard_Deinitialize(void)
{
	if (hId < 0)
		return -1;

	s32 retval;
	retval = iosDestroyHeap(hId);
	hId = -1;

	return retval;
}

//Search for a keyboard connected to the wii usb port
//Thanks to Sven Peter usbstorage support
s32 USBKeyboard_Open(void)
{
	u8 *buffer;
	u8 dummy;
	u8 i;
	u16 vid, pid;
	bool found = false;
	u32 iConf, iInterface, iEp;
	usb_devdesc udd;
	usb_configurationdesc *ucd;
	usb_interfacedesc *uid;
	usb_endpointdesc *ued;
	u8 conf;

	buffer = memalign(32, DEVLIST_MAXSIZE << 3);
	if(buffer == NULL)
		return -1;

	memset(buffer, 0, DEVLIST_MAXSIZE << 3);

	if (USB_GetDeviceList("/dev/usb/oh0", buffer, DEVLIST_MAXSIZE, 0, &dummy) < 0)
	{
		free(buffer);
		return -2;
	}


	if (_kbd) {
		USB_CloseDevice(&_kbd->fd);
	} else {
		_kbd = (struct ukbd *) malloc(sizeof(struct ukbd));

		if (!_kbd)
			return -1;
	}
	
	memset(_kbd, 0, sizeof(struct ukbd));

	for (i = 0; i < DEVLIST_MAXSIZE; i++)
	{
		vid = *((u16 *) (buffer + (i << 3) + 4));
		pid = *((u16 *) (buffer + (i << 3) + 6));
		
		if ((vid==0) || (pid==0))
			continue;
		
		s32 fd = 0;
		if (USB_OpenDevice("oh0", vid, pid, &fd) < 0)
			continue;

		USB_GetDescriptors(fd, &udd);
		for(iConf = 0; iConf < udd.bNumConfigurations; iConf++)
		{
			ucd = &udd.configurations[iConf];

			for(iInterface = 0; iInterface < ucd->bNumInterfaces; iInterface++)
			{
				uid = &ucd->interfaces[iInterface];

				if ((uid->bInterfaceClass == USB_CLASS_HID) &&
					(uid->bInterfaceSubClass == USB_SUBCLASS_BOOT) &&
					(uid->bInterfaceProtocol== USB_PROTOCOL_KEYBOARD))
				{
					for(iEp = 0; iEp < uid->bNumEndpoints; iEp++)
					{
						ued = &uid->endpoints[iEp];

						if (ued->bmAttributes != USB_ENPOINT_INTERRUPT)
							continue;

						if (!(ued->bEndpointAddress & USB_ENDPOINT_IN))
							continue;

						_kbd->fd = fd;
						_kbd->vid = vid;
						_kbd->pid = pid;
						_kbd->ep = ued->bEndpointAddress;
						_kbd->ep_size = ued->wMaxPacketSize;
						_kbd->configuration = ucd->bConfigurationValue;
						_kbd->interface = uid->bInterfaceNumber;
						_kbd->altInterface = uid->bAlternateSetting;

						found = true;

						break;
					}
				}

				if (found)
					break;
			}

			if (found)
				break;
		}

		USB_FreeDescriptors(&udd);

		if (found)
			break;
		else
			USB_CloseDevice(&fd);
	}

	free(buffer);

	if (!found)
		return -3;

	if (USB_GetConfiguration(_kbd->fd, &conf) < 0)
	{
		USBKeyboard_Close();
		return -4;
	}

	if (conf != _kbd->configuration &&
		USB_SetConfiguration(_kbd->fd, _kbd->configuration) < 0)
	{
		USBKeyboard_Close();
		return -5;
	}

	if (_kbd->altInterface != 0 &&
		USB_SetAlternativeInterface(_kbd->fd, _kbd->interface, _kbd->altInterface) < 0)
	{
		USBKeyboard_Close();
		return -6;
	}
	
	if (USBKeyboard_Get_Protocol() != 0)
	{
		if (USBKeyboard_Set_Protocol(0) < 0)
		{
			USBKeyboard_Close();
			return -6;
		}

		if (USBKeyboard_Get_Protocol() == 1)
		{
			USBKeyboard_Close();
			return -7;
		}
	}
	
	if (USB_DeviceRemovalNotifyAsync(_kbd->fd, &_disconnect, NULL) < 0)
	{
		USBKeyboard_Close();
		return -8;
	}

	_kbd->connected = true;

	return 1;
}

//Close the device
void USBKeyboard_Close()
{
	if (!_kbd)
		return;

	USB_CloseDevice(&_kbd->fd);

	free(_kbd);
	_kbd = NULL;

	return;
}

bool USBKeyboard_IsConnected(void) {
	if (!_kbd)
		return false;

	return _kbd->connected;
}

//Get the protocol, 0=bout protocol and 1=report protocol
s32 USBKeyboard_Get_Protocol()
{
	s32 protocol;
	u8 *buffer = 0;
	buffer = iosAlloc(hId, 1);
	if (buffer == NULL)
		return -1;
	USB_WriteCtrlMsg(_kbd->fd,USB_REQTYPE_GET,USB_REQ_GETPROTOCOL,0,0,1,buffer);
	protocol=*buffer;
	iosFree(hId, buffer);
	return protocol;
}

//Modify the protocol, 0=bout protocol and 1=report protocol
s32 USBKeyboard_Set_Protocol(u8 protocol)
{
	return USB_WriteCtrlMsg(_kbd->fd,USB_REQTYPE_SET,USB_REQ_SETPROTOCOL,protocol,0,0,0);
}

//Get an input report from interrupt pipe
s32 USBKeyboard_Get_InputReport_Intr()
{
	u8 *buffer = 0;
	buffer = iosAlloc(hId, 8);
	if (buffer == NULL)
		return -1;
	s32 ret = USB_ReadIntrMsg(_kbd->fd, _kbd->ep, 8, buffer);
	memcpy(_kbd->keyNew,buffer,8);
	iosFree(hId, buffer);
	return ret;
}

//Get an input report from control pipe
s32 USBKeyboard_Get_OutputReport_Ctrl(u8 *leds)
{
	u8 *buffer = 0;
	buffer = iosAlloc(hId, 1);
	if (buffer == NULL)
		return -1;
	s32 ret = USB_WriteCtrlMsg(_kbd->fd,USB_REQTYPE_GET,USB_REQ_GETREPORT, USB_REPTYPE_OUTPUT<<8 | 0,0,1,buffer);
	memcpy(leds,buffer,1);
	iosFree(hId, buffer);
	return ret;
}

//Set an input report to control pipe
s32 USBKeyboard_Set_OutputReport_Ctrl()
{
	u8 *buffer = 0;
	buffer = iosAlloc(hId, 1);
	if (buffer == NULL)
		return -1;
	memcpy(buffer,&_kbd->leds,1);
	s32 ret = USB_WriteCtrlMsg(_kbd->fd,USB_REQTYPE_SET,USB_REQ_SETREPORT, USB_REPTYPE_OUTPUT<<8 | 0,0,1,buffer);
	iosFree(hId, buffer);
	return ret;
}

//Scan for key presses and generate events for the callback function
s32 USBKeyboard_Scan()
{
	u8 i;
	u8 bad_message[6] = {0x01, 0x01, 0x01, 0x01, 0x01, 0x01};
	if (USBKeyboard_Get_InputReport_Intr()<0)
		return -1;

	if (memcmp(_kbd->keyNew + 2, bad_message, 6) == 0)
		return 0;

	if(_kbd->keyNew[0] != _kbd->oldState) {
		if ((_kbd->keyNew[0] & 0x02) && !(_kbd->oldState & 0x02)) {
			USBKeyboard_SubmitEvent(USBKEYBOARD_PRESSED, 0xe1);
		} else if ((_kbd->oldState & 0x02) && !(_kbd->keyNew[0] & 0x02)) {
			USBKeyboard_SubmitEvent(USBKEYBOARD_RELEASED, 0xe1);
		}

		if ((_kbd->keyNew[0] & 0x20) && !(_kbd->oldState & 0x20)) {
			USBKeyboard_SubmitEvent(USBKEYBOARD_PRESSED, 0xe5);
		} else if ((_kbd->oldState & 0x20) && !(_kbd->keyNew[0] & 0x20)) {
			USBKeyboard_SubmitEvent(USBKEYBOARD_RELEASED, 0xe5);
		}

		if ((_kbd->keyNew[0] & 0x01) && !(_kbd->oldState & 0x01)) {
			USBKeyboard_SubmitEvent(USBKEYBOARD_PRESSED, 0xe0);
		} else if ((_kbd->oldState & 0x01) && !(_kbd->keyNew[0] & 0x01)) {
			USBKeyboard_SubmitEvent(USBKEYBOARD_RELEASED, 0xe0);
		}

		if ((_kbd->keyNew[0] & 0x10) && !(_kbd->oldState & 0x10)) {
			USBKeyboard_SubmitEvent(USBKEYBOARD_PRESSED, 0xe4);
		} else if ((_kbd->oldState & 0x10) && !(_kbd->keyNew[0] & 0x10)) {
			USBKeyboard_SubmitEvent(USBKEYBOARD_RELEASED, 0xe4);
		}

		if ((_kbd->keyNew[0] & 0x04) && !(_kbd->oldState & 0x04)) {
			USBKeyboard_SubmitEvent(USBKEYBOARD_PRESSED, 0xe2);
		} else if ((_kbd->oldState & 0x04) && !(_kbd->keyNew[0] & 0x04)) {
			USBKeyboard_SubmitEvent(USBKEYBOARD_RELEASED, 0xe2);
		}

		if ((_kbd->keyNew[0] & 0x40) && !(_kbd->oldState & 0x40)) {
			USBKeyboard_SubmitEvent(USBKEYBOARD_PRESSED, 0xe6);
		} else if ((_kbd->oldState & 0x40) && !(_kbd->keyNew[0] & 0x40)) {
			USBKeyboard_SubmitEvent(USBKEYBOARD_RELEASED, 0xe6);
		}

		if ((_kbd->keyNew[0] & 0x08) && !(_kbd->oldState & 0x08)) {
			USBKeyboard_SubmitEvent(USBKEYBOARD_PRESSED, 0xe3);
		} else if ((_kbd->oldState & 0x08) && !(_kbd->keyNew[0] & 0x08)) {
			USBKeyboard_SubmitEvent(USBKEYBOARD_RELEASED, 0xe3);
		}

		if ((_kbd->keyNew[0] & 0x80) && !(_kbd->oldState & 0x80)) {
			USBKeyboard_SubmitEvent(USBKEYBOARD_PRESSED, 0xe7);
		} else if ((_kbd->oldState & 0x80) && !(_kbd->keyNew[0] & 0x80)) {
			USBKeyboard_SubmitEvent(USBKEYBOARD_RELEASED, 0xe7);
		}
	}
	for (i = 2; i < 8; i++)
	{
		if (_kbd->keyOld[i] > 3 && memchr(_kbd->keyNew + 2, _kbd->keyOld[i], 6) == NULL)
		{
			USBKeyboard_SubmitEvent(USBKEYBOARD_RELEASED, _kbd->keyOld[i]);
		}
		if (_kbd->keyNew[i] > 3 && memchr(_kbd->keyOld + 2, _kbd->keyNew[i], 6) == NULL)
		{
			USBKeyboard_SubmitEvent(USBKEYBOARD_PRESSED, _kbd->keyNew[i]);
		}
	}

	memcpy(_kbd->keyOld, _kbd->keyNew, 8);
	_kbd->oldState = _kbd->keyNew[0];

	return 0;
}

//Turn on/off a led
s32 USBKeyboard_SetLed(const USBKeyboard_led led, bool on)
{
	if (on)
		_kbd->leds = _kbd->leds | (1 << led );
	else
		_kbd->leds = _kbd->leds & (255 ^ (1 << led));

	if (USBKeyboard_Set_OutputReport_Ctrl() < 0)
		return -2;

	return 1;
}

//Toggle a led
s32 USBKeyboard_ToggleLed(const USBKeyboard_led led)
{
	_kbd->leds = _kbd->leds ^ (1 << led);

	if (USBKeyboard_Set_OutputReport_Ctrl() < 0)
		return -2;

	return 1;
}

//Set the callback function which will handle the keyboard events
void USBKeyboard_SetCB(eventcallback cb)
{
	if (!_kbd)
		return;

	_kbd->cb = cb;
}

