#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <gccore.h>
#include <wiiuse/wpad.h>

#include <libwiikeyboard/keyboard.h>

static void *xfb = NULL;
static GXRModeObj *rmode = NULL;

char string[] = "abcdefghijklmnopqrstuvxyz0123456789!:;,?./§&é\"'(-è_çà)=\n";

//---------------------------------------------------------------------------------
int main(int argc, char **argv) {
//---------------------------------------------------------------------------------

	// Initialise the video system
	VIDEO_Init();
	
	// This function initialises the attached controllers
	WPAD_Init();
	
	// Obtain the preferred video mode from the system
	// This will correspond to the settings in the Wii menu
	rmode = VIDEO_GetPreferredMode(NULL);

	// Allocate memory for the display in the uncached region
	xfb = MEM_K0_TO_K1(SYS_AllocateFramebuffer(rmode));
	
	// Initialise the console, required for printf
	console_init(xfb,20,20,rmode->fbWidth,rmode->xfbHeight,rmode->fbWidth*VI_DISPLAY_PIX_SZ);
	
	// Set up the video registers with the chosen mode
	VIDEO_Configure(rmode);
	
	// Tell the video hardware where our display memory is
	VIDEO_SetNextFramebuffer(xfb);
	
	// Make the display visible
	VIDEO_SetBlack(FALSE);

	// Flush the video register changes to the hardware
	VIDEO_Flush();

	// Wait for Video setup to complete
	VIDEO_WaitVSync();
	if(rmode->viTVMode&VI_NON_INTERLACE) VIDEO_WaitVSync();


	// The console understands VT terminal escape codes
	// This positions the cursor on row 2, column 0
	// we can use variables for this with format codes too
	// e.g. printf ("\x1b[%d;%dH", row, column );
	printf("\x1b[2;0H");
	
	int report[99];
	int num=0;

	printf("Welcome in the program test of the libwiikeyboard, you can quit by pressing the home button of the wiimote\n");
	printf("First the program will test some stuff and you will be ask to connect keyboard, disconnect it, press some keys, look at the LED...\n");
	printf("If you don't do what is ask the test will be useless so do it well ;), after you will be given a report to send to the developpers\n");
	printf("And then you will be able to do whatever you want to test my lib and report bugs : what happens and what you've done\n");
	printf("\n");

	printf("First disconnect all keyboard from the wii, you have 5 seconds to do it, if you failed just relaunch the app without keyboard connected\n");

	//Init the usb keyboard driver and his interface
	s32 ret = KEYBOARD_Init();
	if (ret<0)
	{
		printf("Error in KEYBOARD_Init() retcode : %i\n",ret);
		while (1)
		{
			WPAD_ScanPads();
			u32 pressed = WPAD_ButtonsDown(0);
			if ( pressed & WPAD_BUTTON_HOME ) exit(0);
			VIDEO_WaitVSync();
		}
		return 0;
	}
	sleep(5);
	if (ret==0)
	{
		printf("Please connect one usbkeyboard to the wii\n");
		keyboardEvent event;
		while (event.type!=KEYBOARD_CONNECTED)
		{
			WPAD_ScanPads();
			u32 pressed = WPAD_ButtonsDown(0);
			if ( pressed & WPAD_BUTTON_HOME ) exit(0);
			VIDEO_WaitVSync();
			
			KEYBOARD_ScanKeyboards();
			KEYBOARD_getEvent(&event);
		}
		report[num++]=1;
		printf("A keyboard has been connected\n");
	}else
	{
		printf("One or more usbkeyboard are connected disconnect them and relaunch the app(HOME to quit)\n");
		while (1)
		{
			WPAD_ScanPads();
			u32 pressed = WPAD_ButtonsDown(0);
			if ( pressed & WPAD_BUTTON_HOME ) exit(0);
			VIDEO_WaitVSync();
		}
		return 0;
	}
	
	printf("Try to write without the ' : '%s' \\n means return key (you can't use backspace or del, so don't make mistake ;) and to make uppercase press Shift + key, don't use capslock) ",string);
	keyboardEvent event;
	char result[99] = "";
	while (1)
	{
		WPAD_ScanPads();
		u32 pressed = WPAD_ButtonsDown(0);
		if ( pressed & WPAD_BUTTON_HOME ) exit(0);
		VIDEO_WaitVSync();
		
		KEYBOARD_ScanKeyboards();
		if (! KEYBOARD_getEvent(&event))
			continue;
		if (event.type != KEYBOARD_PRESSED)
			continue;
		//TODO
		if ((event.keysym.sym >> 8) & 0xFF == 0) {
			sprintf(result,"%s%c", result, event.keysym.sym & 0xFF);
			printf("%c", event.keysym.sym & 0xFF);
		}
		if (event.keysym.sym == KBD_return)
			break;
	}
	
	if (strcmp(result,string)!=0)
	{
		printf("The text does not correspond\n");
		int i;
		for (i=0;string[i]!=0;i++)
		{
			report[num]=1;
			if (string[i]!=result[i])
			{
				printf("Char %i is false, you type %c in stead of %c\n",i,result[i],string[i]);
				report[num]=0;
				num++;
			}
		}
	}
	printf("Now we will test the led, when you are asked a question press y to respond yes and n to respond no\n");

	printf("Press CapsLock\n");
	while (1)
	{
		WPAD_ScanPads();
		u32 pressed = WPAD_ButtonsDown(0);
		if ( pressed & WPAD_BUTTON_HOME ) exit(0);
		VIDEO_WaitVSync();
		
		KEYBOARD_ScanKeyboards();
		if (! KEYBOARD_getEvent(&event))
			continue;
		if (event.type == KEYBOARD_PRESSED)
			break;
	}
	printf("Does the corresponding led switched on ?");
	while (1)
	{
		WPAD_ScanPads();
		u32 pressed = WPAD_ButtonsDown(0);
		if ( pressed & WPAD_BUTTON_HOME ) exit(0);
		VIDEO_WaitVSync();
		
		KEYBOARD_ScanKeyboards();
		if (! KEYBOARD_getEvent(&event))
			continue;
		if (event.type == KEYBOARD_PRESSED)
			break;
	}
	if (event.keysym.sym == KBD_y)
		report[num++]=1;
	else
		report[num++]=0;
	if (KEYBOARD_getLed(KEYBOARD_LEDCAPS))
		report[num++]=1;
	else
		report[num++]=0;

	printf("Press CapsLock\n");
	while (1)
	{
		WPAD_ScanPads();
		u32 pressed = WPAD_ButtonsDown(0);
		if ( pressed & WPAD_BUTTON_HOME ) exit(0);
		VIDEO_WaitVSync();
		
		KEYBOARD_ScanKeyboards();
		if (! KEYBOARD_getEvent(&event))
			continue;
		if (event.type == KEYBOARD_PRESSED)
			break;
	}
	printf("Does the corresponding led switched off ?");
	while (1)
	{
		WPAD_ScanPads();
		u32 pressed = WPAD_ButtonsDown(0);
		if ( pressed & WPAD_BUTTON_HOME ) exit(0);
		VIDEO_WaitVSync();
		
		KEYBOARD_ScanKeyboards();
		if (! KEYBOARD_getEvent(&event))
			continue;
		if (event.type == KEYBOARD_PRESSED)
			break;
	}
	if (event.keysym.sym == KBD_y)
		report[num++]=1;
	else
		report[num++]=0;
	if (!KEYBOARD_getLed(KEYBOARD_LEDCAPS))
		report[num++]=1;
	else
		report[num++]=0;


	printf("Press NumLock\n");
	while (1)
	{
		WPAD_ScanPads();
		u32 pressed = WPAD_ButtonsDown(0);
		if ( pressed & WPAD_BUTTON_HOME ) exit(0);
		VIDEO_WaitVSync();
		
		KEYBOARD_ScanKeyboards();
		if (! KEYBOARD_getEvent(&event))
			continue;
		if (event.type == KEYBOARD_PRESSED)
			break;
	}
	printf("Does the corresponding led switched on ?");
	while (1)
	{
		WPAD_ScanPads();
		u32 pressed = WPAD_ButtonsDown(0);
		if ( pressed & WPAD_BUTTON_HOME ) exit(0);
		VIDEO_WaitVSync();
		
		KEYBOARD_ScanKeyboards();
		if (! KEYBOARD_getEvent(&event))
			continue;
		if (event.type == KEYBOARD_PRESSED)
			break;
	}
	if (event.keysym.sym == KBD_y)
		report[num++]=1;
	else
		report[num++]=0;
	if (KEYBOARD_getLed(KEYBOARD_LEDNUM))
		report[num++]=1;
	else
		report[num++]=0;

	printf("Press NumLock\n");
	while (1)
	{
		WPAD_ScanPads();
		u32 pressed = WPAD_ButtonsDown(0);
		if ( pressed & WPAD_BUTTON_HOME ) exit(0);
		VIDEO_WaitVSync();
		
		KEYBOARD_ScanKeyboards();
		if (! KEYBOARD_getEvent(&event))
			continue;
		if (event.type == KEYBOARD_PRESSED)
			break;
	}
	printf("Does the corresponding led switched off ?");
	while (1)
	{
		WPAD_ScanPads();
		u32 pressed = WPAD_ButtonsDown(0);
		if ( pressed & WPAD_BUTTON_HOME ) exit(0);
		VIDEO_WaitVSync();
		
		KEYBOARD_ScanKeyboards();
		if (! KEYBOARD_getEvent(&event))
			continue;
		if (event.type == KEYBOARD_PRESSED)
			break;
	}
	if (event.keysym.sym == KBD_y)
		report[num++]=1;
	else
		report[num++]=0;
	if (!KEYBOARD_getLed(KEYBOARD_LEDNUM))
		report[num++]=1;
	else
		report[num++]=0;


	printf("Press ScrollLock\n");
	while (1)
	{
		WPAD_ScanPads();
		u32 pressed = WPAD_ButtonsDown(0);
		if ( pressed & WPAD_BUTTON_HOME ) exit(0);
		VIDEO_WaitVSync();
		
		KEYBOARD_ScanKeyboards();
		if (! KEYBOARD_getEvent(&event))
			continue;
		if (event.type == KEYBOARD_PRESSED)
			break;
	}
	printf("Does the corresponding led switched on ?");
	while (1)
	{
		WPAD_ScanPads();
		u32 pressed = WPAD_ButtonsDown(0);
		if ( pressed & WPAD_BUTTON_HOME ) exit(0);
		VIDEO_WaitVSync();
		
		KEYBOARD_ScanKeyboards();
		if (! KEYBOARD_getEvent(&event))
			continue;
		if (event.type == KEYBOARD_PRESSED)
			break;
	}
	if (event.keysym.sym == KBD_y)
		report[num++]=1;
	else
		report[num++]=0;
	if (KEYBOARD_getLed(KEYBOARD_LEDSCROLL))
		report[num++]=1;
	else
		report[num++]=0;

	printf("Press ScrollLock\n");
	while (1)
	{
		WPAD_ScanPads();
		u32 pressed = WPAD_ButtonsDown(0);
		if ( pressed & WPAD_BUTTON_HOME ) exit(0);
		VIDEO_WaitVSync();
		
		KEYBOARD_ScanKeyboards();
		if (! KEYBOARD_getEvent(&event))
			continue;
		if (event.type == KEYBOARD_PRESSED)
			break;
	}
	printf("Does the corresponding led switched off ?");
	while (1)
	{
		WPAD_ScanPads();
		u32 pressed = WPAD_ButtonsDown(0);
		if ( pressed & WPAD_BUTTON_HOME ) exit(0);
		VIDEO_WaitVSync();
		
		KEYBOARD_ScanKeyboards();
		if (! KEYBOARD_getEvent(&event))
			continue;
		if (event.type == KEYBOARD_PRESSED)
			break;
	}
	if (event.keysym.sym == KBD_y)
		report[num++]=1;
	else
		report[num++]=0;
	if (!KEYBOARD_getLed(KEYBOARD_LEDSCROLL))
		report[num++]=1;
	else
		report[num++]=0;


	printf("Now the app will put on some led for 2 seconds and you will be ask if the led has really been on");
	
	KEYBOARD_putOffLed(KEYBOARD_LEDCAPS);
	KEYBOARD_putOffLed(KEYBOARD_LEDNUM);
	KEYBOARD_putOffLed(KEYBOARD_LEDSCROLL);
	
	printf("Putting CapsLock on ...\n");
	KEYBOARD_putOnLed(KEYBOARD_LEDCAPS);
	sleep(2);
	KEYBOARD_putOffLed(KEYBOARD_LEDCAPS);
	printf("Does the corresponding led switched on ?");
	while (1)
	{
		WPAD_ScanPads();
		u32 pressed = WPAD_ButtonsDown(0);
		if ( pressed & WPAD_BUTTON_HOME ) exit(0);
		VIDEO_WaitVSync();
		
		KEYBOARD_ScanKeyboards();
		if (! KEYBOARD_getEvent(&event))
			continue;
		if (event.type == KEYBOARD_PRESSED)
			break;
	}
	if (event.keysym.sym == KBD_y)
		report[num++]=1;
	else
		report[num++]=0;
	
	printf("Putting NumLock on ...\n");
	KEYBOARD_putOnLed(KEYBOARD_LEDNUM);
	sleep(2);
	KEYBOARD_putOffLed(KEYBOARD_LEDNUM);
	printf("Does the corresponding led switched on ?");
	while (1)
	{
		WPAD_ScanPads();
		u32 pressed = WPAD_ButtonsDown(0);
		if ( pressed & WPAD_BUTTON_HOME ) exit(0);
		VIDEO_WaitVSync();
		
		KEYBOARD_ScanKeyboards();
		if (! KEYBOARD_getEvent(&event))
			continue;
		if (event.type == KEYBOARD_PRESSED)
			break;
	}
	if (event.keysym.sym == KBD_y)
		report[num++]=1;
	else
		report[num++]=0;
	
	printf("Putting ScrollLock on ...\n");
	KEYBOARD_putOnLed(KEYBOARD_LEDSCROLL);
	sleep(2);
	KEYBOARD_putOffLed(KEYBOARD_LEDSCROLL);
	printf("Does the corresponding led switched on ?");
	while (1)
	{
		WPAD_ScanPads();
		u32 pressed = WPAD_ButtonsDown(0);
		if ( pressed & WPAD_BUTTON_HOME ) exit(0);
		VIDEO_WaitVSync();
		
		KEYBOARD_ScanKeyboards();
		if (! KEYBOARD_getEvent(&event))
			continue;
		if (event.type == KEYBOARD_PRESSED)
			break;
	}
	if (event.keysym.sym == KBD_y)
		report[num++]=1;
	else
		report[num++]=0;

	printf("The test are finished, now a report is going to be print, there is %i test, 1 mean the test manage and 0 he don't \n",num);
	printf("Please send me this report, i need only the bad test(with 0) and you can give some information about your hardware and software\n");
	printf("You can add whatever you notice, or if a bug appears you can send how it appears and what happens. In particular now you will be\n");
	printf("able to press keys and it will be print the coresponding char and you can connect or disconnect one or more usbkeyboard\n");
	printf("you will be inform by a message of this events\n");
	printf("Report :\n");
	printf("BEGIN\n");
	u8 i;
	for (i=0;i<num;i++)
		printf("%i: %i\n",i,report[i]);
	printf("END\n");

	while(1) {

		WPAD_ScanPads();
		u32 pressed = WPAD_ButtonsDown(0);
		if ( pressed & WPAD_BUTTON_HOME ) exit(0);
		VIDEO_WaitVSync();
		
		KEYBOARD_ScanKeyboards();
		if (! KEYBOARD_getEvent(&event))
			continue;
		switch (event.type)
		{
			case KEYBOARD_CONNECTED:
				printf("\nA keyboard has been connected\n");
			break;
			case KEYBOARD_DISCONNECTED:
				printf("\nA keyboard has been disconnected\n");
			break;
			case KEYBOARD_PRESSED:
				if ((event.keysym.sym >> 8) & 0xFF == 0) {
					sprintf(result,"%s%c", result, event.keysym.sym & 0xFF);
					printf("%c", event.keysym.sym & 0xFF);
				}
			break;
			case KEYBOARD_RELEASED:
				
			break;
		}
	}

	return 0;
}
