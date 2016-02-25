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
  // ��������ʼ��һ���������˵�socket��ַ�ṹ 
  struct sockaddr_in server_addr; 
  bzero(&server_addr, sizeof(server_addr)); 
  server_addr.sin_family = AF_INET; 
  server_addr.sin_addr.s_addr = htons(INADDR_ANY); 
  server_addr.sin_port = htons(SERVER_PORT); 
  
  // ����socket�����ɹ�������socket������ 
  int server_socket_fd = socket(PF_INET, SOCK_STREAM, 0); 
  if(server_socket_fd < 0) 
  { 
    perror("Create Socket Failed:"); 
    exit(1); 
  } 
  int opt = 1; 
  setsockopt(server_socket_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)); 
  
  // ��socket��socket��ַ�ṹ 
  if(-1 == (bind(server_socket_fd, (struct sockaddr*)&server_addr, sizeof(server_addr)))) 
  { 
    perror("Server Bind Failed:"); 
    exit(1); 
  } 
    
  // socket���� 
  if(-1 == (listen(server_socket_fd, LENGTH_OF_LISTEN_QUEUE))) 
  { 
    perror("Server Listen Failed:"); 
    exit(1); 
  } 
  
  while(1) 
  { 
    // ����ͻ��˵�socket��ַ�ṹ 
    struct sockaddr_in client_addr; 
    socklen_t client_addr_length = sizeof(client_addr); 
	
	// ������ͬ�����߳�������ͬ�Ŀͻ���  
	pthread_t thread;
	
  
    // �����������󣬷���һ���µ�socket(������)�������socket����ͬ���ӵĿͻ���ͨ�� 
    // accept����������ӵ��Ŀͻ�����Ϣд��client_addr�� 
      int *new_server_socket_fd;  
     new_server_socket_fd = (int *)malloc(sizeof(int));  
     *new_server_socket_fd = accept(server_socket_fd, (struct sockaddr*)&client_addr, &client_addr_length); 
    if(new_server_socket_fd < 0) 
   		 { 
     		 perror("Server Accept Failed:"); 
      		 break; 
   		 } 
	if(pthread_create(&thread, NULL, s_thread, new_server_socket_fd)!=0)//�������߳�  
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
           char char_recv[100];//�������  
           client_sockfd=*((int*)fd);  
		   char buffer[BUFFER_SIZE]; 
    	   bzero(buffer, BUFFER_SIZE); 
		    
           for(;;)  
           {  
               // recv�����������ݵ�������buffer�� 
   			
    		 if(recv(client_sockfd, buffer, BUFFER_SIZE, 0) < 0) 
    		{ 
   			  perror("Server Recieve Data Failed:"); 
     			 break; 
    		} 
  
   			// Ȼ���buffer(������)������file_name�� 
    		char file_name[FILE_NAME_MAX_SIZE+1]; 
    		bzero(file_name, FILE_NAME_MAX_SIZE+1); 
    		strncpy(file_name, buffer, strlen(buffer)>FILE_NAME_MAX_SIZE?FILE_NAME_MAX_SIZE:strlen(buffer)); 
   			printf("%s\n", file_name); 
    
			// ���ļ�����ȡ�ļ����� 
    		FILE *fp = fopen(file_name, "w"); 
    		if(NULL == fp) 
    		{ 
    		  printf("File:%s Not Found\n", file_name); 
    		} 
   			 else
    		{ 
     		 bzero(buffer, BUFFER_SIZE); 
      		int length = 0; 
      		// ÿ��ȡһ�����ݣ��㽫�䷢�͸��ͻ��ˣ�ѭ��ֱ���ļ�����Ϊֹ 
     		 while((length = recv(client_sockfd, buffer, BUFFER_SIZE, 0)) > 0) 
      		{ 
        		if(fwrite(buffer, sizeof(char), length, fp) < length) 
       			 { 
         		 printf("File:\t%s Write Failed\n", file_name); 
         		 break; 
        		} 
       	 bzero(buffer, BUFFER_SIZE); 
      } 
  
      // �ر��ļ� 
      fclose(fp); 
      printf("File:%s Transfer Successful!\n", F_PATH); 
    	} 
   		 // �ر���ͻ��˵����� 
   			 close(client_sockfd); 
   		} 
             
           free(fd);  
           close(client_sockfd);  
           pthread_exit(NULL);  
    } 


	 
  
   
  // �رռ����õ�socket 


