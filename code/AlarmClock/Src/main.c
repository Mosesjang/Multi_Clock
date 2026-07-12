
#include <stdint.h>
#include <stdio.h>
#include "rtc.h"
#include "uart.h"
#include "delay.h"


int main(void){

timInit();


rtcInit();
i2cWrite(0x02);
i2cWrite(0x01);
i2cWrite(0x00);
usartInit();

while(1){

	 printNumber(bcdToDec(i2cRead(0x02)));
	 sendUartChar(':');
	 printNumber(bcdToDec(i2cRead(0x01)));
	 sendUartChar(':');
	 printNumber(bcdToDec(i2cRead(0x00)));
	 sendUartChar('\r');
     sendUartChar('\n'); // newline for readability

     delay();

}


}
