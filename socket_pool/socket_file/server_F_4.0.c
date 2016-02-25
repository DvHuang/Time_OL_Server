/************************************************************************* 
  > File Name: Server.c 
  > Author: davy 
************************************************************************/
  
#include<netinet/in.h> // sockaddr_in 
#include<sys/types.h>  // socket 
#include<sys/socket.h> // socket 
#include<stdio.h>    // printf 
#include<stdlib.h>   // exit 
#include<string.h>   // bzero 
  
#define SERVER_PORT 8000 
#define LENGTH_OF_LISTEN_QUEUE 20 
#define BUFFER_SIZE 1024 
#define FILE_NAME_MAX_SIZE 512 

#define Command_Max_Size 4
#define commandchar_recv "recv"
#define commandchar_send "send"

#define F_PATH "/home/davy/samba/socket_file/img0.jpg"
  
int main(void) 
{ 
    //定义文件名称的字符串获取文件名称
	char file_name[FILE_NAME_MAX_SIZE+1]; 
 	bzero(file_name, FILE_NAME_MAX_SIZE+1); 
 	printf("Please Input File Name On Server:\t"); 
 	scanf("%s", file_name); 
	//定义命令字字符串
	char command[Command_Max_Size+1]; 
 	bzero(command, Command_Max_Size+1); 

	
  // 建立socket链接第一步 :::: 声明并初始化一个服务器端的socket地址结构 
  // 其实就是创建一个结构体，并且对结构体赋值
  struct sockaddr_in server_addr; 
  bzero(&server_addr, sizeof(server_addr)); 
  server_addr.sin_family = AF_INET; 
  server_addr.sin_addr.s_addr = htons(INADDR_ANY); 
  server_addr.sin_port = htons(SERVER_PORT); 
  
  // second step::::::创建socket，若成功，返回socket描述符 
  int server_socket_fd = socket(PF_INET, SOCK_STREAM, 0); 
  if(server_socket_fd < 0) 
  { 
    perror("Create Socket Failed:"); 
    exit(1); 
  } 
  int opt = 1; 
  setsockopt(server_socket_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)); 
  
  // third step:::绑定socket和socket地址结构 
  if(-1 == (bind(server_socket_fd, (struct sockaddr*)&server_addr, sizeof(server_addr)))) 
  { 
    perror("Server Bind Failed:"); 
    exit(1); 
  } 
    
  //fourth step::::: socket监听 
  if(-1 == (listen(server_socket_fd, LENGTH_OF_LISTEN_QUEUE))) 
  { 
    perror("Server Listen Failed:"); 
    exit(1); 
  } 
  
  while(1) 
  { 

  	//该步用来建立服务器与客户端的链接，accept函数的调用会返回一个新的句柄，这个句柄用来
  	//表示客户端与服务端之间建立的管道，在accept函数中，我们需要一个容器，用来盛放客户端信息，
  	//所以要先构建该容器，即struct sockaddr_in client_addr;
    // 定义客户端的socket地址结构 
    struct sockaddr_in client_addr; 
    socklen_t client_addr_length = sizeof(client_addr); 
  
    // 接受连接请求，返回一个新的socket(描述符)，这个新socket用于同连接的客户端通信 
    // accept函数会把连接到的客户端信息写到client_addr中 
    int new_server_socket_fd = accept(server_socket_fd, (struct sockaddr*)&client_addr, &client_addr_length); 
    if(new_server_socket_fd < 0) 
    { 
      perror("Server Accept Failed:"); 
      break; 
    }
	else{
	  printf("accept client socket address \n");
	}

	
  
  
	char buffer[BUFFER_SIZE]; 
	while(1){
	
	//接收客户端命令
	printf("receive commandchar...");
	while((strcmp(command,commandchar_send)!=0)&&(strcmp(command,commandchar_recv)!=0))
	 {
	    int dotcount=0;
		if(recv(new_server_socket_fd, buffer, Command_Max_Size, 0) < 0) 
		   { 
		      perror("Server Recieve Data Failed:"); 
		      break; 
		   } 
			
		strncpy(command, buffer, strlen(buffer)>Command_Max_Size?Command_Max_Size:strlen(buffer)); 
		if(dotcount>100){dotcount=0;printf(".");}
		dotcount++;
	 }

	

	//向客户端发送数据，接收到的是send命令
	if(strcmp(command,commandchar_send)==0)
	 {
		bzero(command, Command_Max_Size+1);
    	// 打开文件并读取文件数据 
    	FILE *fp = fopen(file_name, "r"); 
	
		    if(NULL == fp) 
			{ 
		      printf("File:%s Not Found\n", file_name); 
		    } 
		    else
			{ 
			
		      bzero(buffer, BUFFER_SIZE); 
		      int length = 0; 
			  
		      // 每读取一段数据，便将其发送给客户端，循环直到文件读完为止 
		      while((length = fread(buffer, sizeof(char), BUFFER_SIZE, fp)) > 0) 
		      { 
			    if(send(new_server_socket_fd, buffer, length, 0) < 0) 
			        { 
				          printf("Send File:%s Failed./n", file_name); 
				          break; 
			        } 
		        bzero(buffer, BUFFER_SIZE); 
		      } 
		  
		      // 关闭文件 
		      fclose(fp); 
		      printf("File:%s Transfer Successful!\n", F_PATH);
		   	} 
	    	
	 }
	//接收客户端发来的数据，接收到的是recv命令
	if(strcmp(command,commandchar_recv)==0)
	{
			bzero(command, Command_Max_Size+1);
			
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
		      // 从服务器接收数据到buffer中 
  			  // 每接收一段数据，便将其写入文件中，循环直到文件接收完并写完为止
		      while((length = recv(new_server_socket_fd, buffer, BUFFER_SIZE, 0)) > 0) 
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
	   
	} 
  	}
	// 关闭与客户端的连接 
 	close(new_server_socket_fd); 
 }
 
 
 // 关闭监听用的socket 
 close(server_socket_fd); 
 return 0; 

}



