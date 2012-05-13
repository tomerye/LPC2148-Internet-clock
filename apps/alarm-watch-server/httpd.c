

#include "uip.h"
#include "httpd.h"
#include "lcd.h"
#include <string.h>
#include "debug.h"
#include "rtc.h"
#include "alarm-watch.h"
#include "configuration.h"

#define STATE_NO_CONNECTION 0
#define STATE_OUTPUT  1

#define ISO_nl      0x0a
#define ISO_space   0x20
#define ISO_bang    0x21
#define ISO_percent 0x25
#define ISO_period  0x2e
#define ISO_slash   0x2f
#define ISO_colon   0x3a

#define PASSWORD "123"
#define SET_CLOCK 'S'
#define SET_MEETING 'M'
#define DELETE_MEETING 'D'
#define DASH '-'
#define ISO_DOLAR_SIGN '$'


/*---------------------------------------------------------------------------*/
static PT_THREAD(handle_output(struct httpd_state *s))
{

    PT_BEGIN(&s->outputpt);
    PSOCK_SEND_STR(&s->sout, "OK\n");
    PSOCK_CLOSE(&s->sout);
    PT_END(&s->outputpt);
}
/*---------------------------------------------------------------------------*/
static PT_THREAD(handle_input(struct httpd_state *s))
{
    PSOCK_BEGIN(&s->sin);

    PSOCK_READTO(&s->sin, ISO_space);

//check pass
    if(strncmp(s->inputbuf, PASSWORD, strlen(PASSWORD)) != 0) {
    	lcdClearScreen();
    	lcdPrintString("wrong pass");
		PSOCK_CLOSE_EXIT(&s->sin);
    }
//read action
    PSOCK_READTO(&s->sin, ISO_space);
//set action
    s->action=s->inputbuf[0];
//read year
    PSOCK_READTO(&s->sin, DASH);
    s->inputbuf[4]='\0';
    s->year=atoi(s->inputbuf);
//read month
    PSOCK_READTO(&s->sin, DASH);
    s->inputbuf[2]='\0';
    s->month=atoi(s->inputbuf);
//read day
    PSOCK_READTO(&s->sin, 'T');
    s->inputbuf[2]='\0';
    s->day=atoi(s->inputbuf);
//read houre
    PSOCK_READTO(&s->sin, ISO_colon);
    s->inputbuf[2]='\0';
    s->houre=atoi(s->inputbuf);
//read minute
    PSOCK_READTO(&s->sin, ISO_colon);
    s->inputbuf[2]='\0';
    s->minute=atoi(s->inputbuf);
//read seconds
    PSOCK_READTO(&s->sin, ISO_period);
    s->inputbuf[2]='\0';
    s->sec=atoi(s->inputbuf);
//throw milisec
    PSOCK_READTO(&s->sin, '+');
//read timezone
    PSOCK_READTO(&s->sin, ISO_colon);
    s->inputbuf[2]='\0';
    s->timezone=atoi(s->inputbuf);
//throw left over
    PSOCK_READTO(&s->sin, ISO_space);

    s->state = STATE_OUTPUT;
	s->currentschedule=CreateSchedule(s->year,s->month,s->day,s->houre,s->minute);
	switch(s->action){
	case SET_CLOCK:
		RTC_init(s->sec,s->minute, s->houre,s->day, s->month, s->year);
		break;
	case SET_MEETING:
		if(AddSchedule(s->currentschedule)==0)
			RTC_SET_ALARM(0,s->minute, s->houre,s->day, s->month, s->year);
		break;
	case DELETE_MEETING:
		if(DeleteSchedule(s->currentschedule)==0){
			if(GetNumberOfSchedules()!=0){
				RTC_SET_ALARM(0,scheduleQ[0].minuts,scheduleQ[0].houre,scheduleQ[0].day,
					scheduleQ[0].month,scheduleQ[0].year);
			}
			else{
				RTC_DISABLE_ALARM();
			}
		}
		break;
	default:lcdPrintString("Unknown RQST\nfrom server");
		break;
	}

	PSOCK_CLOSE(&s->sin);
    PSOCK_END(&s->sin);
}

/*---------------------------------------------------------------------------*/
static void handle_connection(struct httpd_state *s)
{
    handle_input(s);
    if(s->state == STATE_OUTPUT) {
    	handle_output(s);
    }
}

/*---------------------------------------------------------------------------*/
void httpd_appcall(void)
{
	if(uip_conn->lport== HTONS(12333)){
		HandleWatchServer();
	}
	else{
		lcdClearScreen();
		lcdPrintString("send user&pass");
		SendUserPassword();
	}
}


/*---------------------------------------------------------------------------*/
/**
 * \brief      Initialize the web server
 *
 *             This function initializes the web server and should be
 *             called at system boot-up.
 */
void httpd_init(void)
{

	uip_ipaddr_t addr;
	uip_ipaddr(&addr,SERVER_IP1,SERVER_IP2,SERVER_IP3,SERVER_IP4);
	uip_connect(&addr,HTONS(SERVER_PORT));
	uip_listen(HTONS(LISTENNING_PORT));

}

void SendUserPassword(){
	if(uip_connected()||uip_rexmit())
		uip_send(USER_PASS,strlen(USER_PASS)+1);
}
void HandleWatchServer(){

	struct httpd_state *s = (struct httpd_state *)&(uip_conn->appstate);

	if(uip_closed()) {
	}
	else if(uip_aborted()){
	}
	else if(uip_timedout()){
	}
	else if(uip_connected()) {

		PSOCK_INIT(&s->sin, s->inputbuf, sizeof(s->inputbuf) - 1);
		PSOCK_INIT(&s->sout, s->inputbuf, sizeof(s->inputbuf) - 1);
		PT_INIT(&s->outputpt);
		s->state = STATE_NO_CONNECTION;
		s->timer = 0;
		handle_connection(s);
	}
	else if(s != NULL) {
		if(uip_poll()) {
			++s->timer;
			if(s->timer >= 20) {
				uip_abort();
			}
		}
		else {
			s->timer = 0;
		}
		handle_connection(s);
	}
	else {
		uip_abort();
	}
}
/*---------------------------------------------------------------------------*/
/** @} */
