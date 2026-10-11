/*
 * rtc.h
 *
 *  Created on: Jun 24, 2026
 *      Author: jangm
 */

#ifndef RTC_H_
#define RTC_H_

void rtcInit();
void i2cScan();
int i2cWrite();
int i2cRead();
int bcdToDec(uint8_t bcd);


#endif /* RTC_H_ */
