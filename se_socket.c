/************************************************
 * se_socket.c                                  *
 * function:                                  	*
 *    create a socket                        	*
 *    create a new thread,for receive function  *
 *    send message                             	*
 ************************************************/
#include "se_socket.h"

socklen_t len;
struct sockaddr_in saddr,caddr;
bool pi_from_host;
bool pc_from_host;
int pi_sockfd;
int pc_sockfd;
int pi_cl_sockfd;
int pc_cl_sockfd;
bool socket_start;

void *pi_socket(void *arg)
{
	int res;
	pthread_t recv_thread;
	pthread_attr_t thread_attr;
	len=sizeof(caddr);
	/* Set status of thread */
	res=pthread_attr_init(&thread_attr);
	if(res!=0)
	{
		printf("Setting detached attribute failed");
		exit(EXIT_FAILURE);
	}
	/* Create a socket */
	if((pi_sockfd=socket(AF_INET,SOCK_STREAM,0))<0)
	{
		printf("Socket Error");
		exit(1);
	}
	
	/* To prevent the program from normal exit, the next start bind failed. so set the socket option, the address is reused. */
//    int is_reuse_addr = 1;
//    setsockopt(pi_sockfd, SOL_SOCKET, SO_REUSEADDR, (const char*)&is_reuse_addr, sizeof(is_reuse_addr));
	
	bzero(&saddr,sizeof(struct sockaddr));
	saddr.sin_family=AF_INET;
	saddr.sin_addr.s_addr=htonl(INADDR_ANY);
	saddr.sin_port=htons(PI_PORT);
	if(bind(pi_sockfd,(struct sockaddr *)&saddr,sizeof(struct sockaddr_in))==-1)
	{
		printf("Bind Error");
		exit(1);
	}
	/* Set the status of thread,don't wait for return of the subthread */
	res=pthread_attr_setdetachstate(&thread_attr,PTHREAD_CREATE_DETACHED);
	if(res!=0)
	{
		printf("Setting detached attribute failed");
		exit(EXIT_FAILURE);
	}
	if(listen(pi_sockfd,5)<-1)
	{
		printf("Listen Error");
		exit(1);
	}    
    while(1)
	{
		pi_cl_sockfd=accept(pi_sockfd,(struct sockaddr *)&caddr,&len);
        if (pi_cl_sockfd < 0)
        {
            printf("Server Accept Failed!\n");
            exit(EXIT_FAILURE);
        }
        /* Create a sub thread,call recv_func() */
	    res=pthread_create(&recv_thread,&thread_attr,pi_recv_func,NULL); 
	    if(res!=0)
	    {
			printf("Thread create error");
			exit(EXIT_FAILURE);
		}
		pi_from_host=true;
		/* Callback the attribute of thread */
		(void)pthread_attr_destroy(&thread_attr);
	}
}

void *pc_socket(void *arg)
{
	int res;
	pthread_t recv_thread;
	pthread_attr_t thread_attr;
	len=sizeof(caddr);
	/* Set status of thread */
	res=pthread_attr_init(&thread_attr);
	if(res!=0)
	{
		printf("Setting detached attribute failed");
		exit(EXIT_FAILURE);
	}
	/* Create a socket */
	if((pc_sockfd=socket(AF_INET,SOCK_STREAM,0))<0)
	{
		printf("Socket Error");
		exit(1);
	}
	
	/* To prevent the program from normal exit, the next start bind failed. so set the socket option, the address is reused. */
//    int is_reuse_addr = 1;
//    setsockopt(pc_sockfd, SOL_SOCKET, SO_REUSEADDR, (const char*)&is_reuse_addr, sizeof(is_reuse_addr));
	
	bzero(&saddr,sizeof(struct sockaddr));
	saddr.sin_family=AF_INET;
	saddr.sin_addr.s_addr=htonl(INADDR_ANY);
	saddr.sin_port=htons(PC_PORT);
	if(bind(pc_sockfd,(struct sockaddr *)&saddr,sizeof(struct sockaddr_in))==-1)
	{
		printf("Bind Error");
		exit(1);
	}
	/* Set the status of thread,don't wait for return of the subthread */
	res=pthread_attr_setdetachstate(&thread_attr,PTHREAD_CREATE_DETACHED);
	if(res!=0)
	{
		printf("Setting detached attribute failed");
		exit(EXIT_FAILURE);
	}
	if(listen(pc_sockfd,5)<-1)
	{
		printf("Listen Error");
		exit(1);
	}    
	while(1)
	{
		pc_cl_sockfd=accept(pc_sockfd,(struct sockaddr *)&caddr,&len);
		if (pc_cl_sockfd < 0)
		{
			printf("Server Accept Failed!\n");
			exit(EXIT_FAILURE);
		}
		/* Create a sub thread,call recv_func() */
		res=pthread_create(&recv_thread,&thread_attr,pc_recv_func,NULL); 
		if(res!=0)
		{
			printf("Thread create error");
			exit(EXIT_FAILURE);
		}
		pc_from_host=true;
		/* Callback the attribute of thread */
		(void)pthread_attr_destroy(&thread_attr);
	}
}

/* Receive function */
void *pi_recv_func(void *arg)
{
    int nu=0;
	char recv_text[MAXSIZE];
	while(1)
	{	
		nu=recv(pi_cl_sockfd,recv_text,MAXSIZE,0);
        /* To Receive message from client and get the address infomation */
		if(nu<0)  
		{
			printf("--> pi_Client disconnect!\n");
			close(pi_cl_sockfd);
			pi_from_host=false;
			pthread_exit(&pi_recv_func);
		}
		else if( pi_from_host==true && nu>0)
		{
			pc_send_func(recv_text);
			printf("pi-> %s\n",recv_text);
			memset(recv_text, 0x0, strlen(recv_text));
		}	
	}
}

/* Receive function */
void *pc_recv_func(void *arg)
{
    int nu=0;
	char recv_text[MAXSIZE];
	while(1)
	{	
		nu=recv(pc_cl_sockfd,recv_text,MAXSIZE,0);
        /* To Receive message from client and get the address infomation */
		if(nu<0)  
		{
			printf("--> pc_Client disconnect!\n");
			close(pc_cl_sockfd);
			pc_from_host=false;
			pthread_exit(&pc_recv_func);
		}
		else if( pc_from_host==true && nu>0)
		{
			pi_send_func(recv_text);
			printf("pc-> %s\n",recv_text);
			memset(recv_text, 0x0, strlen(recv_text));
		}
	}
}

/* Send function,send message to client */
void pi_send_func(const char * send_text)
{
        /* If there is no text,continue */
        if(strlen(send_text)==1)
                return;
        if( !pi_from_host ) printf("--> Waiting the client to connect!\n");
        /* Send message */
        if(pi_from_host && send(pi_cl_sockfd,send_text,strlen(send_text),0)<0)
        {
                printf("S send error");
                exit(1);
        }
}

/* Send function,send message to client */
void pc_send_func(const char * send_text)
{
	/* If there is no text,continue */
	if(strlen(send_text)==1)
		return;
	if( !pc_from_host ) printf("--> Waiting the client to connect!\n");
	/* Send message */
	if(pc_from_host && send(pc_cl_sockfd,send_text,strlen(send_text),0)<0)
	{
		printf("S send error");
		exit(1);
	}
}

//void send_text()
//{
//	char *text;
//	/* Socket creating has succeed ,so send message */
//	text=(char *)malloc(MAXSIZE);
//    scanf("%s",text);
//	if(text==NULL)
//	{
//		printf("Malloc error!\n");
//		exit(1);
//	}
//	/* If there is no input,do nothing but return */
//	if(strcmp(text,"")!=0)
//	{
//		send_func(text);
//	}
//	else
//		printf("The message can not be empty ...\n");
//	free(text);
//}

void startsocket(void)
{
    int res;     
    pthread_t pc_thread;
	pthread_t pi_thread;
    res=pthread_create(&pc_thread,NULL,pc_socket,NULL); 
	if(res!=0)
	{
            printf("Thread pc_create error");
            exit(EXIT_FAILURE);
	}
    res=pthread_create(&pi_thread,NULL,pi_socket,NULL); 
	if(res!=0)
	{
            printf("Thread pi_create error");
            exit(EXIT_FAILURE);
	}
	printf("<*********************************>\n");
    printf("<                                 >\n");
    printf("<     Function: Aliyun start      >\n");
    printf("<     Company: SIEMENS of SH      >\n");
    printf("<          Editor: Mxin           >\n");
    printf("<   Data: %s %s    >\n", __DATE__, __TIME__);
    printf("<                                 >\n");
    printf("<*********************************>\n");
    return;
}
