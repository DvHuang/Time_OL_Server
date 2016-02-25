/************************************************************************* 
  > File Name: Server.c 
  > Author: SongLee 
************************************************************************/
  
#include<netinet/in.h> // sockaddr_in 
#include<sys/types.h>  // socket 
#include<sys/socket.h> // socket 
#include<stdio.h>    // printf 
#include<stdlib.h>   // exit 
#include<string.h>   // bzero 
#include<pthread.h> //pthread
  
#define SERVER_PORT 30000 
#define LENGTH_OF_LISTEN_QUEUE 20 
#define BUFFER_SIZE 1024*10 
#define FILE_NAME_MAX_SIZE 512 

#define F_PATH "/home/davy/samba/socket_file/img0.jpg"

void *s_thread(void *fd);
  
int main(void) 
{ 

	 
 	//printf("Please Input File Name On Server:\t"); 
 	//scanf("%s", file_name); 
  // 声明并初始化一个服务器端的socket地址结构 
  struct sockaddr_in server_addr; 
  bzero(&server_addr, sizeof(server_addr)); 
  server_addr.sin_family = AF_INET; 
  server_addr.sin_addr.s_addr = htons(INADDR_ANY); 
  server_addr.sin_port = htons(SERVER_PORT); 
  
  // 创建socket，若成功，返回socket描述符 
  int server_socket_fd = socket(PF_INET, SOCK_STREAM, 0); 
  if(server_socket_fd < 0) 
  { 
    perror("Create Socket Failed:"); 
    exit(1); 
  } 
  int opt = 1; 
  setsockopt(server_socket_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)); 
  
  // 绑定socket和socket地址结构 
  if(-1 == (bind(server_socket_fd, (struct sockaddr*)&server_addr, sizeof(server_addr)))) 
  { 
    perror("Server Bind Failed:"); 
    exit(1); 
  } 
    
  // socket监听 
  if(-1 == (listen(server_socket_fd, LENGTH_OF_LISTEN_QUEUE))) 
  { 
    perror("Server Listen Failed:"); 
    exit(1); 
  } 
  
  while(1) 
  { 
    // 定义客户端的socket地址结构 
    struct sockaddr_in client_addr; 
    socklen_t client_addr_length = sizeof(client_addr); 
	
	// 创建不同的子线程以区别不同的客户端  
	pthread_t thread;
	
  
    // 接受连接请求，返回一个新的socket(描述符)，这个新socket用于同连接的客户端通信 
    // accept函数会把连接到的客户端信息写到client_addr中 
      int *new_server_socket_fd;  
     new_server_socket_fd = (int *)malloc(sizeof(int));  
     *new_server_socket_fd = accept(server_socket_fd, (struct sockaddr*)&client_addr, &client_addr_length); 
    if(new_server_socket_fd < 0) 
   		 { 
     		 perror("Server Accept Failed:"); 
      		 break; 
   		 } 
	if(pthread_create(&thread, NULL, s_thread, new_server_socket_fd)!=0)//创建子线程  
                  {  
                         perror("pthread_create");  
                         break;  
                  } 

  	close(server_socket_fd); 
  	return 0; 
  }
}

	 void *s_thread(void *fd)  
    {  
           int client_sockfd;  
           int i,byte;  
           char char_recv[100];//存放数据  
           client_sockfd=*((int*)fd);  
		   char buffer[BUFFER_SIZE]; 
    	   bzero(buffer, BUFFER_SIZE); 
		    
           for(;;)  
           {  
               // recv函数接收数据到缓冲区buffer中 
   			
    		 if(recv(client_sockfd, buffer, BUFFER_SIZE, 0) < 0) 
    		{ 
   			  perror("Server Recieve Data Failed:"); 
     			 break; 
    		} 
  
   			// 然后从buffer(缓冲区)拷贝到file_name中 
    		char file_name[FILE_NAME_MAX_SIZE+1]; 
    		bzero(file_name, FILE_NAME_MAX_SIZE+1); 
    		strncpy(file_name, buffer, strlen(buffer)>FILE_NAME_MAX_SIZE?FILE_NAME_MAX_SIZE:strlen(buffer)); 
   			printf("%s\n", file_name); 
    
			// 打开文件并读取文件数据 
    		FILE *fp = fopen(file_name, "w"); 
    		if(NULL == fp) 
    		{ 
    		  printf("File:%s Not Found\n", file_name); 
    		} 
   			 else
    		{ 
     		 bzero(buffer, BUFFER_SIZE); 
      		int length = 0; 
      		// 每读取一段数据，便将其发送给客户端，循环直到文件读完为止 
     		 while((length = recv(client_sockfd, buffer, BUFFER_SIZE, 0)) > 0) 
      		{ 
        		if(fwrite(buffer, sizeof(char), length, fp) < length) 
       			 { 
         		 printf("File:\t%s Write Failed\n", file_name); 
         		 break; 
        		} 
       	 bzero(buffer, BUFFER_SIZE); 
      } 
  
      // 关闭文件 
      fclose(fp); 
      printf("File:%s Transfer Successful!\n", F_PATH); 
    	} 
   		 // 关闭与客户端的连接 
   			 close(client_sockfd); 
   		} 
             
           free(fd);  
           close(client_sockfd);  
           pthread_exit(NULL);  
    } 


	 
  
   
  // 关闭监听用的socket 


