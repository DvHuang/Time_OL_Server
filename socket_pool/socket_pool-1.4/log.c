#include <stdio.h>
#include <unistd.h>
#include <semaphore.h>
#include <stdarg.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <time.h>
#include <pthread.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include "log.h"
int lprintf( log_t *log, unsigned int level, char *fmt, ... ) 
{
    int fd;
    int rc;
    va_list ap;
    time_t now;
    char date[50];
    static char line[LOGLINE_MAX];
    static char threadnum[10];
    int cnt;
    static char *levels[6] = { "[(bad)] ", "[debug] ", "[info ] ", "[warn ] "
,  "[error] ", "[fatal] " };
    if(!log) return -1;
    if( !(log->flags&LOG_DEBUG) && level == DEBUG ) return 0;
    fd=log->fd;
    /*日期*/
    if( !(log->flags&LOG_NODATE) ) 
    {
        now=time(NULL);
        strcpy(date,ctime(&now));
        date[strlen(date)-6]=' ';
        date[strlen(date)-5]='\0';
    }
    /*线程号*/
    if( !(log->flags&LOG_NOTID) ) 
    {
        sprintf(threadnum, "(TID:%lu) ", pthread_self());
    }
    cnt = snprintf(line, sizeof(line), "%s%s%s",
                  log->flags&LOG_NODATE ? "" : date,
                  log->flags&LOG_NOLVL  ? "" : 
                  (level > FATAL ? levels[0] : levels[level]),
                  log->flags&LOG_NOTID  ? "" : threadnum);
    va_start(ap, fmt);
    vsnprintf(line+cnt, sizeof(line)-cnt, fmt, ap);    
/*如果输入的日志过长会自动截取*/
    va_end(ap);
    line[sizeof(line)-1] = '\0';
    if( !(log->flags&LOG_NOLF) ) 
    {
        /*chomp(line);*/
        /*strcpy(line+strlen(line), "\n");*/
    }
    sem_wait(&log->sem);        /*用信号实现同步*/
    rc = write(fd, line, strlen(line));
    if (log->flags&LOG_STDERR)
      write(2, line, strlen(line));
    sem_post(&log->sem);
    if( !rc ) errno = 0;
    return rc;
}

log_t *log_openlog( char *fname, int flags ) 
  {
    log_t *log = malloc(sizeof(log_t));
    if(!log) 
    {
        fprintf(stderr, "log_open: Unable to malloc()");
        goto log_open_a;
    }
    log->flags=flags;
    if( !strcmp(fname,"-") ) 
    {
        log->fd = 2;
    } 
  else 
  {
        log->fd = open(fname, O_WRONLY|O_CREAT|O_NOCTTY | 
                (flags&LOG_TRUNC ? O_TRUNC : O_APPEND) , 0666);
    }
    if( log->fd == -1 )
    {
        fprintf(stderr, "log_open: Opening logfile %s: %s", fname, strerror(errno));
        goto log_open_b;
    }
    if( sem_init(&log->sem, 0, 1) == -1 )
    {
        fprintf(stderr, "log_open: Could not initialize log semaphore.");
        goto log_open_c;
    }
    return log;
log_open_c:
    close(log->fd);
log_open_b:
    free(log);
log_open_a:
    return NULL;
}



void log_close( log_t *log ) 
{
    sem_wait(&log->sem);
    sem_destroy(&log->sem);
    close(log->fd);
    free(log);
    return;
}