/*
 * display.h
 *
 *  Created on: Aug 23, 2026
 *      Author: j
 */

#ifndef DISPLAY_H_
#define DISPLAY_H_

void spiInit();
void spiWrite();
void writeCommand(uint8_t cmd);
void writeData(uint8_t data1);
void writeDataArray(uint8_t *data, uint8_t len);
void displayReset();
void displayInit();
void fillScreen(uint16_t color);
void drawPixel(uint16_t x, uint16_t y, uint8_t r, uint8_t g, uint8_t b);


#endif /* DISPLAY_H_ */
