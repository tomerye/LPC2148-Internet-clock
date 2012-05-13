#include "lpc214x.h"
#include "io.h"
#include "rtc.h"
#include "vic.c"
#include "vic.h"
#include "lcd.h"
#include "alarm-watch.h"
#include "configuration.h"

int buzzer_counter=0;
int display_counter=0;
int isShow=0;

void RTC_init(char sec,char min, char hour,char dom, char month, int year)
{

	PREINT 			= 0x00000392;
	PREFRAC 		= 0x00004380;

	SEC				= sec;
	MIN				= min;
	HOUR			= hour;

	DOM				= dom;
	MONTH			= month;
	YEAR			= year;

	CIIR= BIT0;   /* Enable sec counter interrupt*/
	CCR=BIT4;
	CCR |= BIT0;		/*Start the RTC*/

	PINSEL1|=BIT0;
	EXTMODE|=BIT0;
	EXTINT|=BIT0;

/*
	IODIR0|=BIT10;
	PINSEL0|=BIT29;
	EXTMODE|=BIT1;
	EXTINT|=BIT1;
*/
	vicEnableIRQ(INT_SOURCE_RTC, 1, rtc_func);
	vicEnableIRQ(INT_SOURCE_EINT0, 0, button_intterupt);
	}

void RTC_DISABLE_ALARM(){
	AMR=0xff;
}
void RTC_SET_ALARM(char sec,char min, char hour,char dom, char month, int year)
{
	AMR=~0xcf;
	ALSEC	= sec;
	ALMIN	= min;
	ALHOUR	= hour;
	ALDOM	= dom;
	ALMON	= month;
	ALYEAR	= year;

}


void __attribute__ ((interrupt("IRQ"))) rtc_func(void) {
	vicUpdatePriority();
	if (ILR&AMR_INTERRUPT) {  /*AMR interupt*/
		handleALARM();
		ILR = BIT1;       /* clear the interrupt flag    */
	}
	if (ILR&CIIR_INTERRUPT) { /*every sec interrupt*/
		if(display_counter==0)
			handleUPDATETIME();
		else{
			if(!isShow){
				PrintNextAlarm();
				isShow=1;
			}
			else
				display_counter--;
		}
		if(buzzer_counter==0)
			IOPIN0|=BIT7;
		else
			buzzer_counter--;

		ILR = BIT0;       /* clear the interrupt flag    */
	}
}

void PrintNextAlarm(){
	vicUpdatePriority();
	char StringToPrint[50];
	if(GetNumberOfSchedules())
		sprintf(StringToPrint,"next alarm:\n%d:%d %d-%d-%d",
				(int)scheduleQ[0].houre,(int)scheduleQ[0].minuts,(int)scheduleQ[0].day,
				(int)scheduleQ[0].month,scheduleQ[0].year);
	else
		sprintf(StringToPrint,"there is no\nalarm set");
	lcdClearScreen();
	lcdPrintString(StringToPrint);
}
void __attribute__ ((interrupt("IRQ"))) button_intterupt(void) {
	vicUpdatePriority();
	display_counter=4;
	isShow=0;
	IOPIN0|=BIT7;
	EXTINT|=BIT0;
}

void handleUPDATETIME(){
	char StringToPrint[20];
	sprintf(StringToPrint,"%02lu:%02lu:%02lu\n%lu-%lu-%04lu",
	    		HOUR&0x1f,MIN&0x3f,SEC&0x3f,DOM&0x1f,MONTH&0xf,YEAR&0xfff);
	lcdClearScreen();
	lcdPrintString(StringToPrint);
}

void handleALARM(){
	IODIR0|=BIT7;
	IOPIN0&=~BIT7;  //START THE BUZZER
	buzzer_counter=BUZZER_TIME;
	DeleteFirstSchedule();
	if(GetNumberOfSchedules()>1)
		RTC_SET_ALARM(0,scheduleQ[0].minuts,scheduleQ[0].houre,scheduleQ[0].day,
							scheduleQ[0].month,scheduleQ[0].year);
	else
		RTC_DISABLE_ALARM();
}


