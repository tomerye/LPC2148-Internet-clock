/*
 * GCEvent.h
 */

#ifndef GCEVENT_H_
#define GCEVENT_H_

#include <string.h>
extern "C" {
#include <gcalendar.h>
#include <gcal.h>
}
#include <iostream>
#include <vector>
#include <stdlib.h>

using namespace std;

class GCEvent {

public:
	GCEvent(gcal_event_t event);
	char* getTitle();
	char* getStart_Time();
	char* getEnd_Time();
	char* getWhere();
	char* getContent();
	char* getID();
	char* getUpdateTime();
	int	getSend();
	void setSend();
	void printEvent();

private:
	char* title;
	char* start_time;
	char* end_time;
	char* where;
	char* content;
	char* id;
	char* update_time;
	int	send;
};

#endif /* GCEVENT_H_ */
