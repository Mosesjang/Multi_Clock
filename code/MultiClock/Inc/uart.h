/*
 * uart.h
 *
 *  Created on: Jun 28, 2026
 *      Author: jangm
 */

#ifndef UART_H_
#define UART_H_

void usartInit();
void sendUartChar(char c);
void printNumber(uint8_t num);
void uartPuts(const char *s);

#endif /* UART_H_ */
