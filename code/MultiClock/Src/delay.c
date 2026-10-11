#include <stdio.h>
#include <stdint.h>

#define RCC_APB1 (*(volatile uint32_t*)0x40023840)
#define TIM2_CR1 (*(volatile uint32_t*)0x40000000)
#define TIM2_PSC (*(volatile uint32_t*)0x40000028)
#define TIM2_ARR (*(volatile uint32_t*)0x4000002C)
#define TIM2_SR  (*(volatile uint32_t*)0x40000010)
#define TIM2_CNT (*(volatile uint32_t*)0x40000024)

void timInit(){

	RCC_APB1 |= (1 << 0);
	TIM2_PSC = 15999;
	TIM2_CR1 |= (1 << 0);

}

void delay(uint32_t tim){
	TIM2_ARR = tim - 1;
	TIM2_CNT = 0;
	TIM2_SR &= ~(1 << 0);
	while(!(TIM2_SR & (1 << 0)));
	TIM2_SR &= ~(1 << 0);


}
