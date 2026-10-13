#include <stdio.h>
#include <stdint.h>
#include "uart.h"
#include "delay.h"

//sensor pins
#define RCC_AHB1    (*(volatile uint32_t*)0x40023830)
#define GPIOC_MODER (*(volatile uint32_t*)0x40020800)
#define GPIOC_IDR   (*(volatile uint32_t*)0x40020810)
#define GPIOC_PUPDR (*(volatile uint32_t*)0x4002080C)


volatile int8_t  encoderDelta   = 0;
volatile uint8_t buttonPressed  = 0;


void rotatryInit(){
	// clock
	RCC_AHB1 |= (1 << 2);
	// mode
	GPIOC_PUPDR &= ~(0x3F << 20); //clear
	GPIOC_PUPDR |= (0x15 << 20);  //set
	GPIOC_MODER &= ~(0x3F << 20); // clear all / also input mode

}

volatile uint8_t  prevPos = 0;

void dirDetect(){ //function runs in interrupt
	uint8_t  Pos = 0;

	if((GPIOC_IDR & (1 << 10)) == 0 && (GPIOC_IDR & (1 << 11)) == 0){
		Pos = 0;

	}
	else if((GPIOC_IDR & (1 << 11)) == 0 && (GPIOC_IDR & (1 << 10))){
		Pos = 1;
	}
	else if((GPIOC_IDR & (1 << 11)) && (GPIOC_IDR & (1 << 10))){
		Pos = 2;
	}
	else if((GPIOC_IDR & (1 << 11)) && (GPIOC_IDR & (1 << 10)) ==  0){
		Pos = 3;
	}


	//difference
	int diff = prevPos - Pos;

	//clock wise
	if((diff == -1) || (diff == 3)){
		encoderDelta++;

	}
	else if ((diff == 1) || (diff == -3)){
		encoderDelta--;

	}

	prevPos = Pos;

}

void butttonDetect(){ //function runs in interrupt
	buttonPressed = 1;

}

