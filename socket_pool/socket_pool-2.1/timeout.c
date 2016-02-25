CheckFd is the one and only function calling select. 
its used to check if the fd works well.

int RecvableTimeout( int fd , int sec )
{
        fd_set rset;
        struct timeval tv;
        time_t begin_t;
        int retval;

        if( sec == 0 )
        {
                fprintf(stderr,"isset = %d ???\n",FD_ISSET(fd,&rset));
                return( select(fd+1,&rset,NULL,NULL,NULL) );    /* wait for ever */
        }
        if( sec < 0 )
        {
                return -1;
        }

        while( 1 )
        {
                begin_t = time(NULL);
                FD_ZERO( &rset );
                FD_SET( fd , &rset );
                tv.tv_sec  = sec;
                tv.tv_usec = 0;
                if( (retval=select(fd+1,&rset,NULL,NULL,&tv)) < 0 ) 
                {
                        if( errno==EINTR )
                        {

                                sec = sec - ( time(NULL) - begin_t);
                                continue;
                        }
                        else
                        {
                                break;
                        }
                }
                else
                {
                        break;
                }
        }
        return retval;
}

int IMRecv( int fd , struct im_packet_t * readbuf,int sec )
{
        int retval,retval2;
        unsigned short nread=0;
        int timeout;

        timeout = sec;

        if (readbuf == NULL)
        {
                return -1;
        }
        /*
         * Waiting for read buffer ready
        
        if ( timeout < 0 ) timeout = 0 ;
        if( timeout > 0 )
        {
                if( (retval= RecvableTimeout(fd,timeout))<0 )
                {
                        close(fd);
                        fprintf(stderr,"IMRecv: RecvableTimeout for sth wrong...\n");
                        return -3;
                }
                else if( retval==0 )
                {
                        close(fd);
                        fprintf(stderr,"IMRecv: RecvableTimeout for time out...\n");
                        errno = 0;
                        return -2;
                }
                fprintf(stderr,"ww::fd = %d, retval = %d\n timeout = %d,  %s\n",fd,retval,timeout,strerror(errno));
                
        }
    */
        /*
         * Read head of the packet
         */
        memset( readbuf,0,sizeof(struct im_head_t) );
RECV:
        retval = recvn( fd,(char *)readbuf,sizeof(struct im_head_t),0 );
        if( retval<0 )
        {
                /* Add for nonblocking socket */
                if( errno == EWOULDBLOCK )
                {
                        return(0);
                }
                if(errno   ==   EINTR)
                        goto RECV;
                perror("IMRecv:recvn");
                fprintf(stderr,"recvn ww::fd = %d, retval = %d\n timeout = %d,  %s\n",fd,retval,timeout,strerror(errno));
                if( errno == ETIMEDOUT )
                {
                        /* 
                         * on keepalive mode and no timed-out select,
                         * 2 hours later,recv timed out 
                         */
                        close(fd);
                        return(-2);
                }
                else
                {
                        close(fd);
                        return(-3);
                }
        }
        else if ( retval == 0 ) /* disconnected */
        {
                fprintf(stderr,"IMRecv: client close socket by recvn...\n");
                close(fd);
                return(-1);
        }
        else if ( retval < sizeof(struct im_head_t) )  /* some exception occurs */
        {
                fprintf(stderr," Warning: Receving len < im_head_t...\n");
                close(fd);
                return(-4);
        }
        // nread = Ntohs(socket_corba, readbuf->iBufferLength );
        nread = ntohs(readbuf->head.length);

        if( nread == 0 )
        {
                return retval;
        }

        if( nread > MAX_IM_PACKET_BUF )
        {
                fprintf(stderr," Warning: Buffer length=%d| > MAX_IM_PACKET_BUF...\n",nread);
                nread = RADIUS_BUF_SIZE;
                close(fd);
                return (-5);
        }

        /*
         * Read the rest of the packet on length which specicated by nsend
         */
        retval2 = recvn( fd,(char *)readbuf+sizeof(struct im_head_t),nread,0 );
        if( retval2<0 )
        {
                /* Add it for nonblocking socket */
                if( errno == EWOULDBLOCK )
                {
                        return (-1);
                }
                perror("IMRecv:recvn2");
                close(fd);
                return(-3);
        }
        else if( retval2 == 0 )  /* disconnected */
        {
                fprintf(stderr,"IMRecv: client closed socket by recvn2...\n");
                close(fd);
                return (-1);
        }
        else if( retval2 < nread )
        {
                fprintf(stderr,"Warning: recvn2 len=%d| real_len=%d|...\n",retval2,nread);
                close(fd);
                return(-4);
        }
        return(retval+retval2); /* receving total length = (retval+retval2) */
}

int CheckFd(int fd)
{
    fd_set rset,wset;
    struct timeval tv;
    int rc;

        if (fd <0)
        {
                return -1;
        }
    FD_ZERO( &rset );
    FD_ZERO( &wset );
    FD_SET( fd , &rset );
    FD_SET( fd , &wset );
    tv.tv_sec = 0;
    tv.tv_usec = 0;
    rc = select(fd+1,&rset,NULL,NULL,&tv);
    if(rc < 0)
    {
        return -1;
    }
    rc = select(fd+1,NULL,&wset,NULL,&tv);
    if(rc < 0)
    {
        return -1;
    }
    return 0;

}