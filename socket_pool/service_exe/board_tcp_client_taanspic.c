/* this program is improved in the program board_tcp_client ,realizes the 
 * function of send camera caches data to server
 * by davy huang 2015/03/29
 */




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
//#define BUFFER_SIZE 1024 
#define BUFFER_SIZE 320*240*2  //meet the camera size
#define FILE_NAME_MAX_SIZE 512



/*for the camera*/

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include <sys/mman.h>
#define Frame_Size  320*240*2   
 
int main(int argc, char **argv)
{


    int cam_fd; //config a fd for camera device
    char image[320*240*2]; // for camera
//    cam_fd =open ("/dev/camera",O_RDWR);
  //    if(cam_fd<0){
    //           perror("camera.\n");
      //         return -1;
      //   }


    if (argc != 2)
    {
        printf("Usage: ./%s ServerIPAddress\n",argv[0]);
        exit(1);
    }
 
     cam_fd =open ("/dev/camera",O_RDWR);
      if(cam_fd<0){
               perror("camera.\n");
               return -1;
         }

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
   // setsockopt(client_socket,SOL_SOCKET,SO_SNDBUF,(const char*)&Frame_Size,sizeof(int)); 
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
 
   // char file_name[FILE_NAME_MAX_SIZE+1];
   // bzero(file_name, FILE_NAME_MAX_SIZE+1);
  //  printf("Please Input File Name On Server:\t");
   // scanf("%s", file_name);
// while(1) {
  //  printf("Please Input File Name On Server:\t");
   // scanf("%s", file_name);
 //  if
   
    char buffer[BUFFER_SIZE];
  //  bzero(buffer,BUFFER_SIZE);
    read(cam_fd,image,Frame_Size);//read camera data put in buffer
  //  strncpy(buffer, file_name, strlen(file_name)>BUFFER_SIZE?BUFFER_SIZE:strlen(file_name));
    //�����������buffer�е�����
//  send(client_socket,buffer,BUFFER_SIZE,0);
  char image2[]="nihao";
    
printf("%d\n",image[100]);
   
    strncpy(buffer, image, strlen(image)>BUFFER_SIZE?BUFFER_SIZE:strlen(image));
    printf("%s\n",image2);
    printf("%d\n",buffer[100]);
    send(client_socket,image2,sizeof(buffer),0);
   // send(client_socket,buffer,sizeof(buffer),0);
//    int fp = open(file_name, O_WRONLY|O_CREAT);
//    if( fp < 0 )
 //   FILE * fp = fopen(file_name,"r");
 //   if(NULL == fp )
 //   {
   //     printf("File:\t%s Can Not Open To Write\n", file_name);
   //     exit(1);
   // }
    //  else
    //    {
         //   bzero(buffer, BUFFER_SIZE);
         //   int file_block_length = 0;
         //   while( (file_block_length = fread(buffer,sizeof(char),BUFFER_SIZE,fp))>0)
         //   {
         //       printf("file_block_length = %d\n",file_block_length);
         //       if(send(new_server_socket,buffer,file_block_length,0)<0)
         //       {
         //           printf("Send File:\t%s Failed\n", file_name);
         //           break;
         //       }
         //       bzero(buffer, BUFFER_SIZE);
         //   }
         //   fclose(fp);
         //   printf("File:\t%s Transfer Finished\n",file_name);
      //  }

    //�ӷ������������ݵ�buffer��
//    bzero(buffer,BUFFER_SIZE);
  //  int length = 0;
   // while( length = recv(client_socket,buffer,BUFFER_SIZE,0))
   // {
     //   if(length < 0)
      //  {
       //     printf("Recieve Data From Server %s Failed!\n", argv[1]);
        //    break;
       // }
// //       int write_length = write(fp, buffer,length);
       // int write_length = fwrite(buffer,sizeof(char),length,fp);
       // if (write_length<length)
     //   {
       //     printf("File:\t%s Write Failed\n", file_name);
        //    break;
       // }
      //  bzero(buffer,BUFFER_SIZE);    
   // }
   // printf("Recieve File:\t %s From Server[%s] Finished\n",file_name, argv[1]);
    close(cam_fd); 
   // close(fp);
    //�ر�socket
    close(client_socket);
    return 0;
}
