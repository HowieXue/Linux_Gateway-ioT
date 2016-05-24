#include "link_list.h"
#include "data_global.h"

#define LEN_ENV 12
#define LEN_RFID 4 

extern int fd_socket;

extern linklist linkHead;
extern linklist slinkHead;

extern pthread_mutex_t mutex_linklist;
extern pthread_mutex_t mutex_slinklist;
extern pthread_mutex_t mutex_analysis;
extern pthread_mutex_t mutex_global;
extern pthread_mutex_t mutex_sms;
extern pthread_mutex_t mutex_buzzer;
extern pthread_mutex_t mutex_client_send;

extern pthread_cond_t cond_analysis;
extern pthread_cond_t cond_client_send;

extern pthread_cond_t cond_sqlite;
extern pthread_cond_t cond_refresh;
extern pthread_cond_t cond_buzzer;
extern pthread_cond_t cond_sms;





int socket_connect()     //socket创建并连接远程终端函数
{
	
        struct sockaddr_in address;
	int len, result;
	fd_socket = socket(AF_INET, SOCK_STREAM,0); 
        address.sin_family = AF_INET;
	address.sin_addr.s_addr = inet_addr(REMOTE_IP);
	address.sin_port = htons(PORT);
	len = sizeof(address);                                                                                                                                                            
	do{
        result = connect(fd_socket, (struct sockaddr *)&address, len);  
        if(result == -1) 
	{
		perror("oops: socket connect failed");
		//exit(1);
	}
	printf("Socket connect successed !!!\n\n");
	
        }
 
        while(result==-1);

         return 0;  
}



void *pthread_client_send (void *arg)
{
	signal(SIGPIPE, SIG_IGN);
        linklist node;
	link_datatype buf;
        int  len;
       
	printf ("pthread_client_send is ok\n");
        socket_connect();
	while (1)
	{
		pthread_mutex_lock (&mutex_client_send);
		pthread_cond_wait (&cond_client_send, &mutex_client_send);
		pthread_mutex_unlock (&mutex_client_send);
                
      //         if(socket_connect()!=0)
           //     {
              //     close(fd_socket) ;
               //    perror("connect");
                 //  continue;
                 // }
		while (1)
		{
			

                        pthread_mutex_lock (&mutex_linklist);
			if ((node = GetLinknode (linkHead)) == NULL)
			{
				pthread_mutex_unlock (&mutex_linklist);
				break;
			}
			buf = node->data;
			free (node);
			pthread_mutex_unlock (&mutex_linklist);
                        len=write(fd_socket,buf.text,LEN_ENV);
		if(len<0)
                     {
			printf("\nwrite to socket fial\n");
                        close(fd_socket) ;
                        socket_connect();
                        break;
		     }
		else
                     {
			 printf("\nwrite data to socket success, data_len = %d\n",len);
		     }

		}
	}
	return 0;
}
