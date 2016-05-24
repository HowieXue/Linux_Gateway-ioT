#include "link_list.h"
#include "data_global.h"

#define LEN_ENV 12
#define LEN_RFID 4 

int fd_socket_new;

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


void serial_init(int fd)
{
	struct termios options;
	tcgetattr(fd, &options);
	options.c_cflag |= ( CLOCAL | CREAD );
	options.c_cflag &= ~CSIZE;
	options.c_cflag &= ~CRTSCTS;
	options.c_cflag |= CS8;
	options.c_cflag &= ~CSTOPB; 
	options.c_iflag |= IGNPAR;
	options.c_iflag &= ~(ICRNL | IXON);
	options.c_oflag = 0;
	options.c_lflag = 0;

	cfsetispeed(&options, B115200);
	cfsetospeed(&options, B115200);
	tcsetattr(fd,TCSANOW,&options);
}


int socket_connect()     //socket创建并连接远程终端函数
{
        struct sockaddr_in address;
	int len, result;
	fd_socket_new = socket(AF_INET, SOCK_STREAM,0); 
        address.sin_family = AF_INET;
	address.sin_addr.s_addr = inet_addr(REMOTE_IP);
	address.sin_port = htons(PORT);
	len = sizeof(address);                                                                                                                                                            
	do{
        result = connect(fd_socket_new, (struct sockaddr *)&address, len);  
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



void *pthread_client_receive (void *arg)
{
	signal(SIGPIPE, SIG_IGN);

        
#if 1
	if ((dev_uart_fd = open (DEV_ZIGBEE, O_RDWR)) < 0)
	{
		perror ("open arm-gate_com3  is error");
		exit (-1);
	}
	serial_init (dev_uart_fd);  //init  com3
	printf ("com init is ok\n");
#endif       



        socket_connect();
	printf ("pthread_client_receive is ok\n");

	int i,len;
	char socket_buf[100];
	while(1)
	{
		len = read( fd_socket_new, socket_buf, 100);
		if(len <= 0)
		{
			printf("recive data from socket fail !\n");
			perror("recvfrom:");
			exit(EXIT_FAILURE);
		}
		else
		{
			printf("\nrecieve data from socket sueccess, data_len= %d\n",len);
			for(i=0;i<len;i++)
			{
				printf("%x ",socket_buf[i]);
			}
			
                        
                        if(socket_buf[0]==0xff&&socket_buf[0]==0xff)
                         {
			
                            len = write(fd_uart, socket_buf, len);    //send the data recived from socket to uart1
                         }

			if(len < 0)
			{
				printf("\nwrite data to uart fail\n");
			}
			else
			{
				printf("\nwrite data to uart success,data_len=%d\n",len);
			}
		}
	}

	
	return 0;
}
