#ifndef __LOG_H
#define __LOG_H
#include <stdio.h>
#include <semaphore.h>
/*��¼����󳤶�*/
#define LOGLINE_MAX 1024
/*��¼�ĵȼ�*/
#define DEBUG 1
#define INFO  2
#define WARN  3
#define ERROR 4
#define FATAL 5
/*��¼������*/
#define LOG_TRUNC   1<<0
#define LOG_NODATE  1<<1
#define LOG_NOLF    1<<2
#define LOG_NOLVL   1<<3
#define LOG_DEBUG   1<<4
#define LOG_STDERR  1<<5
#define LOG_NOTID   1<<6
typedef struct 
{
    int fd;
    sem_t sem;
    int flags;
} log_t;
/* 
 ���������� ��¼��ӡ����������¼��ӡ����¼�ļ�logfile��
 ������  log_t - log_open()�����ķ���ֵ
 level - ������: DEBUG, INFO, WARN, ERROR, FATAL
 fmt  - ��¼�����ݣ���ʽͬprintf()����
 ����ֵ�� �ɹ�����0��ʧ�ܷ���-1
*/
extern int lprintf( log_t *log, unsigned int level, char *fmt, ... );
/*
 *   ���������� ��ʼ����¼�ļ�the logfile
 *   ������ fname - ��¼�ļ�logfile���ļ���
 *   flags  -  ��¼��ʽ��ѡ��
 *          LOG_TRUNC  - �ضϴ򿪵ļ�¼�ļ�
 *          LOG_NODATE - ���Լ�¼�е�ÿһ��
 *          LOG_NOLF    - �Զ�Ϊÿ����¼�¿�һ��.
 *          LOG_NOLVL  - ����¼��Ϣ�ĵȼ�
 *          LOG_STDERR - ����Ϣͬʱ�͵�STDERR
 *����ֵ���ɹ�����log_t��>0����ʧ�ܷ���NULL
*/
extern log_t *log_open( char *fname, int flags );
/*
 * �����������رռ�¼�ļ�
 * ������  * log  - ��¼�ļ���ָ��
*/
extern void log_close( log_t *log );



#endif