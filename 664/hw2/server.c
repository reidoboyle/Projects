#include <sys/select.h>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>    
#include <netinet/in.h>
#include <unistd.h>
#include <openssl/sha.h>
#include <math.h>
#include <resolv.h>
#include "openssl/ssl.h"
#include "openssl/err.h"

#define SERVER_PORT     3005
#define BUFFER_LENGTH    250
#define FALSE              0
#define FAIL		  -1

SSL_CTX* InitServerCTX(void)
{
    const SSL_METHOD *method;
    SSL_CTX *ctx;
    OpenSSL_add_all_algorithms();  /* load & register all cryptos, etc. */
    SSL_load_error_strings();   /* load all error messages */
    method = TLS_server_method();  /* create new server-method instance */
    ctx = SSL_CTX_new(method);   /* create new context from method */
    if ( ctx == NULL )
    {
        ERR_print_errors_fp(stderr);
        abort();
    }
    return ctx;
}
void LoadCertificates(SSL_CTX* ctx, char* CertFile, char* KeyFile)
{
    /* set the local certificate from CertFile */
    if ( SSL_CTX_use_certificate_file(ctx, CertFile, SSL_FILETYPE_PEM) <= 0 )
    {
        ERR_print_errors_fp(stderr);
        abort();
    }
    /* set the private key from KeyFile (may be the same as CertFile) */
    if ( SSL_CTX_use_PrivateKey_file(ctx, KeyFile, SSL_FILETYPE_PEM) <= 0 )
    {
        ERR_print_errors_fp(stderr);
        abort();
    }
    /* verify private key */
    if ( !SSL_CTX_check_private_key(ctx) )
    {
        fprintf(stderr, "Private key does not match the public certificate\n");
        abort();
    }
}


void ShowCerts(SSL* ssl)
{
    X509 *cert;
    char *line;
    cert = SSL_get_peer_certificate(ssl); /* Get certificates (if available) */
    if ( cert != NULL )
    {
        printf("Server certificates:\n");
        line = X509_NAME_oneline(X509_get_subject_name(cert), 0, 0);
        printf("Subject: %s\n", line);
        free(line);
        line = X509_NAME_oneline(X509_get_issuer_name(cert), 0, 0);
        printf("Issuer: %s\n", line);
        free(line);
        X509_free(cert);
    }
    else
        printf("No certificates.\n");
}

void Servlet(SSL* ssl) /* Serve the connection -- threadable */
{
    char buf[1024] = {0};
    int sd, bytes;
    const char* ServerResponse="<\\Body>\
                               <Name>aticleworld.com</Name>\
                 <year>1.5</year>\
                 <BlogType>Embedede and c\\c++<\\BlogType>\
                 <Author>amlendra<Author>\
                 <\\Body>";
    const char *cpValidMessage = "<Body>\
                               <UserName>aticle<UserName>\
                 <Password>123<Password>\
                 <\\Body>";
    if ( SSL_accept(ssl) == FAIL )     /* do SSL-protocol accept */
        ERR_print_errors_fp(stderr);
    else
    {
        ShowCerts(ssl);        /* get any certificates */
        bytes = SSL_read(ssl, buf, sizeof(buf)); /* get request */
        buf[bytes] = '\0';
        printf("Client msg: \"%s\"\n", buf);
        if ( bytes > 0 )
        {
            if(strcmp(cpValidMessage,buf) == 0)
            {
                SSL_write(ssl, ServerResponse, strlen(ServerResponse)); /* send reply */
            }
            else
            {
                SSL_write(ssl, "Invalid Message", strlen("Invalid Message")); /* send reply */
            }
        }
        else
        {
            ERR_print_errors_fp(stderr);
        }
    }
    sd = SSL_get_fd(ssl);       /* get socket connection */
    SSL_free(ssl);         /* release SSL state */
    close(sd);          /* close connection */
}

int  main() {
   SSL_CTX *ctx;
   SSL *ssl;
   
   SSL_library_init();
   ctx = InitServerCTX();
   LoadCertificates(ctx, "cert.pem","key.pem");

   int    sd=-1, sd2=-1;
   int    rc, length, on=1;
   char   buffer[BUFFER_LENGTH];
   fd_set read_fd;
   struct timeval timeout;
   struct sockaddr_in serveraddr;
   int found = 0;
   sd = socket(AF_INET, SOCK_STREAM, 0);
   // test error: sd < 0)      

   memset(&serveraddr, 0, sizeof(serveraddr));
   serveraddr.sin_family      = AF_INET;
   serveraddr.sin_port        = htons(SERVER_PORT);
   serveraddr.sin_addr.s_addr = htonl(INADDR_ANY);

   rc = bind(sd, (struct sockaddr *)&serveraddr, sizeof(serveraddr));
   // test error rc < 0

   rc = listen(sd, 10);
   // test error rc< 0

   printf("Ready for client connect().\n");

   do {

      sd2 = accept(sd, NULL, NULL);
      // test error sd2 < 0
      
      ssl = SSL_new(ctx);
      SSL_set_fd(ssl,sd2);
      
      const char* ServerResponse="<\\Body>\
                               <Name>aticleworld.com</Name>\
                 <year>1.5</year>\
                 <BlogType>Embedede and c\\c++<\\BlogType>\
                 <Author>amlendra<Author>\
                 <\\Body>";
      const char *cpValidMessage = "<Body>\
                               <UserName>aticle<UserName>\
                 <Password>123<Password>\
                 <\\Body>";

      if(SSL_accept(ssl) == FAIL)
      {
	ERR_print_errors_fp(stderr);	      
      }
      ShowCerts(ssl);
      
      length = BUFFER_LENGTH;
      memset(buffer,0,length);
      int bites = SSL_read(ssl,buffer,sizeof(buffer));
      buffer[bites] = 0;
      printf("%s\n\n",buffer);
      //timeout.tv_sec  = 0;
      //timeout.tv_usec = 0;

      //FD_ZERO(&read_fd);
      //FD_SET(sd2, &read_fd);

      //rc = select(1, &read_fd, NULL, NULL, &timeout);
      // test error rc < 0

      //rc = recv(sd2, buffer, sizeof(buffer), 0);
      // test error rc < 0 or rc == 0 or   rc < sizeof(buffer
      printf("server received %d bytes\n", bites);
      // compute sha1 for each number 
      
      SHA_CTX shactx;
      unsigned char digest[SHA_DIGEST_LENGTH];
      int len;
      char char_digest[SHA_DIGEST_LENGTH*2];
      char pin[4];
      for(int i=0; i<= 9999; i++)
      {
	memset(pin,0,4);
	if(i==0)
	    len = 1;
	else
	    len = floor(log10(i)+1);
	sprintf(pin,"%d",i);
	memset(digest,0,SHA_DIGEST_LENGTH);
	SHA1_Init(&shactx);
	SHA1_Update(&shactx, pin,len);
	SHA1_Final(digest,&shactx);

	//for(int j=0;j<SHA_DIGEST_LENGTH;j++)
	//	printf("%02x",digest[j]);
	//printf("\n");
	memset(char_digest,0,SHA_DIGEST_LENGTH*2);
	char temp[2];
	found = 1;
	for(int k=0;k<sizeof(digest);k++)
	{
	    sprintf(temp,"%02x",digest[k]);
	    if(temp[0] == buffer[k*2] && temp[1] == buffer[k*2+1])
	    {
		continue;
	    }
	    else {
		found = 0;
	    	break;
	    }
	}
	if(found)
	{
	    printf("PIN found: %d\n",i);
	    sprintf(pin,"%d",i);
	    memset(buffer,0,BUFFER_LENGTH);
	    strncpy(buffer,pin,sizeof(pin));	    
	    //rc = send(sd2, buffer,4, 0);
	    SSL_write(ssl,buffer,strlen(buffer));
	    break;
	}
	
		
      }
      if(!found) {
          printf("PIN could not be found\n");
          memset(buffer,0,BUFFER_LENGTH);
          buffer[0] = 'F';
          //rc = send(sd2, buffer,1, 0);
      	  SSL_write(ssl,buffer,strlen(buffer));
	  // test error rc < 0
          printf("server returned %d bytes\n", rc);
      }
   } while (1);
	
   if (sd != -1)
      close(sd);
   if (sd2 != -1)
      close(sd2);
   SSL_free(ssl);
   SSL_CTX_free(ctx);
}


