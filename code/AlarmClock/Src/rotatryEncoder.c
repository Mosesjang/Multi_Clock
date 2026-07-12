#include <stdio.h>
#include <stdint.h>

#define RCC_AHB1    (*(volatile uint32_t*)0x40023830)
#define GPIOC_MODER (*(volatile uint32_t*)0x40020800)
#define GPIOC_IDR   (*(volatile uint32_t*)0x40020810)
#define GPIOC_PUPDR (*(volatile uint32_t*)0x4002080C)


void rotatryInit(){
	// clock
	RCC_AHB1 |= (1 << 3);
	// mode
	GPIOC_PUPDR |= (0x2A << 20);
	GPIOC_MODER &= ~(0x3F << 20); // clear all / also input mode

}

void detectInput(){

}
