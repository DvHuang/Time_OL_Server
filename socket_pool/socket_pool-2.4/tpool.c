/************************************************************************* 
 tpool.c �̳߳ص�ʵ��
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


/* �����߳� */
void *tpool_thread(void *tpool);
extern log_t *logmy;
 
/***************�̳߳س�ʼ��*****************************/
tpool_t *tpool_init(int num_worker_threads, /*�̳߳��̸߳���*/
            int max_queue_size,   /*���������*/
      int do_not_block_when_full) /*�Ƿ�������������ʱ��*/
{
    int i, rtn;
    tpool_t *pool; 
    lprintf(logmy, INFO, "init pool begin ...\n");
    /* �����̳߳ؽṹ�� */
    if((pool = (struct tpool *)malloc(sizeof(struct tpool))) == NULL) 
    {
        lprintf(logmy, FATAL, "Unable to malloc() thread pool!\n");
        return NULL;
    }
    /* �����̳߳ؼܹ����Ա */
    pool->num_threads = num_worker_threads;                      /*�����̸߳���*/
    pool->max_queue_size = max_queue_size;                      /*����������󳤶�*/
    pool->do_not_block_when_full = do_not_block_when_full;      /*����������ʱ�Ƿ�ȴ�*/
    /* �����̳߳ػ��� */
    if((pool->threads = (pthread_t *)malloc(sizeof(pthread_t)*num_worker_threads)) == NULL) 
    {
        lprintf(logmy, FATAL,"Unable to malloc() thread info array\n");
        return NULL; 
    }
    /* ��ʼ���������� */
    pool->cur_queue_size = 0;
    pool->queue_head = NULL;
    pool->queue_tail = NULL;
    pool->queue_closed = 0;
    pool->shutdown = 0;
    /* ��ʼ������������������� �����߳�֮���ͬ�� */
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
    /* �������е��߳� */
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

int tpool_add_work(tpool_t *pool,           /*�̳߳�ָ��*/
                    void (*routine)(void *),  /*�����̺߳���ָ��*/
                    void *arg,
                    int new_server_socket_fd,
                    struct sockaddr_in client_addr)                  /*�����̺߳�������*/
{
    int rtn;
    tpool_work_t *workp; /*��ǰ�����߳�*/
    if((rtn = pthread_mutex_lock(&pool->queue_lock)) != 0)
    {
        lprintf(logmy,FATAL,"pthread mutex lock failure\n");
        return -1;
    }
    /* ��ȡ��ռ����ʽ������������ */
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
    /* �ȴ���������Ϊ���߳��ͷſռ� */
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
    /* ���乤���߳̽ṹ�� */
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
    /* �ͷŶ���������Ķ�ռ */
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
    tpool_work_t *cur;  /*��ǰ�����߳�*/
    lprintf(logmy, INFO, "destroy pool begin!\n");
    /* �ͷŶ���������Ķ�ռ */
    if((rtn = pthread_mutex_lock(&(pool->queue_lock))) != 0)  
    {
        lprintf(logmy,FATAL,"pthread mutex lock failure\n");
        return -1;
    }
    /* ��һ���������߳��˳���� */
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
    /* �ڶ�������ֹ����������������� */
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
    /* �������������̳߳����ٱ�� */
    lprintf(logmy, INFO, "destroy pool begin 3!\n");
    pool->shutdown = 1;
    if((rtn = pthread_mutex_unlock(&(pool->queue_lock))) != 0) 
    {
        lprintf(logmy,FATAL,"pthread mutex unlock failure\n");
        return -1;
    }
    /* ���Ĳ����ȴ������ѽ������߳��˳� */
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
    /* ���岽���ͷ��̳߳���ռ���ڴ�ռ� */
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
    {/* �߳���ѭ�� */
        pthread_mutex_lock(&(pool->queue_lock));  
      /* ��������б�Ϊ0�������̳߳�û�йرգ���һֱ�ȴ���ֱ��������Ϊֹ  */
	  
        while((pool->cur_queue_size == 0) && (!pool->shutdown)) 
	        {
	            pthread_cond_wait(&(pool->queue_not_empty), &(pool->queue_lock));
	        }
		
        /* �̳߳��Ƿ��Ѿ��رգ�����̳߳عر����߳��Լ������ر� */
		
	    if(pool->shutdown)
		    {
		        pthread_mutex_unlock(&(pool->queue_lock));
		        pthread_exit(NULL);      /*�߳��˳�״̬Ϊ�գ����̲߳���������߳�״̬*/
		    }
        my_work = pool->queue_head;  
        pool->cur_queue_size--;
		
        /*����������ͷ��ȥ�������������ڴ�����*/
        if(pool->cur_queue_size == 0)    
            pool->queue_head = pool->queue_tail = NULL;
        else
            pool->queue_head = my_work->next;
		
        /* ��������û���� */ 
        if((!pool->do_not_block_when_full) &&
                (pool->cur_queue_size == (pool->max_queue_size - 1))) 
        {
            pthread_cond_broadcast(&(pool->queue_not_full));
        }
		
        /*��������Ϊ��*/
        if(pool->cur_queue_size == 0) 
        {
            pthread_cond_signal(&(pool->queue_empty));
        }
		
        pthread_mutex_unlock(&(pool->queue_lock));
		
        /*�����߳�ҵ�����߼�*/
        (*(my_work->handler_routine))(my_work->arg,my_work->server,my_work->client); 
		
        free(my_work);
    }
    return(NULL);
}



