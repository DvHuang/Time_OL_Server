#!/bin/sh
rm testpool
gcc -g -pthread -o testpool testpool.c tpool.c log.c socket.c db.c error.c lockreg.c

