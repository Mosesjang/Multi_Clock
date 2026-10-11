/* display.c — GC9A01 240x240 round LCD, bare-metal STM32F401RE
 * PA0 = DC, PA1 = CS, PA4 = RST, PA5 = SCK (AF5), PA7 = MOSI (AF5)
 */
#include <stdint.h>
#include "delay.h"
#include "uart.h"

#define RCC_AHB1      (*(volatile uint32_t*)0x40023830)
#define RCC_APB2      (*(volatile uint32_t*)0x40023844)
#define GPIOA_MODER   (*(volatile uint32_t*)0x40020000)
#define GPIOA_OSPEEDR (*(volatile uint32_t*)0x40020008)
#define GPIOA_BSRR    (*(volatile uint32_t*)0x40020018)
#define GPIOA_AFRL    (*(volatile uint32_t*)0x40020020)
#define SPI1_CR1      (*(volatile uint32_t*)0x40013000)
#define SPI1_SR       (*(volatile uint32_t*)0x40013008)
#define SPI1_DR8      (*(volatile uint8_t *)0x4001300C)

#define SPI_TXE   (1u << 1)
#define SPI_BSY   (1u << 7)

/* BSRR: low half sets, high half resets — atomic, no read-modify-write */
#define DC_CMD()    (GPIOA_BSRR = (1u << (0 + 16)))
#define DC_DATA()   (GPIOA_BSRR = (1u <<  0))
#define CS_LOW()    (GPIOA_BSRR = (1u << (1 + 16)))
#define CS_HIGH()   (GPIOA_BSRR = (1u <<  1))
#define RST_LOW()   (GPIOA_BSRR = (1u << (4 + 16)))
#define RST_HIGH()  (GPIOA_BSRR = (1u <<  4))

void spiInit(void)
{
    RCC_AHB1 |= (0x3 << 0);       // GPIOA + GPIOB clocks
    RCC_APB2 |= (0x1 << 12);      // SPI1 clock

    GPIOA_OSPEEDR |= (0x3 << 10); // PA5 very high speed
    GPIOA_OSPEEDR |= (0x3 << 14); // PA7 very high speed

    GPIOA_MODER &= ~(0x3 << 0);   GPIOA_MODER |= (0x1 << 0);   // PA0 DC   output
    GPIOA_MODER &= ~(0x3 << 2);   GPIOA_MODER |= (0x1 << 2);   // PA1 CS   output
    GPIOA_MODER &= ~(0x3 << 8);   GPIOA_MODER |= (0x1 << 8);   // PA4 RST  output
    GPIOA_MODER &= ~(0x3 << 10);  GPIOA_MODER |= (0x2 << 10);  // PA5 SCK  AF
    GPIOA_MODER &= ~(0x3 << 14);  GPIOA_MODER |= (0x2 << 14);  // PA7 MOSI AF

    GPIOA_AFRL &= ~(0xF << 20);   GPIOA_AFRL |= (0x5 << 20);   // PA5 AF5
    GPIOA_AFRL &= ~(0xF << 28);   GPIOA_AFRL |= (0x5 << 28);   // PA7 AF5

    CS_HIGH();                    // idle levels BEFORE the reset pulse
    RST_HIGH();
    DC_DATA();

    SPI1_CR1  = 0;
    SPI1_CR1 |= (1 << 2);         // MSTR
    SPI1_CR1 |= (1 << 9);         // SSM
    SPI1_CR1 |= (1 << 8);         // SSI
    SPI1_CR1 |= (0x2 << 3);       // BR = /8
    SPI1_CR1 |= (1 << 15);        // BIDIMODE — half-duplex
    SPI1_CR1 |= (1 << 14);        // BIDIOE   — transmit
    SPI1_CR1 |= (1 << 6);         // SPE last
}

/* Push one byte. Only guarantees DR was free — NOT that the byte has left. */
static inline void spiWrite(uint8_t data)
{
    while (!(SPI1_SR & SPI_TXE));
    SPI1_DR8 = data;
}

/* THE FIX: TXE first (byte handed to the shift register, so BSY is now
 * genuinely set), then BSY clear (shift register empty). Only after this
 * is it safe to move CS or DC. */
static void spiWaitDone(void)
{
    while (!(SPI1_SR & SPI_TXE));
    while (SPI1_SR & SPI_BSY);
}

void writeCommand(uint8_t cmd)
{
    DC_CMD();
    CS_LOW();
    spiWrite(cmd);
    spiWaitDone();
    CS_HIGH();
}

void writeData(uint8_t d)
{
    DC_DATA();
    CS_LOW();
    spiWrite(d);
    spiWaitDone();
    CS_HIGH();
}

void writeDataArray(const uint8_t *data, uint32_t len)
{
    DC_DATA();
    CS_LOW();
    while (len--) spiWrite(*data++);   // stream on TXE only
    spiWaitDone();                     // one wait at the end of the burst
    CS_HIGH();
}

static void crude_delay_ms(uint32_t ms)
{
    /* Deliberately over-long. Safe at 16 MHz HSI or 84 MHz PLL.
       Only used during init — slow boot is fine, we are testing, not shipping. */
    for (uint32_t i = 0; i < ms * 30000UL; i++)
        __asm__ volatile ("nop");
}

void displayReset(void)
{
    CS_HIGH();
    RST_HIGH(); crude_delay_ms(50);
    RST_LOW();  crude_delay_ms(50);
    RST_HIGH(); crude_delay_ms(150);
}

void displayInit(void)
{
    displayReset();

    writeCommand(0xEF);
    writeCommand(0xEB); writeData(0x14);
    writeCommand(0xFE);
    writeCommand(0xEF);
    writeCommand(0xEB); writeData(0x14);
    writeCommand(0x84); writeData(0x40);
    writeCommand(0x85); writeData(0xFF);
    writeCommand(0x86); writeData(0xFF);
    writeCommand(0x87); writeData(0xFF);
    writeCommand(0x88); writeData(0x0A);
    writeCommand(0x89); writeData(0x21);
    writeCommand(0x8A); writeData(0x00);
    writeCommand(0x8B); writeData(0x80);
    writeCommand(0x8C); writeData(0x01);
    writeCommand(0x8D); writeData(0x01);
    writeCommand(0x8E); writeData(0xFF);
    writeCommand(0x8F); writeData(0xFF);

    writeCommand(0xB6); { uint8_t d[]={0x00,0x00}; writeDataArray(d,2); }
    writeCommand(0x36); writeData(0x48);
    writeCommand(0x3A); writeData(0x05);   // RGB565

    writeCommand(0x90); { uint8_t d[]={0x08,0x08,0x08,0x08}; writeDataArray(d,4); }
    writeCommand(0xBD); writeData(0x06);
    writeCommand(0xBC); writeData(0x00);
    writeCommand(0xFF); { uint8_t d[]={0x60,0x01,0x04}; writeDataArray(d,3); }
    writeCommand(0xC3); writeData(0x13);
    writeCommand(0xC4); writeData(0x13);
    writeCommand(0xC9); writeData(0x22);
    writeCommand(0xBE); writeData(0x11);
    writeCommand(0xE1); { uint8_t d[]={0x10,0x0E}; writeDataArray(d,2); }
    writeCommand(0xDF); { uint8_t d[]={0x21,0x0C,0x02}; writeDataArray(d,3); }
    writeCommand(0xF0); { uint8_t d[]={0x45,0x09,0x08,0x08,0x26,0x2A}; writeDataArray(d,6); }
    writeCommand(0xF1); { uint8_t d[]={0x43,0x70,0x72,0x36,0x37,0x6F}; writeDataArray(d,6); }
    writeCommand(0xF2); { uint8_t d[]={0x45,0x09,0x08,0x08,0x26,0x2A}; writeDataArray(d,6); }
    writeCommand(0xF3); { uint8_t d[]={0x43,0x70,0x72,0x36,0x37,0x6F}; writeDataArray(d,6); }
    writeCommand(0xED); { uint8_t d[]={0x1B,0x0B}; writeDataArray(d,2); }
    writeCommand(0xAE); writeData(0x77);
    writeCommand(0xCD); writeData(0x63);
    writeCommand(0x70); { uint8_t d[]={0x07,0x07,0x04,0x0E,0x0F,0x09,0x07,0x08,0x03}; writeDataArray(d,9); }
    writeCommand(0xE8); writeData(0x34);
    writeCommand(0x62); { uint8_t d[]={0x18,0x0D,0x71,0xED,0x70,0x70,0x18,0x0F,0x71,0xEF,0x70,0x70}; writeDataArray(d,12); }
    writeCommand(0x63); { uint8_t d[]={0x18,0x11,0x71,0xF1,0x70,0x70,0x18,0x13,0x71,0xF3,0x70,0x70}; writeDataArray(d,12); }
    writeCommand(0x64); { uint8_t d[]={0x28,0x29,0xF1,0x01,0xF1,0x00,0x07}; writeDataArray(d,7); }
    writeCommand(0x66); { uint8_t d[]={0x3C,0x00,0xCD,0x67,0x45,0x45,0x10,0x00,0x00,0x00}; writeDataArray(d,10); }
    writeCommand(0x67); { uint8_t d[]={0x00,0x3C,0x00,0x00,0x00,0x01,0x54,0x10,0x32,0x98}; writeDataArray(d,10); }
    writeCommand(0x74); { uint8_t d[]={0x10,0x85,0x80,0x00,0x00,0x4E,0x00}; writeDataArray(d,7); }
    writeCommand(0x98); { uint8_t d[]={0x3E,0x07}; writeDataArray(d,2); }

    writeCommand(0x35);
    writeCommand(0x21);
    writeCommand(0x11); crude_delay_ms(120);
    writeCommand(0x29); crude_delay_ms(20);
}

void setWindow(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1)
{
    writeCommand(0x2A);
    { uint8_t d[4] = { x0 >> 8, x0 & 0xFF, x1 >> 8, x1 & 0xFF }; writeDataArray(d, 4); }
    writeCommand(0x2B);
    { uint8_t d[4] = { y0 >> 8, y0 & 0xFF, y1 >> 8, y1 & 0xFF }; writeDataArray(d, 4); }
    writeCommand(0x2C);
}

/* RGB565 — 0xF800 red, 0x07E0 green, 0x001F blue, 0xFFFF white */
void fillScreen(uint16_t color)
{
    uint8_t hi = color >> 8;
    uint8_t lo = color & 0xFF;

    setWindow(0, 0, 239, 239);

    DC_DATA();
    CS_LOW();
    for (uint32_t i = 0; i < 240UL * 240UL; i++) {
        spiWrite(hi);
        spiWrite(lo);
    }
    spiWaitDone();
    CS_HIGH();
}

void drawPixel(uint16_t x, uint16_t y, uint16_t color)
{
    setWindow(x, y, x, y);              // was setWindow(x, x, y, y) — reversed

    DC_DATA();
    CS_LOW();
    spiWrite(color >> 8);
    spiWrite(color & 0xFF);
    spiWaitDone();
    CS_HIGH();
}
