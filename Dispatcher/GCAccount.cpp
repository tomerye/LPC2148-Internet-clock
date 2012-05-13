/*
 * GCAccount.cpp
 */

#ifndef GCACCOUNT_CPP_
#define GCACCOUNT_CPP_

#include <sys/time.h>
#include <time.h>
#include <stdlib.h>
#include <unistd.h>
extern "C" {
#include <gcalendar.h>
#include <gcontact.h>
#include <internal_gcal.h>
#include <gcal.h>
}
#include <string.h>
#include <vector>

#include <stdio.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <netdb.h>
#include <fcntl.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <wordexp.h>

#include "GCEvent.h"
#include "GCAccount.h"

using namespace std;

extern "C" gcal_t gcal_new(gservice mode);

int get_timestamp_UTC(char *timestamp, size_t length,int tmit);
int get_timestamp_UTC_with_offset(char *timestamp, size_t length, const char* offset,int tmit);
int recvall(int hRecvSocket,char* buf,int nBytesToRecieve);
int sendall(int hClientSocket,char* buf,int nBytesToSend);
int send_packet(char* sendSTR,char* host,char* port);
int connect_to_mcu(char* host,char* port);
int ntpdate();


/*
*the number of events that we can send to the MCU
*/
#define ntpServerListSize 6


static const char *ntpServerList[ntpServerListSize] =
 {      "129.6.15.28",         
        "129.6.15.29",         
        "132.163.4.101",       
        "132.163.4.102",       
        "132.163.4.103",       
        "128.138.140.44"};

 
/*
*connect to ntp server and return the number of seceonds from 1/1/1900, if cant connect to any of the server from the ntpServerList  return -1;
*/
int ntpdate() {
	
	int	portno=123;		//NTP is port 123
	int	maxlen=1024;		//check our buffers
	int	i;			// misc var i
	socklen_t server_length;
	unsigned char msg[48]={
            0x1B,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
            0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 }; // the packet we send
	unsigned int  buf[maxlen];	// the buffer we get back
	//struct in_addr ipaddr;		//	
	struct protoent *proto;		//
	struct sockaddr_in server_addr;
	int	s;	// socket
	int	tmit;	// the time -- This is a time_t sort of
	int size;
	const char* hostname;
	
	
	proto=getprotobyname("udp");
	s=socket(PF_INET, SOCK_DGRAM, proto->p_proto);

	
	memset( &server_addr, 0, sizeof( server_addr ));
	server_addr.sin_family=AF_INET;
	
	server_addr.sin_port=htons(portno);
	
	int flags = fcntl(s, F_GETFL);
	flags |= O_NONBLOCK;
	fcntl(s, F_SETFL, flags);
	server_length = sizeof(server_addr);
	
	for(i=0;i<ntpServerListSize;i++) {
		hostname=ntpServerList[i];
		server_addr.sin_addr.s_addr = inet_addr(hostname);
		size=sendto(s,msg,sizeof(msg),0,(struct sockaddr *)&server_addr,server_length);
		//printf_debug("size1:%d\n",size);	
	// get the data back
		sleep(1);
		size=recvfrom(s,buf,sizeof(buf),0,(struct sockaddr *)&server_addr,&server_length);
		//printf_debug("size2:%d\n",size);
		if (size>0) {
			printf_debug("success getting time from ntp in %d try\n",i+1); 
			break;
		}
		}
	close(s);
	if (size<0) {
		printf_debug("didnt success getting time from ntp, taking time from OS\n"); 
		return -1;
	}
	tmit=ntohl((time_t)buf[10]);
	tmit-= 2208988800U;
	return tmit;
}

/*
 * param
 * timestamp - Initialized String 
 * length - the size of the timestamp
 * tmit- number of secends from ntp server from 1/1/1900
 * 
*getting the number of secends from ntp server or the clock in the linux system and calculate the the
*time and date in UTC (Coordinated Universal Time) AND create timestamp in RFC-3339 format (year)-(month)-(dayofmonth)T(hour):(min):(sec).(milsec)Z
*/ 
int get_timestamp_UTC(char *timestamp, size_t length, int tmit)
{
	struct tm *loctime;
	time_t curtime;
	struct timeval detail_time;
	char buffer[12];
	//int tmit;

	if (!timestamp || length < TIMESTAMP_MAX_SIZE)
		return -1;
	//tmit=ntpdate();
	if (tmit==-1)
		curtime = time(NULL);
	else
		curtime= (time_t) tmit;
	loctime = gmtime(&curtime);
	gettimeofday(&detail_time, NULL);

	strftime(timestamp, length - 1, "%FT%T", loctime);
	snprintf(buffer, sizeof(buffer) - 1, ".%03d",
		 (int)detail_time.tv_usec/1000);

	strncat(timestamp, buffer, length);
	strncat(timestamp, "Z", length);

	return 0;
}

/*
 * param
 * timestamp - Initialized String 
 * length - the size of the timestamp
 * offset - the time that we want to UTC (example:israel - daylight the offset is: +03:00
 * tmit- number of secends from ntp server from 1/1/1900 
 * 
*getting the number of secends from ntp server or the clock in the linux system and adding the offset to the time (offset format: (+/-)(hour):(min)) 
*calculate the the time and date AND create timestamp in RFC-3339 format (year)-(month)-(dayofmonth)T(hour):(min):(sec).(milsec)(offset) 
*/
int get_timestamp_UTC_with_offset(char *timestamp, size_t length, const char* offset, int tmit)
{
	struct tm *loctime;
	time_t curtime;
	struct timeval detail_time;
	char buffer[12];
	int c,hour,min;
	const char* tz=offset;
	//int tmit;

	if (*tz=='+')
		c=1;
	else if (*tz=='-')
		c=-1;
		else return 0;

	if (!timestamp || length < TIMESTAMP_MAX_SIZE)
		return -1;

	hour=c*((*(tz+1)-'0')*10+*(tz+2)-'0');
	min=c*((*(tz+4)-'0')*10+*(tz+5)-'0');
	//printf("hour%d min:%d\n",hour,min);
	//tmit=ntpdate();
	if (tmit==-1)
		curtime = time(NULL);
	else
		curtime= (time_t) tmit;
	loctime = gmtime(&curtime);
	gettimeofday(&detail_time, NULL);
	loctime->tm_hour+=hour;
	loctime->tm_min+=min;
	loctime->tm_isdst = -1;

	mktime(loctime);

	strftime(timestamp, length - 1, "%FT%T", loctime);
	snprintf(buffer, sizeof(buffer) - 1, ".%03d",
		 (int)detail_time.tv_usec/1000);

	strncat(timestamp, buffer, length);
	strncat(timestamp, offset, length);

	return 0;
}

/*
* constructor:
* param:
* user2 and pass2: the user and password of the Google Calender Account
* strkey: the title of the events in Goggle Calenser that the user want to dump
* host2 and port2: the ip and the port of the board
* 
* getting the time from ntp server
* send the current time to the MCU
* getting all events and send them to the MCU
*/
GCAccount::GCAccount(char* user2,char* pass2,char* strkey2, char* host2,char* port2){
	int check;
	int tmit=-1;
	user=strdup(user2);
	pass=strdup(pass2);
	host=strdup(host2);
	port=strdup(port2);
	tmit=ntpdate();
	timestamp= (char *)malloc(TIMESTAMP_MAX_SIZE);
	get_timestamp_UTC(timestamp,TIMESTAMP_MAX_SIZE,tmit);
	//printf_debug("timestamp offset UTC:%s\n",timestamp);
	strKey=strdup(strkey2);
	//printf_debug("%s\n","begin send time");
	check=send_time(tmit);
	if (check!=1) {
		perror("send time");
		exit(1);
	}
	//printf_debug("%s\n","end send time");
	getAllEvents(tmit);
	check=sendEvents();
	if (check!=1) {
		perror("send events");
		exit(1);
	}
}

/*
*return the user of Google Calender Account
*/
char* GCAccount::getUser(){
	return user;
}

/*
*return the password Google Calender Account
*/
char* GCAccount::getPass(){
	return pass;
}

/*
* return the current NUmber Of Events in our list
*/
int GCAccount::getNumEvents(){
	return num_events;
}

/*
 * return the timestamp of the last update
 */
char* GCAccount::getTimeStamp(){
	return timestamp;
}

/*
 * param
 * tmit: number of secends from ntp server from 1/1/1900 
 * 
 * getting the current timestamp and compare it to the start timestamp of of all the events in our list to compare it and
 * delete the events that start in the pass from the list.
 */
void GCAccount::deleteOldEvents(int tmit){
	char* query_timestamp = (char *)malloc(TIMESTAMP_MAX_SIZE);
	int result,i;
	 result= get_timestamp_UTC_with_offset(query_timestamp, TIMESTAMP_MAX_SIZE, ISRAELDAYLIGHTTIME,tmit);
	if (result==-1)
		return;
	for (i=0;i<(int) events.size();i++) {
		if (strcmp(events[i]->getStart_Time(),query_timestamp)<0) {
			if (events[i]->getSend()==1)
				num_send--;
			num_events--;
			events.erase(events.begin()+i);
		}
	}
	free(query_timestamp);
}

/*
 * param
 * tmit: number of secends from ntp server from 1/1/1900 
 * 
 * getting new events from the google calender and add them to our list/
 * Getting all the changes (add/delete/Edit) that hapend in the google Account since our last connection, 
 * for each event we calculate if we need to add him to to our list or delete the event.
 */
int GCAccount::getNewEvents(int tmit){
    gcal_t gcal;
    gcal_event_t event;
    struct gcal_event_array event_array;
	int result=0,exevent=0,i=0;
	char* timestampUTC=(char *)malloc(TIMESTAMP_MAX_SIZE);
	char* timestampLOCAL=(char *)malloc(TIMESTAMP_MAX_SIZE);
	result=get_timestamp_UTC(timestampUTC,TIMESTAMP_MAX_SIZE,tmit);
	if (result==-1)
		return -1;
	get_timestamp_UTC_with_offset(timestampLOCAL, TIMESTAMP_MAX_SIZE, ISRAELDAYLIGHTTIME,tmit);
	if (!(gcal = gcal_new(GCALENDAR)))
		return -2;
	result = gcal_get_authentication(gcal, user,pass);
    if (!(result = gcal_get_updated_events(gcal, &event_array, timestamp)))
    	printf_debug("updated events: %d\n", (int)event_array.length);

    for (i = 0; i < event_array.length; ++i) {
		event = gcal_event_element(&event_array, i);
		if (!event)
			break;
		exevent=remove_event_id(gcal_event_get_id(event));
		/*
		if (exevent!=1) {
			perror("send delete");
			return -1;
		}
		*/
		if ((gcal_event_is_deleted(event)==1)||(strcmp(gcal_event_get_start(event),timestampLOCAL)<0)||(strcmp(gcal_event_get_title(event),strKey)!=0))
			continue;
		addEvent(event);
    }

	free(timestamp);
    timestamp=timestampUTC;
    gcal_cleanup_events(&event_array);
    gcal_delete(gcal);
    gcal_final_cleanup();
    free(timestampLOCAL);
    return 1;
}

/*
 * param
 * tmit: number of secends from ntp server from 1/1/1900 
 * 
 * getting all the events from the google Calender And add the new one to Our list
 */
int GCAccount::getAllEvents(int tmit){
    gcal_t gcal;
    gcal_event_t event;
    struct gcal_event_array event_array;
	int result=0,exevent=0,i=0;
	char* timestampLOCAL=(char *)malloc(TIMESTAMP_MAX_SIZE);	
	get_timestamp_UTC_with_offset(timestampLOCAL, TIMESTAMP_MAX_SIZE, ISRAELDAYLIGHTTIME,tmit);
	//printf_debug("timestamp with offsett:%s\n",timestampLOCAL);
	//char c=*(timestampLOCAL+4);
	//printf("tav: %d\n",(int) c);
	if (!(gcal = gcal_new(GCALENDAR)))
		return -2;
	result = gcal_get_authentication(gcal, user,pass);
        if (!(result = gcal_get_events(gcal, &event_array)))
                printf_debug("all events: %d\n", (int)event_array.length);

    for (i = 0; i < event_array.length; ++i) {
		event = gcal_event_element(&event_array, i);
		if (!event)
			break;
		if ((gcal_event_is_deleted(event)==1)||(strcmp(gcal_event_get_start(event),timestampLOCAL)<0)||(strcmp(gcal_event_get_title(event),strKey)!=0))
			continue;
		addEvent(event);
    }
	
    gcal_cleanup_events(&event_array);
    gcal_delete(gcal);
    gcal_final_cleanup();
    free(timestampLOCAL);
    get_timestamp_UTC(timestamp,TIMESTAMP_MAX_SIZE,tmit);
    return 1;
}

/*
 * param:
 * id: id of event
 * 
 * check if exist event with the same id, when match - remove the event from our list
 * and send delete message to to MCU return 1, if ther isnt such id in our list return -1
 */
int GCAccount::remove_event_id(char* id){
	int check=0;
	for (int i=0;i<(int) events.size();i++) {
		if (strcmp(events[i]->getID(),id)==0) {
			if (events[i]->getSend()==1) {
				num_send--;
				check=send_delete_event(events[i]);
				if (check!=1){
					return -2;
				}
			}
			events.erase(events.begin()+i);
			num_events--;
			return 1;
		}
	}
	return -1;
}

/*
 * param:
 * event
 * 
 * add the Event to The list in order of the start timr Of the Event
 */
void GCAccount::addEvent(gcal_event_t event){
	char* startEvent=gcal_event_get_start(event);
	for (int i=0;i<(int) events.size();i++) {
		if (strcmp(startEvent,events[i]->getStart_Time())<0) {
			events.insert(events.begin()+i,new GCEvent(event));
			num_events++;
			return;
		}
	}
	events.push_back(new GCEvent(event));
	num_events++;
	return;
}

/*
 * prints all the events in our list
 */
void GCAccount::printEvents() {
	int i=0;
	for (i=0;i<(int) events.size();i++) {
		printf("event: %d\ttitle:%s\tstart:%s\tid:%s\t\n",
			i,
			events[i]->getTitle(),
			events[i]->getStart_Time(),
			events[i]->getID());
	}
}

/*
 * param
 * event
 * 
 * connect to the MCU and send Delete message of The event -
 * the delete message format: "123 D startEventtimeStamp "
 */
int GCAccount::send_delete_event(GCEvent* event){
	int check=0;
	char* sendSTR= new char[200];
	strcpy(sendSTR,"123 D ");
	strcat(sendSTR,event->getStart_Time());
	strcat(sendSTR," ");
	printf_debug("Start send Delete Event %s\n",sendSTR);
	check=send_packet(sendSTR,host,port);
	if (check!=1){
		return -1;
	}
	printf_debug("End send Delete\n");
	delete[] sendSTR;
	return 1;
}

/*
 * param
 * event
 * 
 * connect to the MCU and send add new Event message of The event -
 * the add new Event message format: "123 M startEventtimeStamp DescriptionOfEvent "
 * UPDATE: we will not send desription
 */
int GCAccount::send_new_event(GCEvent* event){
	int check=0;
	char* sendSTR= new char[200];
	strcpy(sendSTR,"123 M ");
	strcat(sendSTR,event->getStart_Time());
	strcat(sendSTR," ");
	/*
	if (event->getContent()!=NULL) {
		strncat(sendSTR,event->getContent(),32);
	}
	strcat(sendSTR,"$ ");
	*/
	printf_debug("Start sending new Event :%s\n",sendSTR);
	check=send_packet(sendSTR,host,port);
	if (check!=1){
		printf_debug("%s\n","problem_with_send_event");
		return -1;
	}
	printf_debug("%s\n","End Send Event");
	delete[] sendSTR;
	return 1;
}

/*
 * param
 * tmit: number of secends from ntp server from 1/1/1900 
 * 
 * connect to the MCU and send time message of The event -
 * the time message format: "123 S CurrentTimeStamp "
 */
int GCAccount::send_time(int tmit){
	char* sendSTR= new char[200];
	int check=0;
	strcpy(sendSTR,"123 S ");
	char* timestampLOCAL=(char *)malloc(TIMESTAMP_MAX_SIZE);	
	get_timestamp_UTC_with_offset(timestampLOCAL, TIMESTAMP_MAX_SIZE, ISRAELDAYLIGHTTIME,tmit);
	strcat(sendSTR,timestampLOCAL);
	strcat(sendSTR," ");
	printf_debug("Start send Time:%s\n",sendSTR);
	check=send_packet(sendSTR,host,port);
	if (check!=1){
		printf_debug("%s\n","problem with sending");
		return -1;
	}
	printf_debug("%s\n","End Send Time");
	free(timestampLOCAL);
	delete[] sendSTR;
	return 1;
}

/*
 * check in the EventList for events that we not send and
 * if the number of Events in the MCU not full we send the event.
 */
int GCAccount::sendEvents(){
	for (int i=0;i<(int) events.size();i++) {
		if (num_send==MAX_SEND_EVENTS)
			break;
		if (events[i]->getSend()==0){
			int check=send_new_event(events[i]);
		if (check!=1){
			return -1;
		}
			events[i]->setSend();
			num_send++;
		}
	}
	return 1;
}

/*
 * sleep for SLEEP_TIME sec, getting time from ntp server, delete old events, check for changes in Google Calender Account from our
 * last connection, and send all relevant changes to the MCU
 */
int GCAccount::action(){
	int check;
	int tmit=-1;
	sleep(SLEEP_TIME);
	printf_debug("action\n");
	tmit=ntpdate();
	deleteOldEvents(tmit);
	check=getNewEvents(tmit);
	if (check!=1){
		return -1;
	printf_debug("end action\n");
	}
	check=sendEvents();
	if (check!=1){
		return -1;
	}
	return 1;	
}

/*
 * param
 * sendSTR: the String We want To Send
 * host and port: the host and port of the destination
 * 
 * send the string to the destination 
 * reciecv "ok" from the MCU (for debugging)
 * if we failed connecting to the MCU we return -3
 * if we cant complete the Sending or the recieving we return -2
 * we return 1 if the sending and the recieving complete
 * 
 */
int send_packet(char* sendSTR,char* host,char* port){
	char* recieveSTR= new char[10];
	int numbytes;
	int sockfd=connect_to_mcu(host,port);
	if ((sockfd==-1)||(sockfd==-2))
		return -3;
	if (sendall(sockfd,sendSTR,strlen(sendSTR)+1)==-1) {
		printf_debug("problem With Sending\n");
		delete[] sendSTR;
		delete[] recieveSTR;
		perror("send");
		return -2;
	}
	/*
	printf("%s\n","beign recv all");
	numbytes =recvall(sockfd,recieveSTR,3);
	printf("%d\n",numbytes);
	if((numbytes == 0)||(numbytes == -1)){
		delete[] sendSTR;
		delete[] recieveSTR;
		perror("recv");
		return -2;
	}
	printf("Recive Str:%s\n",recieveSTR);
	if (strcmp(recieveSTR,"ok\0")!=0) {
		delete[] sendSTR;
		delete[] recieveSTR;
		perror("recieve ok");
		return -1;
	}
	*/
	delete[] recieveSTR;
	close(sockfd);
	return 1;
}

/*
 * param:
 * hClientSocket: the socket
 * buf: the string we want to send
 * nBytesToSend: the size of the string we want to send
 * 
 * send the string by the socket
 */
int sendall(int hClientSocket,char* buf,int nBytesToSend) {
	int iPos=0;
	//int nSent=send(hClientSocket,&nBytesToSend,sizeof(nBytesToSend),0); //send the size we are goging to send
	//assert(nSent!=0);
	while(nBytesToSend)
	{
		//send next chunk
		int nSent=send(hClientSocket,buf+iPos,nBytesToSend,0);
		if(nSent==-1)
			return -1;
		if(nSent==0)
			return 0;
		nBytesToSend-=nSent;
		iPos+=nSent;
	}
	return 1;
}

/*
 * param:
 * hClientSocket: the socket
 * buf: Initialized String
 * nBytesToRecieve: the size of the string we want to recieve
 * 
 * recieve the string by the socket
 */
int recvall(int hRecvSocket,char* buf,int nBytesToRecieve) {   
	int nLeft= nBytesToRecieve;
	int iPos=0;
	do //loop till there are no more data
	{
		int nNumBytes=recv(hRecvSocket,buf+iPos,nLeft,0);

		//check if client closed connection
		if(!nNumBytes)
			return 0;

		if (nNumBytes==-1)
			return -1;

		//update free space and pointer to next byte
		nLeft-=nNumBytes;
		iPos+=nNumBytes;

	} while(nLeft);
	return 1;
}

/*
 * param:
 * host and port: the host and port if the destination
 * 
 * establish a connection to the detination and return the identifier of the socket
 * or -1 if we failed finding info of the client  or -2 if we faild connect to the destination
 */
int connect_to_mcu(char* host,char* port) {
	int sockfd,header;
	struct addrinfo hints, *servinfo, *p;
	int rv;
	char s[INET6_ADDRSTRLEN];
	unsigned long nonblocking = 1;
	long arg;
	memset(&hints, 0, sizeof hints);
	
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = 0;   
    hints.ai_protocol = 0;         


	
	if ((rv = getaddrinfo(host, port, &hints, &servinfo)) != 0) {
		fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
		return -1;
	}
	// loop through all the results and connect to the first we can
	for(p = servinfo; p != NULL; p = p->ai_next) {
		if ((sockfd = socket(p->ai_family, p->ai_socktype,p->ai_protocol)) == -1) {
			perror("client: socket");
			continue;
		}
		
		//arg=fcntl(sockfd, F_GETFL, NULL);
		//arg |= O_NONBLOCK; 
		//fcntl(sockfd, F_SETFL, arg);

		printf_debug("%s\n","try to connect TO mcu");
		if (connect(sockfd, p->ai_addr, p->ai_addrlen) == -1) {
			close(sockfd);
			perror("client: connect");
			close(sockfd);
			continue;
		}
		break;
	}
	if (p == NULL) {
		fprintf(stderr, "client: failed to connect\n");
		return -2;
	}
	printf_debug("%s\n","success to connect TO mcu");
	//inet_ntop(p->ai_family, get_in_addr((struct sockaddr *)p->ai_addr),s, sizeof s); //maybe not need
	//printf("client: connecting to %s\n", s);
	freeaddrinfo(servinfo); // all done with this structure
	return sockfd;
}

#endif /* GCACCOUNT_CPP_ */
