

#include "alarm-watch.h"

 schedule scheduleQ[NUM_OF_SHEDULES];
 int numberOfSchedules=0;

//return 1 if s1>s2 -1 if s1<s2 and 0 else
int CompareSchedules(schedule s1,schedule s2){
	if(s1.year<s2.year)
		return -1;
	else if(s1.year>s2.year)
		return 1;
	if(s1.month<s2.month)
		return -1;
	else if (s1.month>s2.month)
		return 1;
	if(s1.day<s2.day)
		return -1;
	else if(s1.day>s2.day)
		return 1;
	if(s1.houre<s2.houre)
		return -1;
	else if(s1.houre>s2.houre)
		return 1;
	if(s1.minuts<s2.minuts)
		return -1;
	else if(s1.minuts>s2.minuts)
		return 1;
	else return 0;
}

schedule CreateSchedule(int year,char month, char day, char houre,char minuts){
	schedule temp;
	temp.year=year;
	temp.month=month;
	temp.day=day;
	temp.houre=houre;
	temp.minuts=minuts;
	return temp;
}
//return the position of s in array or -1 else
int FindSchedule(schedule s){
	int i,res;
	for (i=0;i<NUM_OF_SHEDULES;i++){
		res=CompareSchedules(s,scheduleQ[i]);
		if (res==0)
			return i;
	}
	return -1;
}

//add new schedule return where its locate or return -1
char AddSchedule(schedule s){
	char i,k;
	int res;
	if(numberOfSchedules==NUM_OF_SHEDULES)
		return -1;
	for(i=0;i<numberOfSchedules;i++){
		res=CompareSchedules(s,scheduleQ[i]);
		if(res==0)
			return i;
		if(res==-1)
			break;
	}
	for (k=numberOfSchedules;k>i;k--)
		scheduleQ[k]=scheduleQ[k-1];
	scheduleQ[i]=s;
	numberOfSchedules++;
	return i;

}
//delete schedule s if exist
int DeleteSchedule(schedule s){
	int i=FindSchedule(s);
	int result=i;
	if(i==-1)
		return -1;
	for(;i<numberOfSchedules-1;i++)
		scheduleQ[i]=scheduleQ[i+1];
	numberOfSchedules--;
	return result;
}

int GetNumberOfSchedules(){
	return numberOfSchedules;
}
schedule GetFirstSchedule(){
	return scheduleQ[0];
}

int DeleteFirstSchedule(){
	schedule tmp=GetFirstSchedule();
	DeleteSchedule(tmp);
	return 1;
}
