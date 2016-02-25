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
    //�����ļ����Ƶ��ַ�����ȡ�ļ�����
	char file_name[FILE_NAME_MAX_SIZE+1]; 
 	bzero(file_name, FILE_NAME_MAX_SIZE+1); 
 	printf("Please Input File Name On Server:\t"); 
 	scanf("%s", file_name); 
	//�����������ַ���
	char command[Command_Max_Size+1]; 
 	bzero(command, Command_Max_Size+1); 

	
  // ����socket���ӵ�һ�� :::: ��������ʼ��һ���������˵�socket��ַ�ṹ 
  // ��ʵ���Ǵ���һ���ṹ�壬���ҶԽṹ�帳ֵ
  struct sockaddr_in server_addr; 
  bzero(&server_addr, sizeof(server_addr)); 
  server_addr.sin_family = AF_INET; 
  server_addr.sin_addr.s_addr = htons(INADDR_ANY); 
  server_addr.sin_port = htons(SERVER_PORT); 
  
  // second step::::::����socket�����ɹ�������socket������ 
  int server_socket_fd = socket(PF_INET, SOCK_STREAM, 0); 
  if(server_socket_fd < 0) 
  { 
    perror("Create Socket Failed:"); 
    exit(1); 
  } 
  int opt = 1; 
  setsockopt(server_socket_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)); 
  
  // third step:::��socket��socket��ַ�ṹ 
  if(-1 == (bind(server_socket_fd, (struct sockaddr*)&server_addr, sizeof(server_addr)))) 
  { 
    perror("Server Bind Failed:"); 
    exit(1); 
  } 
    
  //fourth step::::: socket���� 
  if(-1 == (listen(server_socket_fd, LENGTH_OF_LISTEN_QUEUE))) 
  { 
    perror("Server Listen Failed:"); 
    exit(1); 
  } 
  
  while(1) 
  { 

  	//�ò�����������������ͻ��˵����ӣ�accept�����ĵ��û᷵��һ���µľ��������������
  	//��ʾ�ͻ���������֮�佨���Ĺܵ�����accept�����У�������Ҫһ������������ʢ�ſͻ�����Ϣ��
  	//����Ҫ�ȹ�������������struct sockaddr_in client_addr;
    // ����ͻ��˵�socket��ַ�ṹ 
    struct sockaddr_in client_addr; 
    socklen_t client_addr_length = sizeof(client_addr); 
  
    // �����������󣬷���һ���µ�socket(������)�������socket����ͬ���ӵĿͻ���ͨ�� 
    // accept����������ӵ��Ŀͻ�����Ϣд��client_addr�� 
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
	
	//���տͻ�������
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

	

	//��ͻ��˷������ݣ����յ�����send����
	if(strcmp(command,commandchar_send)==0)
	 {
		bzero(command, Command_Max_Size+1);
    	// ���ļ�����ȡ�ļ����� 
    	FILE *fp = fopen(file_name, "r"); 
	
		    if(NULL == fp) 
			{ 
		      printf("File:%s Not Found\n", file_name); 
		    } 
		    else
			{ 
			
		      bzero(buffer, BUFFER_SIZE); 
		      int length = 0; 
			  
		      // ÿ��ȡһ�����ݣ��㽫�䷢�͸��ͻ��ˣ�ѭ��ֱ���ļ�����Ϊֹ 
		      while((length = fread(buffer, sizeof(char), BUFFER_SIZE, fp)) > 0) 
		      { 
			    if(send(new_server_socket_fd, buffer, length, 0) < 0) 
			        { 
				          printf("Send File:%s Failed./n", file_name); 
				          break; 
			        } 
		        bzero(buffer, BUFFER_SIZE); 
		      } 
		  
		      // �ر��ļ� 
		      fclose(fp); 
		      printf("File:%s Transfer Successful!\n", F_PATH);
		   	} 
	    	
	 }
	//���տͻ��˷��������ݣ����յ�����recv����
	if(strcmp(command,commandchar_recv)==0)
	{
			bzero(command, Command_Max_Size+1);
			
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
		      // �ӷ������������ݵ�buffer�� 
  			  // ÿ����һ�����ݣ��㽫��д���ļ��У�ѭ��ֱ���ļ������겢д��Ϊֹ
		      while((length = recv(new_server_socket_fd, buffer, BUFFER_SIZE, 0)) > 0) 
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
	   
	} 
  	}
	// �ر���ͻ��˵����� 
 	close(new_server_socket_fd); 
 }
 
 
 // �رռ����õ�socket 
 close(server_socket_fd); 
 return 0; 

}



