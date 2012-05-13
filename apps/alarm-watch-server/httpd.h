

#ifndef __HTTPD_H__
#define __HTTPD_H__

#include "psock.h"
#include "alarm-watch.h"


struct httpd_state {
    unsigned char timer;
    struct psock sin, sout;
    struct pt outputpt;
    char inputbuf[100];
    char state;

    unsigned short count;
   schedule currentschedule;
    int year;
    char month;
    char day;
    char houre;
    char minute;
    char sec;
    char timezone;
    char action;

};

void httpd_init(void);
void httpd_appcall(void);
void httpd_log(char *msg);
void httpd_log_file(u16_t *requester, char *file);
void SendUserPassword();
void HandleWatchServer();

#endif /* __HTTPD_H__ */
