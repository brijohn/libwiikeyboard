#ifndef __USBKEYBOARD_H__
#define __USBKEYBOARD_H__

#ifdef __cplusplus
   extern "C" {
#endif /* __cplusplus */

#define DEVLIST_MAXSIZE				0x08
#define CBEVENT_MAXNUM				0x08

#define USB_CLASS_HID				0x03
#define USB_SUBCLASS_BOOT			0x01
#define USB_PROTOCOL_KEYBOARD			0x01

#define USB_ENPOINT_INTERRUPT			0x03

#define USB_DT_HID				0x21
#define USB_DT_HID_SIZE				0x09
#define USB_DT_REPORT				0x22

#define USB_REQ_GETPROTOCOL			0x03
#define USB_REQ_SETPROTOCOL			0x0B
#define USB_REQ_GETREPORT			0x01
#define USB_REQ_SETREPORT			0x09

#define USB_REPTYPE_INPUT			0x01
#define USB_REPTYPE_OUTPUT			0x02
#define USB_REPTYPE_FEATURE			0x03

#define USB_REQTYPE_GET				0xA1
#define USB_REQTYPE_SET				0x21

typedef enum
{
	USBKEYBOARD_PRESSED = 0,
	USBKEYBOARD_RELEASED,
	USBKEYBOARD_DISCONNECTED

}USBKeyboard_eventType;

typedef enum
{
	USBKEYBOARD_LEDNUM=0,
	USBKEYBOARD_LEDCAPS,
	USBKEYBOARD_LEDSCROLL
}USBKeyboard_led;

typedef struct _USBKeyboard_event
{
	USBKeyboard_eventType type;
	u8 keyCode;
	u8 state;

}USBKeyboard_event;

typedef s32 (*eventcallback)(USBKeyboard_event event,void *usrdata);

typedef struct _device
{
	u16 vid,pid;
}device;

#define DEVNULL (device){0,0}

typedef struct _keyboard
{
	device dev;
	s32 fd;

	bool connect;
	
	u8 keyNew[8];
	u8 keyOld[8];
	
	u8 leds;
	
	eventcallback cb[CBEVENT_MAXNUM];
	void* cbData[CBEVENT_MAXNUM];
	u8 numCB;

	u8 configuration;
	u32 interface;
	u32 altInterface;

	u8 ep;
	u32 ep_size;

}keyboard;

bool devEqual(device dev1,device dev2);

s32 USBKeyboard_Initialize();
s32 USBKeyboard_Deinitialize();

s32 USBKeyboard_Find(device (*devs)[DEVLIST_MAXSIZE]);

s32 USBKeyboard_Open(keyboard *key,device dev);
s32 USBKeyboard_Close(keyboard *key);

s32 USBKeyboard_Get_Protocol(keyboard *key);
s32 USBKeyboard_Set_Protocol(keyboard *key, u8 protocol);

s32 USBKeyboard_Get_OutputReport_Ctrl(keyboard *key,u8 *leds);
s32 USBKeyboard_Set_OutputReport_Ctrl(keyboard *key);

s32 USBKeyboard_Get_InputReport_Intr(keyboard *key);

s32 USBKeyboard_GetState(keyboard *key);

s32 USBKeyboard_PutOnLed(keyboard *key,USBKeyboard_led led);
s32 USBKeyboard_PutOffLed(keyboard *key,USBKeyboard_led led);
s32 USBKeyboard_SwitchLed(keyboard *key,USBKeyboard_led led);

s32 USBKeyboard_Add_EventCB(keyboard* key,eventcallback cb, void* data);
s32 USBKeyboard_Remove_EventCB(keyboard* key,u8 n);

#ifdef __cplusplus
   }
#endif /* __cplusplus */

#endif /* __USBKEYBOARD_H__ */

