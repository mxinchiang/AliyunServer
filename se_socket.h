/***********************************
 * se_socket.h                     *
 * the header files and functions  *
 ***********************************/
#ifndef __SE_SOCKET_H__
#define __SE_SOCKET_H__

#include <sys/types.h> 
#include <sys/socket.h> 
#include <netinet/in.h> 
#include <stdio.h> 
#include <errno.h> 
#include <stdlib.h>
#include <string.h>
#include <semaphore.h>
#include <unistd.h>
#include <pthread.h>
#include <arpa/inet.h>
#define bool _Bool
#define true 1
#define false 0
#define PC_PORT 8888
#define PI_PORT 6666
#define MAXSIZE 1024
void *pi_recv_func(void *arg);
void *pc_recv_func(void *arg);
void *pi_socket(void *arg);
void *pc_socket(void *arg);
void send_text(void);
void pi_send_func(const char *);
void pc_send_func(const char *);
void startsocket(void);
#endif
