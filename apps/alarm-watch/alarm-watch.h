#ifndef _ALARM_WATCH_
#define _ALARM_WATCH_

#include <stdio.h>
#include <stdlib.h>

#define NUM_OF_SHEDULES 50


typedef struct schedule_{
	int year;
	char month;
	char day;
	char houre;
	char minuts;
}schedule;

extern  schedule scheduleQ[NUM_OF_SHEDULES];
extern int numberOfSchedules;


int CompareSchedules(schedule s1,schedule s2);
schedule CreateSchedule(int year,char month, char day, char houre,char minuts);
int FindSchedule(schedule s);
char AddSchedule(schedule s);
int DeleteSchedule(schedule s);
int DeleteFirstSchedule();
schedule GetFirstSchedule();
int GetNumberOfSchedules();

#endif /*_ALARM_WATCH_*/
