/*
 * rtc.h
 *
 *  Created on: Apr 15, 2012
 *      Author: tomer
 */

#ifndef RTC_H_
#define RTC_H_

#define AMR_INTERRUPT 0x02
#define CIIR_INTERRUPT 0x01

void RTC_SET_ALARM(char sec,char min, char hour,char dom, char month, int year);
void RTC_init(char sec,char min, char hour,char dom, char month, int year);
void __attribute__ ((interrupt("IRQ"))) rtc_func(void);
void __attribute__ ((interrupt("IRQ"))) button_intterupt(void);
void handleUPDATETIME();
void handleALARM();
void RTC_DISABLE_ALARM();
void PrintNextAlarm();

#endif /* RTC_H_ */
