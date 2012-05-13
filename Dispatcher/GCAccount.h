/*
 * GCAccount.h
 */

#ifndef GCACCOUNT_H_
#define GCACCOUNT_H_

#define DEBUG

#include <string.h>
#include <vector>
extern "C" {
#include <gcalendar.h>
#include <gcal.h>
}
#include "GCEvent.h"

using namespace std;

#define MAX_SEND_EVENTS 10
#define ISRAELDAYLIGHTTIME "+03:00"
#define	SLEEP_TIME 25

#ifdef DEBUG 
#define printf_debug printf 
#else 
#define printf_debug while(0)printf 
#endif 



extern "C" gcal_t gcal_new(gservice mode); 


class GCAccount {

public:
	GCAccount(char* user2,char* pass2,char* strkey2, char* host2,char* port2);
	char* getUser();
	char* getPass();
	int getNumEvents();
	void deleteOldEvents(int tmit);

	int getNewEvents(int tmit);
	int getAllEvents(int tmit);

	char* getTimeStamp();
	void addEvent(gcal_event_t event);
	int remove_event_id(char* id);
	void printEvents();
	
	int sendEvents();
	int action();
	
	int send_delete_event(GCEvent* event);
	int send_new_event(GCEvent* event);
	int send_time(int tmit);


private:
	char* user;
	char* pass;
	vector<GCEvent*> events;
	int num_events;
	int num_send;
	char* timestamp;
	char* strKey;
	char* host;
	char* port;
};

#endif /* GCACCOUNT_H_ */
