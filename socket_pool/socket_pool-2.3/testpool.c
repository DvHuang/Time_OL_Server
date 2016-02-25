#include<netinet/in.h> // sockaddr_in 
#include<sys/types.h>  // socket 
#include<sys/socket.h> // socket 
#include<stdio.h>    // printf 
#include<stdlib.h>   // exit 
#include<string.h>   // bzero 
#include <time.h>    //time
#include <string.h>   //string 

#include <netinet/tcp.h>

#include <errno.h>

#include "apue.h"
#include "apue_db.h"
#include <fcntl.h>

#include <arpa/inet.h>

#include <getopt.h> //getopt_long

#include <pthread.h>

#include "log.h"
#include "tpool.h"
#include "socket.h"




log_t *logmy;
DBHANDLE	db;


void thread(void *arg,int new_server_socket_fd,struct sockaddr_in client_addr)
{
	 char *ptr=(char *)arg;
	 int len = sizeof(client_addr);
	 char *pdbfetch,*pdbfetch1,*pdbfetch2,*pdbfetch3,*pdbfetch4;
 
	 sleep(1);


	 struct tm *tmstruct;
	 time_t lt;
	 lt=time(NULL);
	 
	 tmstruct=localtime(&lt);

	 int time_s,time_m,time_h,time_min_end;
	 
	 time_s=tmstruct->tm_sec;
   	 time_m=tmstruct->tm_min;
	 time_h=tmstruct->tm_hour;
	 
	 printf ("new_server_socket_fd:%d \n ",new_server_socket_fd);
	 printf("time_s: %d\ntime_m: %d\n time_h:%d\n",time_s,time_m,time_h);
	
	int keepAlive = 1; // ¿ªÆôkeepaliveÊôÐÔ
	int keepIdle = 60; // Èç¸ÃÁ¬½ÓÔÚ60ÃëÄÚÃ»ÓÐÈÎºÎÊý¾ÝÍùÀ´,Ôò½øÐÐÌ½²â 
	int keepInterval = 5; // Ì½²âÊ±·¢°üµÄÊ±¼ä¼ä¸ôÎª5 Ãë
	int keepCount = 3; // Ì½²â³¢ÊÔµÄ´ÎÊý.Èç¹ûµÚ1´ÎÌ½²â°ü¾ÍÊÕµ½ÏìÓ¦ÁË,Ôòºó2´ÎµÄ²»ÔÙ·¢.

	setsockopt(new_server_socket_fd, SOL_SOCKET, SO_KEEPALIVE, (void *)&keepAlive, sizeof(keepAlive));
	setsockopt(new_server_socket_fd, SOL_TCP, TCP_KEEPIDLE, (void*)&keepIdle, sizeof(keepIdle));
	setsockopt(new_server_socket_fd, SOL_TCP, TCP_KEEPINTVL, (void *)&keepInterval, sizeof(keepInterval));
	setsockopt(new_server_socket_fd, SOL_TCP, TCP_KEEPCNT, (void *)&keepCount, sizeof(keepCount));


	char recvbuffer[100];
	int retval;
	char namechar[50];
	char macchar[50];

	char str_send[100];
	int  str_send_sizeof;


	
	while(1){
	retval = recv( new_server_socket_fd,(char *)recvbuffer,sizeof(recvbuffer),0 ); 

	if(retval>0){

			void rep(char *s,char *s1,char *s2)
			{
			  char *p;
			  for(;*s;s++) /*Ë³Ðò·ÃÎÊ×Ö·û´®sÖÐµÄÃ¿¸ö×Ö·û*/
			  {
			    for(p=s1;*p&&*p!=*s;p++);/*¼ì²éµ±Ç°×Ö·ûÊÇ·ñÔÚ×Ö·û´®s1ÖÐ³öÏÖ*/
			      if(*p)
			        *s=*(p-s1+s2); /*µ±Ç°×Ö·ûÔÚ×Ö·û´®s1ÖÐ³öÏÖ,ÓÃ×Ö·û´®s2ÖÐµÄ¶ÔÓ¦×Ö·û´úÌæsÖÐµÄ×Ö·û*/
			  }
			}

			printf("recvbuffer=%s\n",recvbuffer);
			
			//strº¯Êý²»°²È«
			if(recvbuffer!=NULL){

			 //»ñµÃÓÃ»§Ãû
			 char *pname;
			 int iname=0;
			 pname=strstr(recvbuffer,"@");
			 while (*(++pname)!='#')
			    {
			        *(namechar+iname)=*pname;
			        ++iname;
			    }
			 *(namechar+iname)='\0';
			 
			 *pname=0;
			 iname=0;
			 
			printf ("the namechar:%s \n ",namechar);
			
            //»ñµÃmac
			 char *pmac;
			 int imac=0;
			 pmac=strstr(recvbuffer,"!");
			 while (*(++pmac)!='@')
			    {
			        *(macchar+imac)=*pmac;
			        ++imac;
			    }
			 *(macchar+imac)='\0';
			 *pmac=0;
			 imac=0;
			 			 
			printf ("the macaddress:%s \n ",macchar);
			}
			   
            //ÇÐ»»
			char s1[1]=":";
			char s2[1]="-";
			rep(macchar,s1,s2);
			printf ("the macaddress:%s \n ",macchar);
		
			
			int time_min_start;
			time_s=tmstruct->tm_sec;			 	
			time_m=tmstruct->tm_min;
			time_h=tmstruct->tm_hour;
			time_min_start=time_h*3600+time_m*60+time_s;
			
			char *testname;	
			
            //ÐÂÓÃ»§¿
			if((testname=db_fetch(db,macchar))==NULL){
				//ÐÂÓÃ»§
				printf("the client was a new client,make a new ide\n");
				printf("macchar@%s\nnamechar@%s\n",macchar,namechar);

				
				
				if (db_store(db, macchar, namechar, DB_STORE) != 0) err_quit("db_store error for alpha");
				printf("ide macchar is ok\n");	
							
				if (db_store(db, namechar, "0", DB_STORE) != 0)err_quit("db_store error for alpha");
				printf("ide namechar is ok\n");

				str_send_sizeof=snprintf(str_send,sizeof(str_send),"user name -> ! %s@  Time Online -> # 00$:^ 00&\r\n",namechar);
				if(send(new_server_socket_fd,str_send,sizeof(str_send), 0)==-1){
								      perror("send fail");
									  exit(0);
								}
				

			}
			else{
				//ÀÏÓÃ»§
				
				    printf("testname@%s\n",testname);
					//ÅÐ¶ÏÓÃ»§ÃûÓëmacÊÇ·ñÆ¥Åä
				  	if(strcmp(testname,namechar)==0)
					{
					//ºÏ·¨
						printf("the name was expected with the mac address\n");

						if((pdbfetch=db_fetch(db,namechar))==NULL){
							perror("old user db_fetch error");
						}
							 
								

						//µÃµ½ÀÏÓÃ»§ÉÏ´ÎÀÛ¼ÆÊ±¼ä 
						int N,hour,fen;
						N=atoi(pdbfetch);
						hour=N/3600;
						fen=(N%3600)/60;
						
						if(hour>=10&&fen>=10){
							str_send_sizeof=snprintf(str_send,sizeof(str_send),"user name -> ! %s@  Time Online (H:Min)-># %d$:^ %d&\r\n",namechar,hour,fen);  
							printf("the client's name %s -> Time Online(H:Min):%d:%d\r\n",namechar,hour,fen);
							 //¿Í»§¶Ëack
							if(send(new_server_socket_fd,str_send,str_send_sizeof, 0)==-1){
									      perror("send fail");
										  exit(0);
										}
						}
						else if(hour>=10&&fen<=10){
							str_send_sizeof=snprintf(str_send,sizeof(str_send),"user name -> ! %s@  Time Online (H:Min)-># %d$:^ 0%d&\r\n",namechar,hour,fen);
							printf("the client's name %s -> Time Online(H:Min):%d:0%d\r\n",namechar,hour,fen);
							 //¿Í»§¶Ëack
							if(send(new_server_socket_fd,str_send,str_send_sizeof, 0)==-1){
									      perror("send fail");
										  exit(0);
										}
						}
						else if(hour<=10&&fen<=10){
							str_send_sizeof=snprintf(str_send,sizeof(str_send),"user name -> ! %s@  Time Online (H:Min)-># 0%d$:^ 0%d&\r\n",namechar,hour,fen);
							printf("the client's name %s -> Time Online(H:Min):0%d:0%d\r\n",namechar,hour,fen);
							 //¿Í»§¶Ëack
							if(send(new_server_socket_fd,str_send,str_send_sizeof, 0)==-1){
									      perror("send fail");
										  exit(0);
										}
						}
						else if(hour<=10&&fen>=10){
							str_send_sizeof=snprintf(str_send,sizeof(str_send),"user name -> ! %s@  Time Online (H:Min)-># 0%d$:^ %d&\r\n",namechar,hour,fen);					
							printf("the client's name %s -> Time Online(H:Min):0%d:%d\r\n",namechar,hour,fen);
							 //¿Í»§¶Ëack
							if(send(new_server_socket_fd,str_send,str_send_sizeof, 0)==-1){
									      perror("send fail");
										  exit(0);
										}
						}
						
					    
					   
					
				 	}
					else
					{
					//²»ºÏ·¨
					
						  printf("your mac was not expected mac\r\n");
						  if(send(new_server_socket_fd,"your mac was not expected mac\r\n",sizeof("your mac was not expected mac\r\n"),0)==-1)
						  {
						   perror("send fail");
						   exit (EXIT_FAILURE);
					  	  }
						  return;
					}
			}
   }

 
   else{ 
             printf("IMRecv: client close socket by recvn...\n"); 
			 
			 struct tm *tmstruct2;
			 time_t lt2;
			 
			 lt2=time(NULL);
			 
			 tmstruct2=localtime(&lt2);
			 
		     time_s=tmstruct2->tm_sec-time_s;
			 time_m=tmstruct2->tm_min-time_m;
				 time_h=tmstruct2->tm_hour-time_h;
	
			 time_min_end=time_h*3600+time_m*60+time_s;
	 
			 int dbint;		
			 
			 if((pdbfetch=db_fetch(db,namechar))==NULL){
			 	dbint=0;
				printf("dbfetch null\n");
			 	}
			 else {
				printf("last time %s\n",pdbfetch);
			 	dbint=atoi(pdbfetch);
			 	}
			 time_min_end=time_min_end+dbint;
		

			char string[10]={0};
			int nLen;
			if((nLen=snprintf(string,sizeof(string),"%d", time_min_end))<0)
			printf("snprintf error");
			
			printf("this time the client:%s 's total time is %s",namechar,string);

			if (db_store(db, namechar,string, DB_STORE) != 0)
			err_quit("db_store error for alpha");
			
			
            close(new_server_socket_fd); 
            return; 
				
    } 
 
 }

 db_close(db);
 return;
	
}

int main(int argc, char *argv[])
{
	 
	    tpool_t *pool;      
	    logmy=log_openlog("test.log", 0);   
	    pool=tpool_init(10,20,1);	
		socket_branch();
		int  Construction;
		
		if ((db = db_open("db4", O_RDWR ,FILE_MODE)) == NULL)
		err_sys("db_open error");		
		printf("Please Input Construction number On Server:\t"); 
		
		scanf("%d", &Construction); 
		printf("%d\n", Construction);
			
        switch (Construction) {
        

        case 9:
            	tpool_destroy(pool,1);
				log_close(logmy);
				pthread_exit(NULL);
				exit (EXIT_SUCCESS);

        case 8:
                exit (EXIT_FAILURE);
				
	    case 6:
                break;

        default:
                break;
        }
   
    while(1)
     	{
     	
         struct sockaddr_in client_addr;
		 int client_addr_length =sizeof(client_addr);
 		 int new_server_socket_fd =accept(server_socket_fd,(struct sockaddr*)&client_addr, &client_addr_length);
		 printf ("new_server_socket_fd:%d \n ",new_server_socket_fd);
		
		 if(new_server_socket_fd < 0)
		 	{
				perror("Server Accept Failed:");
				break;
		    }
		 else
		 	{   

                printf("Accept Client Socket Address\n");
				char peeraddrstr[60];
			    char peerip[18];
			    
			    time_t timep;
				time(&timep);
								
				int len = sizeof(client_addr);
				if(!getpeername(new_server_socket_fd, (struct sockaddr *)&client_addr, &len))
				 {
					 sprintf(peeraddrstr, "%s", inet_ntop(AF_INET, &client_addr.sin_addr, peerip, sizeof(peerip)));
	                 printf("%s\n", peeraddrstr);
				 }
		 	 	
				tpool_add_work(pool,thread,peeraddrstr,new_server_socket_fd,client_addr);
		 	}
     	}
     
	

	sleep(5);
	
	tpool_destroy(pool,1);
	
	log_close(logmy);
	
	pthread_exit(NULL);
}
