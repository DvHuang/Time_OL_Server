/************************************************************************* 
tpool.h
************************************************************************/
  
#ifndef _TPOOL_H_
#define _TPOOL_H_
#include  <stdio.h>
#include  <pthread.h>
/*工作线程链表*/
typedef struct tpool_work
{
  void (*handler_routine)();   /*任务函数指针*/
  void *arg;      /*任务函数参数*/
  struct tpool_work *next;    /*下一个任务链表*/
} tpool_work_t;
/*线程池结构体*/
typedef struct tpool
{
  int num_threads;     /*最大线程数*/
  int max_queue_size;    /*最大任务链表数*/
  int do_not_block_when_full;   /*当链表满时是否阻塞*/
  pthread_t *threads;    /*线程指针*/
  int cur_queue_size;
  tpool_work_t *queue_head;   /*链表头*/
  tpool_work_t *queue_tail;   /*链表尾*/
  pthread_mutex_t queue_lock;  /*链表互斥量*/
  pthread_cond_t queue_not_full;  /*链表条件量-未满*/
  pthread_cond_t queue_not_empty; /*链表条件量-非空*/
  pthread_cond_t queue_empty;  /*链表条件量-空*/
  int queue_closed;
  int shutdown;
} tpool_t;
/* 初始化连接池 */
extern tpool_t *tpool_init(int num_worker_threads,\
    int max_queue_size, int do_not_block_when_full);
/* 添加一个工作线程 */
extern int tpool_add_work(tpool_t *pool, void  (*routine)(), void *arg);
/* 清除线程池*/
extern int tpool_destroy(tpool_t *pool, int finish);
#endif /* _TPOOL_H_ */

  

