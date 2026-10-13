/*
 * menu.c
 *
 *  Created on: Oct 6, 2026
 *      Author: j
 */
#include <stdint.h>
#include "menu.h"
#include "uart.h"
#include "delay.h"
#include "rtc.h"

static void printScreen(void);
static void screenMenu(event_t ev);

static screen_t currentScreen;
static uint8_t  selection = 0;

static const char *menuNames[]   = { "Clock", "Alarm", "Pomodoro" };
static const char *screenNames[] = { "MENU", "CLOCK", "ALARM", "POMODORO"};
static const char *clockMenuNames[] = {"Display clock", "Set clock", "Back"};
//static const char *clockScreenNames[] = { "TIME", "SET", "BACK"};


static void printScreen(void){
	switch(currentScreen){
	case SCR_MENU: 		uartPuts("Menu > ");
						uartPuts(menuNames[selection]);
	break;
	case SCR_CLOCK: 	uartPuts("Clock > ");
						uartPuts(clockMenuNames[selection]);
	break;
	}
		uartPuts("\r\n");
}

static void screenMenu(event_t ev){
	switch(ev){
				case EV_CW: selection = (selection + 1) % 3;
				break;
				case EV_CCW: selection = (selection + 2) % 3;
				break;
				case EV_CLICK: currentScreen = (screen_t)(SCR_CLOCK + selection);
							   selection = 0;
				break;
				default: return;
	}
	printScreen();
}

static void clockMenu(event_t ev){
	switch(ev){
	case EV_CW: selection = (selection + 1) % 3;
    break;
	case EV_CCW: selection = (selection + 2) % 3;
	break;
	case EV_CLICK: currentScreen = selection;
					if(currentScreen == 2){ // if we click back it goes to menu
						currentScreen = SCR_MENU;
					}
					else if(currentScreen == 0){
						currentScreen = SCR_DISPLAY_CLOCK;
					}
				   selection = 0;
	break;
	default: return;
	}
	printScreen();
}
static void screenDisplayClock(event_t ev){
	switch(ev){
	case EV_NONE: printTime();
	break;
	case EV_CLICK: currentScreen = SCR_CLOCK;
				   printScreen();
	break;

	}
}

static void screenStub(event_t ev)
{
    if (ev == EV_CLICK) {
        currentScreen = SCR_MENU;
        selection = 0;
        printScreen();
    }
}


void menuUpdate(event_t ev)
{
    switch (currentScreen) {
    case SCR_MENU: screenMenu(ev); break;
    case SCR_CLOCK: clockMenu(ev); break;
    case SCR_DISPLAY_CLOCK: screenDisplayClock(ev); break;
    default:       screenStub(ev); break;
    }
}

void menuInit(){
		printScreen();
}

