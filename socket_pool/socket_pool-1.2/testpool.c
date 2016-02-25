#include<netinet/in.h> // sockaddr_in 
#include<sys/types.h>  // socket 
#include<sys/socket.h> // socket 
#include<stdio.h>    // printf 
#include<stdlib.h>   // exit 
#include<string.h>   // bzero 
#include <time.h>    //time
#include <string.h>   //string 

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



void thread(void *arg)
{
	  char * ptr=(char *)arg;
	  
	  sleep(1);

	  FILE *datafp = NULL;;
	  datafp=fopen("student.txt", "w+");
	  fwrite(ptr,1,20,datafp);
	  fclose(datafp);

	  DBHANDLE	db;

	if ((db = db_open("db4", O_RDWR | O_CREAT | O_TRUNC,
	  FILE_MODE)) == NULL)
		err_sys("db_open error");

	if (db_store(db, "Alpha", "data1", DB_INSERT) != 0)
		err_quit("db_store error for alpha");
	if (db_store(db, "beta", "Data for beta", DB_INSERT) != 0)
		err_quit("db_store error for beta");
	if (db_store(db, "gamma", "record3", DB_INSERT) != 0)
		err_quit("db_store error for gamma");

	db_close(db);
	exit(0);

	  
	  //printf("hello world! %s\n",ptr);
}

int main(int argc, char *argv[])
{
	 
	    tpool_t *pool;  
	  
	    
	    logmy=log_openlog("test.log", 0);
		
	   
	    pool=tpool_init(10,20,1);

		
		socket_branch();

		int  Construction;
   
    while(1)
     	{

		
         

			
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
    			 

		
         struct sockaddr_in client_addr;
		 int client_addr_length =sizeof(client_addr);

 		 int new_server_socket_fd =accept(server_socket_fd,(struct sockaddr*)&client_addr, &client_addr_length);
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
					 sprintf(peeraddrstr, "time: %s\npeer address: %s:%d\n\n",ctime(&timep) , inet_ntop(AF_INET, &client_addr.sin_addr, peerip, sizeof(peerip)), ntohs(client_addr.sin_port));
	                 printf("%s\n", peeraddrstr);
				 }
		 	 	
				tpool_add_work(pool,thread,peeraddrstr);
		 	}
     	}
     
	

	sleep(5);
	
	tpool_destroy(pool,1);
	log_close(logmy);
	pthread_exit(NULL);
}
