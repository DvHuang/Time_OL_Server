#ifndef __LOG_H
#define __LOG_H
#include <stdio.h>
#include <semaphore.h>
/*记录的最大长度*/
#define LOGLINE_MAX 1024
/*记录的等级*/
#define DEBUG 1
#define INFO  2
#define WARN  3
#define ERROR 4
#define FATAL 5
/*记录的类型*/
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
 功能描述： 记录打印函数，将记录打印至记录文件logfile。
 参数：  log_t - log_open()函数的返回值
 level - 可以是: DEBUG, INFO, WARN, ERROR, FATAL
 fmt  - 记录的内容，格式同printf()函数
 返回值： 成功返回0，失败返回-1
*/
extern int lprintf( log_t *log, unsigned int level, char *fmt, ... );
/*
 *   功能描述： 初始化记录文件the logfile
 *   参数： fname - 记录文件logfile的文件名
 *   flags  -  记录格式的选项
 *          LOG_TRUNC  - 截断打开的记录文件
 *          LOG_NODATE - 忽略记录中的每一行
 *          LOG_NOLF    - 自动为每条记录新开一行.
 *          LOG_NOLVL  - 不记录消息的等级
 *          LOG_STDERR - 将消息同时送到STDERR
 *返回值：成功返回log_t（>0），失败返回NULL
*/
extern log_t *log_open( char *fname, int flags );
/*
 * 功能描述：关闭记录文件
 * 参数：  * log  - 记录文件的指针
*/
extern void log_close( log_t *log );



#endif