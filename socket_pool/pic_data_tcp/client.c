#include<stdio.h>
#include<stdlib.h>
#include<sys/socket.h>
#include<error.h>
#include<string.h>
#include<sys/types.h>
#include<netinet/in.h>
#include<arpa/inet.h>
#include <signal.h>
#include <time.h>
#include <sys/time.h>


#define UCHAR unsigned char

int  Ret;
char buff[65535]; 
int i=0;
int j=0;
int m=0;
int s;//创建套结字

void set_timer()
{
        struct itimerval itv, oldtv;
        itv.it_interval.tv_sec = 0;
        itv.it_interval.tv_usec = 50000;
        itv.it_value.tv_sec = 0;
        itv.it_value.tv_usec = 50000;

        setitimer(ITIMER_REAL, &itv, &oldtv);
}

void sigalrm_handler(int sig)
{
//        count++;
//        printf("timer signal.. %d\n", count);
	j++;
	printf("\n(%d)send:    ",j);
	Ret = send(s, buff, 8192, 0);
}

void main(int argc, char ** argv)
{

   struct sockaddr_in ServerAddr;
   int Port;//端口
   char     DataBuffer[65535];//接受数据缓冲区
   char ip[20]={0};
   FILE* fp;
   FILE* File;

 /*  if (argc < 3)
   {
      printf("运行方式： 应用程序 端口号 服务器IP\n");
      return;
   }

   Port = atoi(argv[1]);*/

   //////
;
   
   ///////////////read ini
  fp = fopen("config.ini", "r");
    if (fp == NULL)
    {
        printf("open file error！\n");
        fclose(fp);
        exit(0);
    }
    while (fscanf(fp, "%s\n", buff) == 1)
    {
        //printf("%s\n", buff);
        switch (buff[0])
        {
        case '#':
            break;
        case 'c':
            Port = atoi(buff + 4);
            break;
		case 'i':
            strcpy(ip, buff + 3);
            break;
        default:
            break;
        }
    }
    printf("COM=%d,IP=%s\n", Port,ip);
    fclose(fp);
//////////////
   //////

   s = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);//创建一个套节字

   ServerAddr.sin_family = AF_INET;
   ServerAddr.sin_port = htons(Port);    
   ServerAddr.sin_addr.s_addr = inet_addr(ip);//服务器地址IP

   printf("We are trying to connect to %s:%d...\n",
          inet_ntoa(ServerAddr.sin_addr), ntohs(ServerAddr.sin_port));

   if(connect(s, (struct sockaddr_in *) &ServerAddr, sizeof(ServerAddr)) == -1)//连接到服务器
   {
        printf("Connect failed!\n");
	return ;
   }
   else 
   {
	   printf("Our connection succeeded.\n");
	   printf("We will now try to send  messages.\n");


	   	memset(buff, 0, 65535);

		for(i=0;i<8192;i++)
			buff[i]=rand();
		buff[8192]=0;
	/*	File=fopen("send_client.dat","ab+");
		if(File)
		{
			fwrite(DataBuffer,8192,1,File);
		}
		fclose(File);
		*/
		signal(SIGALRM, sigalrm_handler);
		set_timer();

		while(1)
		{
			memset(DataBuffer, 0, sizeof(DataBuffer));
           if(Ret = recv(s, DataBuffer, sizeof(DataBuffer), 0), Ret >0)
           {
			   m++;
		     printf("(%d)recv \n", m);
		 /* 	 File=fopen("recv_client.dat","ab+");
			   if(File)
			   {
				   fwrite(DataBuffer,8192,1,File);
			   }
			   fclose(File);*/
		}
    }

   printf("We are closing the connection.\n");
   close(s);
}
}