/* Server Code 
Author: Dania Chavez
description: server side of program 2. This programm gets URL through client and server side gets HTML by connecting to HTTP through port 80
*/

#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>

int webserver(char REQUEST[], char hostname[])//function to connect to get HTML request connection
{
	int sockfd;
	struct sockaddr_in servaddr;
	struct hostent *host;
	
	sockfd=socket(AF_INET, SOCK_STREAM,0);
	host=gethostbyname(hostname);//the host of URL
	memset(&servaddr, 0, sizeof(servaddr)); //reser server address
		
	servaddr.sin_family=AF_INET;
	servaddr.sin_port=htons(80); //port 80
	memcpy(&servaddr.sin_addr.s_addr,host->h_addr,host->h_length);
		
	connect(sockfd,(struct sockaddr *)&servaddr, sizeof(servaddr)); //connect to http

	return sockfd;//return socket
}
 
int main(int argc, char **argv)
{
	if(argc>2)
	{
		printf("usage %s <port number>\n", argv[0]);
		exit(1);
	}
	
	
    	char recvline[40960];//initialize of variables
	char sentline[40959];
    	int listen_fd, conn_fd;
    	int port = atoi(argv[1]);//port of server nad client program 
		
	struct sockaddr_in servaddr;
 
	
    	/* AF_INET - IPv4 IP , Type of socket, protocol*/
    	listen_fd = socket(AF_INET, SOCK_STREAM, 0);
 
	bzero(&servaddr, sizeof(servaddr));
 
	servaddr.sin_family = AF_INET;
	servaddr.sin_addr.s_addr = htons(INADDR_ANY);	
	servaddr.sin_port = htons(port);
 
    	/* Binds the above details to the socket */
	bind(listen_fd,  (struct sockaddr *) &servaddr, sizeof(servaddr));

   	/* Start listening to incoming connections */
    	listen(listen_fd, 10);
	conn_fd = accept(listen_fd, (struct sockaddr*)NULL, NULL);
	
	int i=0;
    	char caches[5][50];
	char filenames[5][15];
	int cachescount=0; //there can only be 5 caches
	while(1) {
   		bzero(recvline, 40960);//zero out sent line and received line
		bzero(sentline, 40959);
		char storerecvline[sizeof(recvline)];
		char hostname[sizeof(recvline)];
		char pathway[sizeof(recvline)]="/";
		char *whatsLeftOfString;
		char *token;
		char *nameToken;
		char *nameTokenString;
		char line[40960];
		read(conn_fd, recvline, sizeof(recvline));//read received from client
			

		if(strncmp("quit", recvline, 4)==0)//if quit, break loop
		{
			printf("quiting\n");
			break;
		}
		else
		{
			FILE * cache=NULL;//stream initialize and open list.txt
			cache = fopen("list.txt", "r");
			if(cache==NULL)
			{
				printf("Unable to open file!\n");
				exit(1);
			}

			strcpy(storerecvline, recvline);//copy received URL from client
			token=strtok_r(storerecvline, "/", &whatsLeftOfString);//parse URL for hostname and pathway
			token=strtok_r(whatsLeftOfString,"/",&whatsLeftOfString);
			strcat(pathway, whatsLeftOfString);
			strcpy(hostname, token);
			
			int checkCache=0;//default not in cache file
			while(fgets(line, sizeof(line), cache)!=NULL)//loop through cache file to see if we already have HTML
			{
				
				nameToken=strtok_r(line," ",&nameTokenString);//store name of file in nameToken
				int checkcmp=strcmp(recvline,nameTokenString);
				
				if(checkcmp==0)//if URL in cache 
				{

					checkCache=1;//in cache
					break;
				}
			}
			fclose(cache);//close file
			
			if(checkCache==1)//in cache
			{
				char fileNameWrite[sizeof(sentline)];
				strcpy(fileNameWrite, nameToken);
				printf("SAME URL IN CACHE:%s\n",nameToken);
				FILE * nameFile;//open nameToken file
				nameFile = fopen(nameToken,"a+");
				if(nameFile==NULL)
				{
					printf("Unable to open file!\n");
					exit(1);
				}
				fclose(nameFile);
				
				write(conn_fd, fileNameWrite, sizeof(fileNameWrite));//write to client name of file
				
											
			}
			else if(checkCache==0)//not in cache
			{
				time_t stamptime;
				char storetime[80];
				time (&stamptime);	
				
				struct tm *stamp=localtime (&stamptime);//to get stamp time
				strftime (storetime,16, "%Y%m%d%H%M%S", stamp);
				FILE * cachefile;//storetime named file
				int sockfd;
				char send[200]="GET ";
				char response[8096];
				
				cachefile = fopen(storetime, "a+");//open file with stamptime name
				if(cachefile==NULL)//cant open file
				{	
					printf("Unable to open file!\n");
					exit(1);
				}
						
				strcat(send, pathway);//request in send string
				strcat(send, " HTTP/1.1 \r\nHost: ");
				strcat(send, hostname);
				strcat(send, "\r\n\r\n");
				
				sockfd=webserver(send, hostname);//
				write(sockfd, send, sizeof(send));//url port 80 write
				int j=0;	
				while(1)//read all request response
				{
					bzero(response, 8096);//memset(response, 0, sizeof(response));
					read(sockfd, response, sizeof(response)); //read response from port 80
					//printf("%s\n",response);
					//write(conn_fd, response, sizeof(response));	
					fputs(response, cachefile); //fprintf(cachefile, response);	
					strstr(response,"\0");
					if(j==7)
					{
						break;
					}
					else
					{
						j++;
					}	

				}
				close (sockfd);//close port 80 connection
				fclose(cachefile);//close cachefile
				char temp[40960];
				strcat(temp,"\n");
				strcat(temp,storetime);
				strcat(temp," ");
				strcat(temp,recvline);
				
				FILE * cacheNO=NULL;//stream initialize and open list.txt
				cacheNO = fopen("list.txt", "a+");
				if(cacheNO==NULL)
				{
					printf("Unable to open file cache list!\n");
					exit(1);
				}
				fputs(temp,cacheNO);
				fclose(cacheNO);
				write(conn_fd, storetime, sizeof(storetime));//write to client name of file
				
			}
		}
	}			
	close (conn_fd); //close the connection to client
	return 0;
}
