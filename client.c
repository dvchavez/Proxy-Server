/* Client Code 
Author: Dania Chavez
description: client side of program 2. This programm gets URL through client and server side gets HTML by connecting to HTTP through port 80
*/

#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <time.h>
 
int main (int argc, char **argv)
{
	if(argc>2)
	{	
		printf("usage %s <port number>\n", argv[0]);
		exit(1);
	}	
	
    	int sockfd;
    	int len = sizeof(struct sockaddr);
    	char recvline[40961];
	char sentline[40959];
	int port = atoi(argv[1]);

    	struct sockaddr_in servaddr;//start connection

   	/* AF_INET - IPv4 IP , Type of socket, protocol*/
    	sockfd=socket(AF_INET, SOCK_STREAM, 0);
	bzero(&servaddr,sizeof(servaddr));
 
    	servaddr.sin_family=AF_INET;
    	servaddr.sin_port=htons(port); // Server port number
 
    	/* Convert IPv4 and IPv6 addresses from text to binary form */
    	inet_pton(AF_INET,"129.120.151.94",&(servaddr.sin_addr));
 
   	/* Connect to the server */
    	connect(sockfd,(struct sockaddr *)&servaddr,sizeof(servaddr));
 	
	printf("Client %i\n", port);
	
    	while(1)
	{
		bzero(sentline, 40959);//zero out variables
		bzero(recvline, 40961);
        	printf("URL: ");
		scanf("%s", sentline);
		if(strncmp("quit", sentline, 4)==0)//if quit, break loop
		{
			printf("quiting\n");
			write(sockfd, sentline, sizeof(sentline));//request to server to request response
			break;
		}
		else
		{
			//printf("%s\n",sentline);
			write(sockfd, sentline, sizeof(sentline));//request to server to request response
			bzero(recvline, 40961);
			read(sockfd, recvline, sizeof(recvline));
			printf("File Name : %s\n",recvline);	
		}
	}
}
