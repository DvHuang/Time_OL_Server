#include <netinet/in.h>    // for sockaddr_in
#include <sys/types.h>    // for socket
#include <sys/socket.h>    // for socket
#include <stdio.h>        // for printf
#include <stdlib.h>        // for exit
#include <string.h>        // for bzero
/*
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
*/
#define HELLO_WORLD_SERVER_PORT    30000 
#define LENGTH_OF_LISTEN_QUEUE 20
#define BUFFER_SIZE 1024
#define FILE_NAME_MAX_SIZE 512

#define file_name "/home/samba/service_exe/camera_data"
 
int main(int argc, char **argv)
{
    //����һ��socket��ַ�ṹserver_addr,���������internet��ַ, �˿�
    struct sockaddr_in server_addr;
    bzero(&server_addr,sizeof(server_addr)); //��һ���ڴ���������ȫ������Ϊ0
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = htons(INADDR_ANY);
    server_addr.sin_port = htons(HELLO_WORLD_SERVER_PORT);
 
    //��������internet����Э��(TCP)socket,��server_socket���������socket
    int server_socket = socket(PF_INET,SOCK_STREAM,0);
    if( server_socket < 0)
    {
        printf("Create Socket Failed!");
        exit(1);
    }
{ 
   int opt =1;
   setsockopt(server_socket,SOL_SOCKET,SO_REUSEADDR,&opt,sizeof(opt));
}
     
    //��socket��socket��ַ�ṹ��ϵ����
    if( bind(server_socket,(struct sockaddr*)&server_addr,sizeof(server_addr)))
    {
        printf("Server Bind Port : %d Failed!", HELLO_WORLD_SERVER_PORT); 
        exit(1);
    }
 
    //server_socket���ڼ���
    if ( listen(server_socket, LENGTH_OF_LISTEN_QUEUE) )
    {
        printf("Server Listen Failed!"); 
        exit(1);
    }
    while (1) //��������Ҫһֱ����
    {
        //����ͻ��˵�socket��ַ�ṹclient_addr
        struct sockaddr_in client_addr;
        socklen_t length_addr = sizeof(BUFFER_SIZE);
 
        //����һ����server_socket�����socket��һ������
        //���û����������,�͵ȴ�������������--����accept����������
        //accept��������һ���µ�socket,���socket(new_server_socket)����ͬ���ӵ��Ŀͻ���ͨ��
        //new_server_socket�����˷������Ϳͻ���֮���һ��ͨ��ͨ��
        //accept���������ӵ��Ŀͻ�����Ϣ��д���ͻ��˵�socket��ַ�ṹclient_addr��
        int new_server_socket = accept(server_socket,(struct sockaddr*)&client_addr,&length_addr);
        if ( new_server_socket < 0)
        {
            printf("Server Accept Failed!\n");
            break;
        }
         
        char buffer[BUFFER_SIZE];
        bzero(buffer, BUFFER_SIZE);
       


        FILE * fp = fopen(file_name,"w");
        if(NULL == fp )
        {
            printf("File:\t%s Not Found\n", file_name);
        }

    int length = 0;
    while( length = read(new_server_socket,buffer,BUFFER_SIZE,0))
    {
        
       printf("buffer0=%d\n",buffer);

       if(length < 0)
        {
            printf("Recieve Data From Server %s Failed!\n", argv[1]);
            break;
        }
//        int write_length = write(fp, buffer,length);
        int write_length = fwrite(buffer,sizeof(char),length,fp);
        if (write_length<length)
        {
            printf("File:\t%s Write Failed\n", file_name);
            break;
        }
        bzero(buffer,BUFFER_SIZE);    
    }
    printf("Recieve File:\t %s From Server[%s] Finished\n",file_name, argv[1]);
     
        
        //�ر���ͻ��˵�����
        close(new_server_socket);
    }
    //�رռ����õ�socket
    close(server_socket);
    return 0;
}
