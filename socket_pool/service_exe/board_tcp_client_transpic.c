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

#include <fcntl.h> 
#define HELLO_WORLD_SERVER_PORT    30000 
#define BUFFER_SIZE 1024
#define FILE_NAME_MAX_SIZE 512
 
int main(int argc, char **argv)
{
    if (argc != 2)
    {
        printf("Usage: ./%s ServerIPAddress\n",argv[0]);
        exit(1);
    }
   printf("confirm2");
   int cam_fd; 
   cam_fd=open("/dev/camera",O_RDWR);
   if(cam_fd<0) printf("camera can't open");
    //����һ��socket��ַ�ṹclient_addr,����ͻ���internet��ַ, �˿�
    struct sockaddr_in client_addr;
    bzero(&client_addr,sizeof(client_addr)); //��һ���ڴ���������ȫ������Ϊ0
    client_addr.sin_family = AF_INET;    //internetЭ����
    client_addr.sin_addr.s_addr = htons(INADDR_ANY);//INADDR_ANY��ʾ�Զ���ȡ������ַ
    client_addr.sin_port = htons(0);    //0��ʾ��ϵͳ�Զ�����һ�����ж˿�
    //��������internet����Э��(TCP)socket,��client_socket����ͻ���socket
    int client_socket = socket(AF_INET,SOCK_STREAM,0);
    if( client_socket < 0)
    {
        printf("Create Socket Failed!\n");
        exit(1);
    }
    //�ѿͻ�����socket�Ϳͻ�����socket��ַ�ṹ��ϵ����
    if( bind(client_socket,(struct sockaddr*)&client_addr,sizeof(client_addr)))
    {
        printf("Client Bind Port Failed!\n"); 
        exit(1);
    }
 
    //����һ��socket��ַ�ṹserver_addr,�����������internet��ַ, �˿�
    struct sockaddr_in server_addr;
    bzero(&server_addr,sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    if(inet_aton(argv[1],&server_addr.sin_addr) == 0) //��������IP��ַ���Գ���Ĳ���
    {
        printf("Server IP Address Error!\n");
        exit(1);
    }
    server_addr.sin_port = htons(HELLO_WORLD_SERVER_PORT);
    socklen_t server_addr_length = sizeof(server_addr);
    //���������������,���ӳɹ���client_socket�����˿ͻ����ͷ�������һ��socket����
    if(connect(client_socket,(struct sockaddr*)&server_addr, server_addr_length) < 0)
    {
        printf("Can Not Connect To %s!\n",argv[1]);
        exit(1);
    }
 
    
    
    char buffer[BUFFER_SIZE]="2081987878898979868769756767";
    bzero(buffer,BUFFER_SIZE);
    //�����������buffer�е�����
//    read(cam_fd,buffer,BUFFER_SIZE);
    send(client_socket,buffer,BUFFER_SIZE,0);
 
//    int fp = open(file_name, O_WRONLY|O_CREAT);
//    if( fp < 0 )
    
     

    close(cam_fd);
    //�ر�socket
    close(client_socket);
    return 0;






}	
