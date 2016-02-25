/************************************************************************* 
 tpool.c 线程池的实现
************************************************************************/
  
#include  <stdio.h>
#include  <stdlib.h>
#include  <string.h> 
#include  <pthread.h>
#include<netinet/in.h> // sockaddr_in 
#include<sys/types.h>  // socket 
#include<sys/socket.h> // socket 


#include "tpool.h"
#include "log.h"


/* 工作线程 */
void *tpool_thread(void *tpool);
extern log_t *logmy;
 
/***************线程池初始化*****************************/
tpool_t *tpool_init(int num_worker_threads, /*线程池线程个数*/
            int max_queue_size,   /*最大任务数*/
      int do_not_block_when_full) /*是否阻塞任务满的时候*/
{
    int i, rtn;
    tpool_t *pool; 
    lprintf(logmy, INFO, "init pool begin ...\n");
    /* 创建线程池结构体 */
    if((pool = (struct tpool *)malloc(sizeof(struct tpool))) == NULL) 
    {
        lprintf(logmy, FATAL, "Unable to malloc() thread pool!\n");
        return NULL;
    }
    /* 设置线程池架构体成员 */
    pool->num_threads = num_worker_threads;                      /*工作线程个数*/
    pool->max_queue_size = max_queue_size;                      /*任务链表最大长度*/
    pool->do_not_block_when_full = do_not_block_when_full;      /*任务链表满时是否等待*/
    /* 生成线程池缓存 */
    if((pool->threads = (pthread_t *)malloc(sizeof(pthread_t)*num_worker_threads)) == NULL) 
    {
        lprintf(logmy, FATAL,"Unable to malloc() thread info array\n");
        return NULL; 
    }
    /* 初始化任务链表 */
    pool->cur_queue_size = 0;
    pool->queue_head = NULL;
    pool->queue_tail = NULL;
    pool->queue_closed = 0;
    pool->shutdown = 0;
    /* 初始化互斥变量，条件变量 用于线程之间的同步 */
    if((rtn = pthread_mutex_init(&(pool->queue_lock),NULL)) != 0) 
    {
        lprintf(logmy,FATAL,"pthread_mutex_init %s",strerror(rtn));
        return NULL;
    }
    if((rtn = pthread_cond_init(&(pool->queue_not_empty),NULL)) != 0) 
    {
        lprintf(logmy,FATAL,"pthread_cond_init %s",strerror(rtn));
        return NULL;
    }
    if((rtn = pthread_cond_init(&(pool->queue_not_full),NULL)) != 0) 
    {
        lprintf(logmy,FATAL,"pthread_cond_init %s",strerror(rtn));
        return NULL;
    }
    if((rtn = pthread_cond_init(&(pool->queue_empty),NULL)) != 0) 
    {
        lprintf(logmy,FATAL,"pthread_cond_init %s",strerror(rtn));
        return NULL;
    }
    /* 创建所有的线程 */
    for(i = 0; i != num_worker_threads; i++) 
    {
        if( (rtn=pthread_create(&(pool->threads[i]),NULL,tpool_thread,(void*)pool)) != 0) 
        {
            lprintf(logmy,FATAL,"pthread_create %s\n",strerror(rtn));
            return NULL; 
        }
        lprintf(logmy, INFO, "init pthread  %d!\n",i);
    }
    lprintf(logmy, INFO, "init pool end!\n");
    return pool;
}

int tpool_add_work(tpool_t *pool,           /*线程池指针*/
                    void (*routine)(void *),  /*工作线程函数指针*/
                    void *arg,
                    int new_server_socket_fd,
                    struct sockaddr_in client_addr)                  /*工作线程函数参数*/
{
    int rtn;
    tpool_work_t *workp; /*当前工作线程*/
    if((rtn = pthread_mutex_lock(&pool->queue_lock)) != 0)
    {
        lprintf(logmy,FATAL,"pthread mutex lock failure\n");
        return -1;
    }
    /* 采取独占的形式访问任务链表 */
    if((pool->cur_queue_size == pool->max_queue_size) && \
            (pool->do_not_block_when_full)) 
    {
        if((rtn = pthread_mutex_unlock(&pool->queue_lock)) != 0)
        {
            lprintf(logmy,FATAL,"pthread mutex lock failure\n");
            return -1;
        }
        return -1;
    }
    /* 等待任务链表为新线程释放空间 */
    while((pool->cur_queue_size == pool->max_queue_size) &&
            (!(pool->shutdown || pool->queue_closed))) 
  	{
        if((rtn = pthread_cond_wait(&(pool->queue_not_full),
                        &(pool->queue_lock)) ) != 0) 
        {
            lprintf(logmy,FATAL,"pthread cond wait failure\n");
            return -1;
        }
    }
    if(pool->shutdown || pool->queue_closed) 
    {
        if((rtn = pthread_mutex_unlock(&pool->queue_lock)) != 0)  
        {
            lprintf(logmy,FATAL,"pthread mutex lock failure\n");
            return -1;
        }
        return -1;
    }
    /* 分配工作线程结构体 */
    if((workp = (tpool_work_t *)malloc(sizeof(tpool_work_t))) == NULL) 
    {
        lprintf(logmy,FATAL,"unable to create work struct\n");
        return -1;
    }
    workp->handler_routine = routine;
    workp->arg = arg;
    workp->next = NULL;
	workp->server = new_server_socket_fd;
	workp->client = client_addr;
	
    if(pool->cur_queue_size == 0) 
    {
        pool->queue_tail = pool->queue_head = workp;
        if((rtn = pthread_cond_broadcast(&(pool->queue_not_empty))) != 0)
        {
            lprintf(logmy,FATAL,"pthread broadcast error\n");
            return -1;
        }
    }
    else 
    {
        pool->queue_tail->next = workp;
        pool->queue_tail = workp;
    }
    pool->cur_queue_size++;
    /* 释放对任务链表的独占 */
    if((rtn = pthread_mutex_unlock(&pool->queue_lock)) != 0) 
    {
        lprintf(logmy,FATAL,"pthread mutex lock failure\n");
        return -1;
    }
    return 0;
}

int tpool_destroy(tpool_t *pool, int finish)
{
    int i, rtn;
    tpool_work_t *cur;  /*当前工作线程*/
    lprintf(logmy, INFO, "destroy pool begin!\n");
    /* 释放对任务链表的独占 */
    if((rtn = pthread_mutex_lock(&(pool->queue_lock))) != 0)  
    {
        lprintf(logmy,FATAL,"pthread mutex lock failure\n");
        return -1;
    }
    /* 第一步，设置线程退出标记 */
    lprintf(logmy, INFO, "destroy pool begin 1!\n");
    if(pool->queue_closed || pool->shutdown) 
    {
        if((rtn = pthread_mutex_unlock(&(pool->queue_lock))) != 0) 
        {
            lprintf(logmy,FATAL,"pthread mutex lock failure\n");
            return -1;
        }
        return 0;
    }
    /* 第二步，禁止新任务加入任务链表 */
    lprintf(logmy, INFO, "destroy pool begin 2!\n");
    pool->queue_closed = 1;
    if(finish) 
    {
        while(pool->cur_queue_size != 0) 
        {
            if((rtn = pthread_cond_wait(&(pool->queue_empty),&(pool->queue_lock))) != 0) 
            {
                lprintf(logmy,FATAL,"pthread_cond_wait %d\n",rtn);
                return -1;
            }
        }
    }
    /* 第三步，设置线程池销毁标记 */
    lprintf(logmy, INFO, "destroy pool begin 3!\n");
    pool->shutdown = 1;
    if((rtn = pthread_mutex_unlock(&(pool->queue_lock))) != 0) 
    {
        lprintf(logmy,FATAL,"pthread mutex unlock failure\n");
        return -1;
    }
    /* 第四步，等待所有已建立的线程退出 */
    lprintf(logmy, INFO, "destroy pool begin 4!\n");
    if((rtn = pthread_cond_broadcast(&(pool->queue_not_empty))) != 0) 
    {
        lprintf(logmy,FATAL,"pthread_cond_boradcast %d\n",rtn);
        return -1;
    }
    if((rtn = pthread_cond_broadcast(&(pool->queue_not_full)))  != 0) 
    {
        lprintf(logmy,FATAL,"pthread_cond_boradcast %d\n",rtn);
        return -1;
    }
    for(i = 0; i < pool->num_threads; i++) 
    {
        if((rtn = pthread_join(pool->threads[i],NULL)) != 0)
        {
            lprintf(logmy,FATAL,"pthread_join %d\n",rtn);
            return -1;
        }
    }
    /* 第五步，释放线程池所占的内存空间 */
    free(pool->threads);
    while(pool->queue_head != NULL) 
    {
        cur = pool->queue_head->next;
        pool->queue_head = pool->queue_head->next;
        free(cur);
    }
    free(pool);
    lprintf(logmy, INFO, "destroy pool end!\n");
    return 0;
}
		
void *tpool_thread(void *tpool) 
{
    tpool_work_t *my_work;
    tpool_t *pool = (struct tpool *)tpool;

    for(;;)
    {/* 线程内循环 */
        pthread_mutex_lock(&(pool->queue_lock));  
      /* 如果任务列表为0，并且线程池没有关闭，则一直等待，直到任务到来为止  */
	  
        while((pool->cur_queue_size == 0) && (!pool->shutdown)) 
	        {
	            pthread_cond_wait(&(pool->queue_not_empty), &(pool->queue_lock));
	        }
		
        /* 线程池是否已经关闭，如果线程池关闭则线程自己主动关闭 */
		
	    if(pool->shutdown)
		    {
		        pthread_mutex_unlock(&(pool->queue_lock));
		        pthread_exit(NULL);      /*线程退出状态为空，主线程不捕获各副线程状态*/
		    }
        my_work = pool->queue_head;  
        pool->cur_queue_size--;
		
        /*将任务链表头部去掉，改任务正在处理中*/
        if(pool->cur_queue_size == 0)    
            pool->queue_head = pool->queue_tail = NULL;
        else
            pool->queue_head = my_work->next;
		
        /* 任务链表还没有满 */ 
        if((!pool->do_not_block_when_full) &&
                (pool->cur_queue_size == (pool->max_queue_size - 1))) 
        {
            pthread_cond_broadcast(&(pool->queue_not_full));
        }
		
        /*任务链表为空*/
        if(pool->cur_queue_size == 0) 
        {
            pthread_cond_signal(&(pool->queue_empty));
        }
		
        pthread_mutex_unlock(&(pool->queue_lock));
		
        /*启动线程业务处理逻辑*/
        (*(my_work->handler_routine))(my_work->arg,my_work->server,my_work->client); 
		
        free(my_work);
    }
    return(NULL);
}



