#include <sys/select.h>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>    
#include <netinet/in.h>
#include <unistd.h>
#include <openssl/sha.h>
#include <math.h>

#define SERVER_PORT     3005
#define BUFFER_LENGTH    250
#define FALSE              0

int  main() {
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

      timeout.tv_sec  = 0;
      timeout.tv_usec = 0;

      FD_ZERO(&read_fd);
      FD_SET(sd2, &read_fd);

      rc = select(1, &read_fd, NULL, NULL, &timeout);
      // test error rc < 0

      length = BUFFER_LENGTH;
      memset(buffer,0,length);
      rc = recv(sd2, buffer, sizeof(buffer), 0);
      // test error rc < 0 or rc == 0 or   rc < sizeof(buffer
      printf("server received %d bytes\n", rc);
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
	    rc = send(sd2, buffer,4, 0);
	    break;
	}
	
		
      }
      if(!found) {
          printf("PIN could not be found\n");
          memset(buffer,0,BUFFER_LENGTH);
          buffer[0] = 'F';
          rc = send(sd2, buffer,1, 0);
      // test error rc < 0
          printf("server returned %d bytes\n", rc);
      }
   } while (1);

   if (sd != -1)
      close(sd);
   if (sd2 != -1)
      close(sd2);
}


