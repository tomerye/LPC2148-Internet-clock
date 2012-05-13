/*
 * GCEvent.cpp
 */

#include <string.h>
#include <stdlib.h>
#include <stdio.h>

extern "C" {
#include <gcalendar.h>
#include <gcal.h>
}
#include "GCEvent.h"


using namespace std;

#define MAXDATASIZE 100



GCEvent::GCEvent(gcal_event_t event){

	title=strdup(gcal_event_get_title(event));
	start_time= strdup(gcal_event_get_start(event));
	end_time= strdup(gcal_event_get_end(event));
	id=strdup(gcal_event_get_id(event));
	update_time=strdup(gcal_event_get_updated(event));

	if(gcal_event_get_where(event)!=NULL)
		where=strdup(gcal_event_get_where(event));
	else where=NULL;

	if(gcal_event_get_content(event)!=NULL)
		content=strdup(gcal_event_get_content(event));
	else content=NULL;
	
	send=0;
}

int GCEvent::getSend(){
	return send;
}

void GCEvent::setSend(){
	send=1;
}

char* GCEvent::getTitle(){
	return title;
}

char* GCEvent::getStart_Time(){
	return start_time;
}

char* GCEvent::getEnd_Time(){
	return end_time;
}

char* GCEvent::getWhere(){
	return where;
}

char* GCEvent::getContent(){
	return content;
}

char* GCEvent::getID(){
	return id;
}

char* GCEvent::getUpdateTime(){
	return update_time;
}

void GCEvent::printEvent() {
			printf("event: ttitle:%s\tstart:%s\tid:%s\t\n",
			getTitle(),
			getStart_Time(),
			getID());
}
