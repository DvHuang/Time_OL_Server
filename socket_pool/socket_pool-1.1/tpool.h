/************************************************************************* 
tpool.h
************************************************************************/
  
#ifndef _TPOOL_H_
#define _TPOOL_H_
#include  <stdio.h>
#include  <pthread.h>
/*�����߳�����*/
typedef struct tpool_work
{
  void (*handler_routine)();   /*������ָ��*/
  void *arg;      /*����������*/
  struct tpool_work *next;    /*��һ����������*/
} tpool_work_t;
/*�̳߳ؽṹ��*/
typedef struct tpool
{
  int num_threads;     /*����߳���*/
  int max_queue_size;    /*�������������*/
  int do_not_block_when_full;   /*��������ʱ�Ƿ�����*/
  pthread_t *threads;    /*�߳�ָ��*/
  int cur_queue_size;
  tpool_work_t *queue_head;   /*����ͷ*/
  tpool_work_t *queue_tail;   /*����β*/
  pthread_mutex_t queue_lock;  /*��������*/
  pthread_cond_t queue_not_full;  /*����������-δ��*/
  pthread_cond_t queue_not_empty; /*����������-�ǿ�*/
  pthread_cond_t queue_empty;  /*����������-��*/
  int queue_closed;
  int shutdown;
} tpool_t;
/* ��ʼ�����ӳ� */
extern tpool_t *tpool_init(int num_worker_threads,\
    int max_queue_size, int do_not_block_when_full);
/* ���һ�������߳� */
extern int tpool_add_work(tpool_t *pool, void  (*routine)(), void *arg);
/* ����̳߳�*/
extern int tpool_destroy(tpool_t *pool, int finish);
#endif /* _TPOOL_H_ */

  

