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
	 char * ptr=(char *)arg;
	 int len = sizeof(client_addr);
	 ptr="huangwei";

	 //snprintf(ptr,sizeof(ptr),"%d", new_server_socket_fd);
	  
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
	
	//sleep(500);

	//while(getpeername (new_server_socket_fd, (struct sockaddr *)&client_addr,&len) < 0 && errno == ENOTCONN){

	int keepAlive = 1; // 开启keepalive属性
	int keepIdle = 60; // 如该连接在60秒内没有任何数据往来,则进行探测 
	int keepInterval = 5; // 探测时发包的时间间隔为5 秒
	int keepCount = 3; // 探测尝试的次数.如果第1次探测包就收到响应了,则后2次的不再发.

	setsockopt(new_server_socket_fd, SOL_SOCKET, SO_KEEPALIVE, (void *)&keepAlive, sizeof(keepAlive));
	setsockopt(new_server_socket_fd, SOL_TCP, TCP_KEEPIDLE, (void*)&keepIdle, sizeof(keepIdle));
	setsockopt(new_server_socket_fd, SOL_TCP, TCP_KEEPINTVL, (void *)&keepInterval, sizeof(keepInterval));
	setsockopt(new_server_socket_fd, SOL_TCP, TCP_KEEPCNT, (void *)&keepCount, sizeof(keepCount));

	char recvbuffer[100];
	int retval;
	char *namechar;
	char macchar[31];
	while(1){
	retval = recv( new_server_socket_fd,(char *)recvbuffer,sizeof(recvbuffer),0 ); 
	printf ("what send from client :%s \n ",recvbuffer);



	if(retval>0){

			//将名字作为数据库健值提出数据库中存储的mac地址进行比较，相同则可以进行
		    /*int charcount;
			int namecount;
			int maccount;
			for(charcount=0;charcount<(retval+1);charcount++)
			  {
			  printf("%c\n",recvbuffer[charcount]);
				if(recvbuffer[charcount]=="#"){
					    namecount=charcount;
						
						break;
					}
				if(recvbuffer[charcount]=="\0"){
					   // printf("%s\n",recvbuffer[charcount]);
					    maccount=charcount;
						break;
					}
			  }
			  */
			void rep(char *s,char *s1,char *s2)
			{
			  char *p;
			  for(;*s;s++) /*顺序访问字符串s中的每个字符*/
			  {
			    for(p=s1;*p&&*p!=*s;p++);/*检查当前字符是否在字符串s1中出现*/
			      if(*p)
			        *s=*(p-s1+s2); /*当前字符在字符串s1中出现,用字符串s2中的对应字符代替s中的字符*/
			  }
			}
            
			//namechar=strstr(recvbuffer, "Client Name:");
			snprintf(macchar,sizeof(macchar),"%s", recvbuffer);
			printf ("macchar from recvbuffer half:%s \n ",macchar);

            char s1[1]=":";
			char s2[1]=";";
			rep(macchar,s1,s2);
			
		
			namechar=strstr(recvbuffer, "Client Name");
			
			if(namechar)
       			 printf("namechar from recvbuffer half:%s \n ",namechar);
    		else
      			 printf("NotFound!\n");
			
			
			

			//char macchar[maccount];
			//char namechar[namecount];
			
		    //char *pnamechar=&recvbuffer[charcount];
			
			//snprintf(macchar,sizeof(macchar),"%s", recvbuffer);
			//snprintf(namechar,sizeof(namechar),"%s", pnamechar);

			ptr=macchar;
			
			char *testname;
				printf("macchar@%s\nnamechar@%s\nptr@%s",macchar,namechar,ptr);

			if (db_store(db, ptr, namechar, DB_STORE) != 0) err_quit("db_store error for alpha");

			

			if((testname=db_fetch(db,ptr))==NULL){
				int time_min_start;
				//char stingname;

				time_s=tmstruct->tm_sec;			 	
				time_m=tmstruct->tm_min;
				time_h=tmstruct->tm_hour;

				time_min_start=time_h*3600+time_m*60+time_s;

				  //snprintf(stingname,sizeof(stingname),"%d", time_min_start);
			      if (db_store(db, ptr, namechar, DB_STORE) != 0) err_quit("db_store error for alpha");
				  }
			else{
					int testfinale;
				  	if(testfinale=strcmp(testname,namechar)==0){
						printf("the name was expected with the mac address\n");
				 	   }
				 	 else{
				  	   char noexpectedmac="your mac was not expected name ";
			           send(new_server_socket_fd,  noexpectedmac,    sizeof(noexpectedmac),0);
					   exit (EXIT_FAILURE);
				  	  }
					}
	}
        if( retval<0 ) 
        { 
               
            if( errno == ETIMEDOUT ) 
            { 
                    /* 
                     * on keepalive mode and no timed-out select, 
                     * 2 hours later,recv timed out 
                     */ 
					 lt=time(NULL);
					 tmstruct=localtime(&lt);
					 printf("time_s: %d\ntime_m: %d\n time_h:%d\n",time_s,time_m,time_h);
					
                     time_s=tmstruct->tm_sec-time_s;
					 time_m=tmstruct->tm_min-time_m;
	  				 time_h=tmstruct->tm_hour-time_h;
		
					time_min_end=time_h*3600+time_m*60+time_s;
		

					//time_min_end=((const int)*db_fetch(db, ptr))+time_min_end;

					
					char string[25];
					sprintf(string,"%s",time_min_end);
					printf("time_min_end:::%s\n",string);
					
		 
					if (db_store(db, ptr, "3000", DB_STORE) != 0)
					err_quit("db_store error for alpha");
                    close(new_server_socket_fd); 
                    return(-2); 
            } 
            else 
            { 
                    close(new_server_socket_fd); 
                    return(-3); 
            } 
        } 
        else if ( retval == 0 ) /* disconnected */ 
        { 
                 printf("IMRecv: client close socket by recvn...\n"); 


				 
				 struct tm *tmstruct2;
				 time_t lt2;
				 
				 lt2=time(NULL);
				 
				 tmstruct2=localtime(&lt2);
				 
				 printf("time_s: %d\ntime_m: %d\n time_h:%d\n",tmstruct2->tm_sec,tmstruct2->tm_min,tmstruct2->tm_hour);
				 
			     time_s=tmstruct2->tm_sec-time_s;
				 time_m=tmstruct2->tm_min-time_m;
  				 time_h=tmstruct2->tm_hour-time_h;
		
				 time_min_end=time_h*3600+time_m*60+time_s;
				 
				 printf("this time time_min_end:::%d\n",time_min_end);

				 char *pdbfetch,*pdbfetch1,*pdbfetch2,*pdbfetch3,*pdbfetch4;
				
				 
				 int dbint;
				/* if (db_store(db, "4hao", "4hao", DB_STORE) != 0)
				 err_quit("db_store error for alpha");
				 if (db_store(db, "1hao", "1hao", DB_STORE) != 0)
				 err_quit("db_store error for alpha");
				 if (db_store(db, "2hao", "2hao", DB_STORE) != 0)
				 err_quit("db_store error for alpha");
				 if (db_store(db, "3hao", "3hao", DB_STORE) != 0)
				 err_quit("db_store error for alpha");
				 if (db_store(db, "5hao", "5hao", DB_STORE) != 0)
				 err_quit("db_store error for alpha");
				 */

				//  if (db_store(db, ptr, "3000", DB_INSERT) != 0)
				// err_quit("db_store error for alpha");

				/* printf("dbstore ok\n");
					 
				 pdbfetch4=db_fetch(db,"4hao");
				 printf("pdbfetch:::%s\n",pdbfetch4);
				 pdbfetch1=db_fetch(db,"2hao");
				 printf("pdbfetch:::%s\n",pdbfetch1);
				 pdbfetch2=db_fetch(db,"1hao");
				 printf("pdbfetch:::%s\n",pdbfetch2);
				 pdbfetch3=db_fetch(db,"3hao");
				 printf("pdbfetch:::%s\n",pdbfetch3);
				

				 printf("fetch ok\n");
	

				
				 printf("pdbfetch:::%s\n",pdbfetch1);
	             printf("pdbfetch:::%s\n",pdbfetch2);
				 printf("pdbfetch:::%s\n",pdbfetch3);
				 printf("pdbfetch:::%s\n",pdbfetch4);

				 printf("sssprintf ok\n");
				 */
				
				 pdbfetch=db_fetch(db,namechar);
				 
				 if(pdbfetch==NULL){
				 	dbint=0;
					printf("dbfetch null");
				 	}
				 else {

					printf("last time %s\n",pdbfetch);
				 	dbint=atoi(pdbfetch);
				 	}
				 time_min_end=time_min_end+dbint;
			
		

					//time_min_end=((const int)*db_fetch(db, ptr))+time_min_end;

					
					char string[10]={0};
 				    int nLen=snprintf(string,sizeof(string),"%d", time_min_end);
					if(nLen<0)printf("snprintf error");
					
					
					printf("string=time_min_end:::%s\n",string);
					
		 
					if (db_store(db, namechar,string, DB_STORE) != 0)
					err_quit("db_store error for alpha");
                    close(new_server_socket_fd); 
                    return(-2); 
        } 
        else if ( recvbuffer==1 )  /* recv instruction start */ 
        { 
                printf(" instruction: start time");                 
                return(-4); 
        } 
		else if ( recvbuffer==2 )  /* recv instruction end*/ 
        { 
                printf(" instruction: end time"); 
				
			 	time_s=tmstruct->tm_sec-time_s;			 	
				time_m=tmstruct->tm_min-time_m;
				time_h=tmstruct->tm_hour-time_h;

				time_min_end=time_h*3600+time_m*60+time_s;

				//time_min_end=((const int)*db_fetch(db, ptr))+time_min_end;

				
				char string[25];
				sprintf(string,"%d",time_min_end);
	 
				if (db_store(db, ptr, string, DB_INSERT) != 0)
				err_quit("db_store error for alpha");
                close(new_server_socket_fd); 
                return(-4); 
        } 
	}

 db_close(db);
	
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
					 //sprintf(peeraddrstr, "Client Address: %s\n", inet_ntop(AF_INET, &client_addr.sin_addr, peerip, sizeof(peerip)));
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
