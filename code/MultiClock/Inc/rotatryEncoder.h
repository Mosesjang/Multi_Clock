/*
 * rotatryEncoder.h
 *
 *  Created on: Jul 10, 2026
 *      Author: jangm
 */

#ifndef ROTATRYENCODER_H_
#define ROTATRYENCODER_H_


void rotatryInit();
void dirDetect();
void butttonDetect();
extern volatile int8_t  encoderDelta;
extern volatile uint8_t buttonPressed;

#endif /* ROTATRYENCODER_H_ */
