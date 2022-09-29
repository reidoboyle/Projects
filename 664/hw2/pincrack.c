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
#include "openssl/err.h"
#include "openssl/ssl.h"

void LoadCertificates(SSL_CTX* ctx, char* CertFile, char* KeyFile);
int OpenConnection(const char *hostname, int port);
void ShowCerts(SSL* ssl);
SSL_CTX* InitCTX(void);

#define FAIL -1
#define SERVER_PORT 3005
#define SERVER_NAME "localhost"
#define MAX_HOST_NAME_LENGTH 20
int pincrack(char *hash, int hashLength) {

/* Here you will implement all logic: 
socket creation, communication with the server and returning 
the value to the caller of this function. 
*/
    SSL_CTX *ctx;
    SSL *ssl;

    SSL_library_init();
    ctx = InitCTX();

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
    printf("Connection success yo\n");
    ssl = SSL_new(ctx);
    SSL_set_fd(ssl,sd);
    char buffer[10];
    int bytesReceived = 0;
    
    int pin =-1;
    if(SSL_connect(ssl) != FAIL) {
    	printf("Connected with %s encryption\n", SSL_get_cipher(ssl));
    	printf("ah\n"); 
	fflush(stdout);
	ShowCerts(ssl);

	SSL_write(ssl,hash,strlen(hash));
	
    	printf("Sent successfully\n");
	
    	memset(buffer,0,10);    
	
	bytesReceived = SSL_read(ssl,buffer,sizeof(buffer));
	buffer[bytesReceived] = 0;
	
	


	//rc = send(sd,hash,hashLength,0);
    	//if (rc < 0) {
        //	return -1;
    	//}
    	//rc = recv(sd,&buffer[bytesReceived],10-bytesReceived,0);
    	bytesReceived += rc;
    	if (buffer[0] == 'F')
    	{
            return -1;
    	}
	pin = atoi(buffer);

	SSL_free(ssl);
	
    }
    else printf("yoshi\n");//ERR_print_errors_fp(stderr);

    /*rc = send(sd,hash,hashLength,0);
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
    }*/

    SSL_CTX_free(ctx);

    return pin;
}
SSL_CTX* InitCTX(void) {


    SSL_METHOD const *method;
    SSL_CTX *ctx;


    // Load cryptos, et.al.
    OpenSSL_add_all_algorithms();


    // Bring in and register error messages
    SSL_load_error_strings();


    // Create new client-method instance
    method = SSLv23_client_method();


    // Create new context
    ctx = SSL_CTX_new(method);


    if ( ctx == NULL ) {


        ERR_print_errors_fp(stderr);
        abort();


    } // if


    return ctx;


} //InitCTX


int OpenConnection(const char *hostname, int port) {


    int sd;
    struct hostent *host;
    struct sockaddr_in addr;


    if ( (host = gethostbyname(hostname)) == NULL ) {


        perror(hostname);
        abort();


    } // if


    sd = socket(PF_INET, SOCK_STREAM, 0);
    bzero(&addr, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    addr.sin_addr.s_addr = *(long*)(host->h_addr);


    if ( connect(sd, (struct sockaddr*)&addr, sizeof(addr)) != 0 ) {


        close(sd);
        perror(hostname);
        abort();


    } // if


    return sd;


} // OpenConnection

void ShowCerts(SSL* ssl) {


    X509 *cert;
    char *line;


    cert = SSL_get_peer_certificate(ssl); /* get the server's certificate */


    if ( cert != NULL ) {


        printf("\nServer certificate:\n");
        line = X509_NAME_oneline(X509_get_subject_name(cert), 0, 0);
        printf("Subject: %s\n", line);


        // Free the malloc'ed string
        free(line);
        line = X509_NAME_oneline(X509_get_issuer_name(cert), 0, 0);
        printf("Issuer: %s\n", line);


        // Free the malloc'ed string
        free(line);


        // Free the malloc'ed certificate copy
        X509_free(cert);


    } // if


    else printf("No certificates.\n");


} // ShowCerts

