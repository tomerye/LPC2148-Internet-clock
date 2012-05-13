//============================================================================
// Name        : GoogleCalender.cpp
//============================================================================

#include <string.h>
#include <stdlib.h>
extern "C" {
#include <gcalendar.h>
#include <gcontact.h>
#include <internal_gcal.h>
#include <gcal.h>
}
#include "GCAccount.h"


#include <fstream>
#include <stdio.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <sys/wait.h>
#include <signal.h>
#include <pthread.h>

using namespace std;

#define DEBUG

#define SERVER_PORT "12222"
#define CLIENT_PORT "12333"
#define ALARMSTR "ALARM"
#define BACKLOG 10 // how many pending connections queue will hold

//GCAccount* gca;

void sigchld_handler(int s);
void *get_in_addr(struct sockaddr *sa);
void* threadfunc(void * new_gca);



void sigchld_handler(int s)
{
    while(waitpid(-1, NULL, WNOHANG) > 0);
}

// get sockaddr, IPv4 or IPv6:
void *get_in_addr(struct sockaddr *sa)
{
    if (sa->sa_family == AF_INET) {
        return &(((struct sockaddr_in*)sa)->sin_addr);
    }

    return &(((struct sockaddr_in6*)sa)->sin6_addr);
}


void* threadfunc(void * new_gca) {
	int check;
	GCAccount* gca_thread= (*(GCAccount**) new_gca);
	while (1) {
		check=gca_thread->action();
				
		if(check!=1)
			break;
	}
	
	printf("end connection\n");
	return NULL;	
}




int main(int argc, char *argv[])
{
	
	int sockfd, new_fd;  // listen on sock_fd, new connection on new_fd
    struct addrinfo hints, *servinfo, *p;
    struct sockaddr_storage their_addr; // connector's address information
    socklen_t sin_size;
    struct sigaction sa;
    int yes=1;
    char ipaddress[INET6_ADDRSTRLEN];
    int rv;
	char * port=strdup(SERVER_PORT);
    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE; 

	rv = getaddrinfo(NULL, port, &hints, &servinfo);
 
    if (rv != 0) {
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
        return 1;
    }

    // loop through all the results and bind to the first we can
    for(p = servinfo; p != NULL; p = p->ai_next) {
        if ((sockfd = socket(p->ai_family, p->ai_socktype,
                p->ai_protocol)) == -1) {
            perror("server: socket");
            continue;
        }

        if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &yes,
                sizeof(int)) == -1) {
            perror("setsockopt");
            exit(1);
        }

        if (bind(sockfd, p->ai_addr, p->ai_addrlen) == -1) {
            close(sockfd);
            perror("server: bind");
            continue;
        }

        break;
    }

    if (p == NULL)  {
        fprintf(stderr, "server: failed to bind\n");
        return 2;
    }

    freeaddrinfo(servinfo); // all done with this structure

    if (listen(sockfd, BACKLOG) == -1) {
        perror("listen");
        exit(1);
    }

    sa.sa_handler = sigchld_handler; // reap all dead processes
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = SA_RESTART;
    if (sigaction(SIGCHLD, &sa, NULL) == -1) {
        perror("sigaction");
        exit(1);
    }

    printf("server: waiting for connections...\n");
	
    while(1) { 
        sin_size = sizeof their_addr;
        int numbytes;
        new_fd = accept(sockfd, (struct sockaddr *)&their_addr, &sin_size);
        pthread_t thread;
        if (new_fd == -1) {
            perror("accept");
            continue;
        }

        inet_ntop(their_addr.ss_family,
            get_in_addr((struct sockaddr *)&their_addr),
            ipaddress, sizeof ipaddress);
        char buf[100];    
        printf("server: got connection from %s\n", ipaddress);
        if ((numbytes = recv(new_fd, buf, 99, 0)) == -1) {
			perror("recv");
			exit(1);
		}
		buf[numbytes] = '\0';
		close(new_fd);
		char* pch = strtok (buf," ");
        char* user=strdup(pch);
        pch=strtok(NULL," ");
        char* pass=strdup(pch);
        char* AlaramTittle=strdup(ALARMSTR);
        char* connectPort=strdup(CLIENT_PORT);
		GCAccount* gca= new GCAccount(user,pass,AlaramTittle,ipaddress,connectPort);
		pthread_create(&thread, NULL, threadfunc, (void *) &gca);
    }
	
	
	
	
	
	
	
	
	
	
	
	/*
	int check;
	char port[6],host[100];

	if(argc==3){
		strcpy(host,argv[1]);
		strcpy(port,argv[2]);
	}
	else{
		printf("Error:wrong parameters/n");
		exit(1);
	}	
	
	
	gca= new GCAccount("motit86","motymoty","aaa",host,port);
	while (1) {
		check=gca->action();
		
		if(check!=1)
			break;
	}
	printf("problem detect\n");
	return 1;
	*/
}
