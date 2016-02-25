#include<netinet/in.h> // sockaddr_in 
#include<sys/types.h>  // socket 
#include<sys/socket.h> // socket 
#include<stdio.h>    // printf 
#include<stdlib.h>   // exit 
#include<string.h>   // bzero 



#include "socket.h"

#define LENGTH_OF_LISTEN_QUEUE  100
#define SERVER_PORT 8000



int server_socket_fd;

int socket_branch (void){

   struct sockaddr_in server_addr;
   bzero(&server_addr,sizeof(server_addr));

   server_addr.sin_family = AF_INET;
   server_addr.sin_addr.s_addr = htons(INADDR_ANY);
   server_addr.sin_port = htons(SERVER_PORT);

   server_socket_fd = socket(AF_INET,SOCK_STREAM,0);
   if(server_socket_fd<0)
   		{
   		  perror("Create Socket Failed:");
		  exit(1);
   		}
	
	/*
	int recv_buffer =0;
	getsockopt(server_socket_fd,SOL_SOCKET,SO_RCVBUF,&recv_buffer,sizeof(recv_buffer));
 	printf("getsocket recv_buffer is %d\n,recv_buffer");
	*/
    int val;
	socklen_t len;
	len = sizeof(val);
	char strres[128];
	
	int opt = 1;
    setsockopt(server_socket_fd,SOL_SOCKET,SO_REUSEADDR,&opt,sizeof(opt));
	
	if(getsockopt(server_socket_fd,SOL_SOCKET,SO_RCVBUF,&val,&len) == -1)
	{
		perror("getsockopt error");
	}
	else 
	{
		if(len != sizeof(int))
			snprintf(strres, sizeof(strres), "sizeof (%d) not sizeof(int)", len);
		else
			snprintf(strres, sizeof(strres), "%d", val);
		printf("default = %s\n", strres);
	}

	
 	if(-1 == (bind(server_socket_fd,(struct sockaddr*)&server_addr,sizeof(server_addr))))
 		{
			perror("Server Bind Failed:");
			exit(1);
		}

	if(-1 == (listen(server_socket_fd,LENGTH_OF_LISTEN_QUEUE)))
		{
			perror("Server Listen Failed:");
			exit(1);
		}
}
    





	
	
	
	