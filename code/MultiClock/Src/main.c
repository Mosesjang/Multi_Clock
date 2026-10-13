
#include <stdint.h>
#include <stdio.h>
#include "rtc.h"
#include "uart.h"
#include "delay.h"
#include "interrupt.h"
#include "rotatryEncoder.h"
#include "display.h"
#include "menu.h"

#define RCC_AHB1 (*(volatile uint32_t*)0x40023830)
#define GPIOA_MODER (*(volatile uint32_t*)0x40020000)
#define GPIOA_ODR (*(volatile uint32_t*)0x40020014)


event_t getEvent(void);



int main(void){

timInit();
rtcInit();
i2cWrite(0x02);
i2cWrite(0x01);
i2cWrite(0x00);
usartInit();
rotatryInit();
interruptInit();
menuInit();

//spiInit();
//displayInit();
//delay(50);
//writeCommand(0x23);   // all pixels ON
//sendUartChar('D');   // checkpoint 4 — displayInit fully returned?

while(1){

	menuUpdate(getEvent());



//     screen /dev/tty.usbmodem* 9600

}


}



event_t getEvent(void){
	delay(50);
	if (buttonPressed){
		buttonPressed = 0;
		return EV_CLICK;
	}
	else if(encoderDelta >= 2){
		encoderDelta-=2;
		return EV_CW;
	}
	else if (encoderDelta <= -2){
		encoderDelta+=2;
		return EV_CCW;
	}
	else{
		return EV_NONE;
	}
}
