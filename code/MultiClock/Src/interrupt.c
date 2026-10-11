#include <stdio.h>
#include <stdint.h>
#include "uart.h"
#include "rotatryEncoder.h"
#include "delay.h"



//Interrupt define
#define APB2_SYSCFG   (*(volatile uint32_t*)0x40023844)
#define SYSCFG_EXTI3   (*(volatile uint32_t*)0x40013810)
#define SYSCFG_EXTI4   (*(volatile uint32_t*)0x40013814)
#define EXTI_FTSR      (*(volatile uint32_t*)0x40013C0C)
#define EXTI_RTSR      (*(volatile uint32_t*)0x40013C08)
#define EXTI_IMR       (*(volatile uint32_t*)0x40013C00)
#define EXTI_PR        (*(volatile uint32_t*)0x40013C14)
#define NVIC_ISER1     (*(volatile uint32_t*)0xE000E104)

void interruptInit(){
	//route port pin(which pin should be listending)
	APB2_SYSCFG |= (1 << 14);
	SYSCFG_EXTI3 &= ~(0xFF << 8);
	SYSCFG_EXTI3 |= (0x2 << 8);     // CLK
	SYSCFG_EXTI3 |= (0x2 << 12);    // DT
	SYSCFG_EXTI4 &= ~(0xF << 0);
	SYSCFG_EXTI4 |= (0x2 << 0);		// SW
    //change detection method.
	EXTI_FTSR |= (0x7 << 10);
	EXTI_RTSR |= (0x3 << 10);
	// open gate
	EXTI_IMR |= (0x7 << 10);
	//cpu part
	NVIC_ISER1 |= (1 << 8);

}

void EXTI15_10_IRQHandler(void){

	if(EXTI_PR & (0x3 << 10)){
	    dirDetect();
	}
	else if(EXTI_PR & (0x1 << 12)){
		butttonDetect();
	}

	EXTI_PR = (0x7 << 10);
}
