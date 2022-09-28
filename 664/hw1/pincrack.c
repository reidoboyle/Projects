#include <sys/select.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "pincrack.h"

#define SERVER_PORT 3005
#define SERVER_NAME "localhost"
#define MAX_HOST_NAME_LENGTH 20
int pincrack(char *hash, int hashLength) {

/* Here you will implement all logic: 
socket creation, communication with the server and returning 
the value to the caller of this function. 
*/
    struct sockaddr_in serveraddr;
    struct hostent *hostp;
    int sd = socket(AF_INET,SOCK_STREAM,0);

    char server[MAX_HOST_NAME_LENGTH];
    strcpy(server,SERVER_NAME);

    if(sd < 0) {
	printf("Error creating socket\n");
	return -1;	
    }
    memset(&serveraddr,0,sizeof(serveraddr));
    serveraddr.sin_family = AF_INET;
    serveraddr.sin_port = htons(SERVER_PORT);
    serveraddr.sin_addr.s_addr = inet_addr(server);	
   
    if(serveraddr.sin_addr.s_addr == (unsigned long)INADDR_NONE) {
	hostp = gethostbyname(server);
	if(hostp == (struct hostent *)NULL) {
	    return -1;
	}	
	memcpy(&serveraddr.sin_addr,hostp->h_addr,sizeof(serveraddr.sin_addr));
    }
    int rc = connect(sd,(struct sockaddr *)&serveraddr,sizeof(serveraddr));
    if (rc < 0) {
	return -1;
    }
    printf("Connection success\n");
    
    rc = send(sd,hash,hashLength,0);
    if (rc < 0) {
	return -1;
    }
    printf("Sent successfully\n");
    int bytesReceived = 0;
    char buffer[10];
    memset(buffer,0,10);    
    rc = recv(sd,&buffer[bytesReceived],10-bytesReceived,0);
    bytesReceived += rc;
    if (buffer[0] == 'F')
    {
	return -1;
    }
    int pin = atoi(buffer);
    return pin;
}
