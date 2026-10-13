#include <stdio.h>
#include <stdint.h>
#include "uart.h"
#include "delay.h"
#include "rotatryEncoder.h"

// RCC
#define RCC_AHB1      (*(volatile uint32_t*)0x40023830)
#define RCC_APB1      (*(volatile uint32_t*)0x40023840)

// GPIOB
#define GPIOB_MODER   (*(volatile uint32_t*)0x40020400)
#define GPIOB_OTYPER  (*(volatile uint32_t*)0x40020404)
#define GPIOB_OSPEEDR (*(volatile uint32_t*)0x40020408)
#define GPIOB_PUPDR   (*(volatile uint32_t*)0x4002040C)
#define GPIOB_AFRH    (*(volatile uint32_t*)0x40020424)

// I2C1
#define I2C1_CR1      (*(volatile uint32_t*)0x40005400)
#define I2C1_CR2      (*(volatile uint32_t*)0x40005404)
#define I2C_DR        (*(volatile uint32_t*)0x40005410)
#define I2S1          (*(volatile uint32_t*)0x40005414)
#define I2S2          (*(volatile uint32_t*)0x40005418)
#define I2C1_CCR      (*(volatile uint32_t*)0x4000541C)
#define I2C_TRISE     (*(volatile uint32_t*)0x40005420)

void rtcInit(){
    // 1. Clocks
    RCC_AHB1 |= (1 << 1);    // GPIOB clock
    RCC_APB1 |= (1 << 21);   // I2C1 clock

    // 2. Force I2C reset (clean state)
    I2C1_CR1 |= (1 << 15);
    I2C1_CR1 &= ~(1 << 15);

    // 3. GPIO config (PB8 = SCL, PB9 = SDA)
    GPIOB_MODER &= ~(0xF << 16);
    GPIOB_MODER |= (2 << 16);    // PB8 = AF
    GPIOB_MODER |= (2 << 18);    // PB9 = AF

    GPIOB_OSPEEDR |= (3 << 16) | (3 << 18);
    GPIOB_OTYPER |= (3 << 8);
    GPIOB_PUPDR |= (1 << 16) | (1 << 18);

    GPIOB_AFRH &= ~(0xFF << 0);
    GPIOB_AFRH |= (4 << 0);      // PB8 = AF4
    GPIOB_AFRH |= (4 << 4);      // PB9 = AF4

    // 4. I2C timing config
    I2C1_CR2 |= 16;
    I2C1_CCR |= 80;
    I2C_TRISE |= 17;

    // 5. Enable peripheral LAST
    I2C1_CR1 |= (1 << 0);
}

int i2cWrite(uint8_t timAddress){
    I2C1_CR1 |= (1 << 8);              // START
    while(!(I2S1 & (1 << 0)));         // wait SB

    I2C_DR = (0x68 << 1) | 0;          // address + write
    while(!(I2S1 & (1 << 1)));         // wait ADDR

    volatile uint32_t temp = I2S1;	   // Clearing address.
    temp = I2S2;

    I2C_DR = timAddress;                 // register: seconds
    while(!(I2S1 & (1 << 7)));          // wait TXE

    I2C_DR = 0x00;                      // data: 1 seconds
    while(!(I2S1 & (1 << 7)));          // wait TXE

    I2C1_CR1 |= (1 << 9);               // STOP
}

int i2cRead(uint8_t timAddress){
    I2C1_CR1 |= (1 << 8);
    while(!(I2S1 & (1 << 0)));

    I2C_DR = (0x68 << 1) | 0;
    while(!(I2S1 & (1 << 1)));

    volatile uint32_t temp = I2S1;
    temp = I2S2;

    I2C_DR = timAddress;
    while(!(I2S1 & (1 << 7)));

    I2C1_CR1 |= (1 << 8);
    while(!(I2S1 & (1 << 0)));

    I2C_DR = (0x68 << 1) | 1;
    while(!(I2S1 & (1 << 1)));

    I2C1_CR1 &= ~(1 << 10);
    temp = I2S1;
    temp = I2S2;

    I2C1_CR1 |= (1 << 9);

    while(!(I2S1 & (1 << 6)));
    uint8_t seconds = I2C_DR;

    return seconds;
}

int bcdToDec(uint8_t bcd){
	uint8_t tens = bcd >> 4;
	uint8_t ones = bcd & 0x0F;
	return  (tens * 10) + ones;
}

void printTime(){

		 printNumber(bcdToDec(i2cRead(0x02)));
		 sendUartChar(':');
		 printNumber(bcdToDec(i2cRead(0x01)));
		 sendUartChar(':');
		 printNumber(bcdToDec(i2cRead(0x00)));
		 sendUartChar('\r');
	     sendUartChar('\n'); // newline for readability

	     delay(1000);

	}



