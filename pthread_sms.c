#include "data_global.h"
struct Message{
	char size[4];		//消息长度
	char which;			//消息模板
	char size_tal[16];	//指令
	char info[32];		//信息长度
}gprs_msg[] = {
	{"0c", 0, "AT+CMGS=27\r", "4ED35E937740706B5566FF01"},	//仓库着火啦！
//	{"0e", 1, "AT+CMGS=29\r", "4ED35E9367098D3C95EF5165FF01"},	//仓库有贼闯入！
	{"0e", 1, "AT+CMGS=29\r", "test"},	//仓库有贼闯入！
	{"0a", 2, "AT+CMGS=25\r", "514971678D856807FF01"},	//光照超标！
	{"0a", 3, "AT+CMGS=25\r", "6E7F5EA68D856807FF01"}		//湿度超标！
};

struct From_to_send{
	char center_number[16];
	char to_number[16];
}phone_NUM;

extern void gprs_serial_init(int fd);
//extern int send_message_tar(int fd, int which);
extern int send_message(int dev_fd,char *buf);
extern void set_phone_number(char from[],char to[]);
extern void *pthread_sms (void *);

extern pthread_mutex_t mutex_sms;
extern pthread_cond_t cond_sms;

//char recive_no[12] = "18657660187";
//char center_no[12] = "13647737579";
extern char recive_phone[12];
extern char center_phone[12];
extern unsigned char dev_sms_mask;

extern int dev_sms_fd;

//*******************************************************************
//获取线程号
pid_t gettid(void)
{
	return syscall(SYS_gettid);
}

//*******************************************************************
//A8与GPRS模块之间的初始化串口
void gprs_serial_init(int fd)
{
	struct termios options;//termios函数族提供一个常规的终端接口，用于控制非同步通信端口。

	tcgetattr(fd, &options);//取得当前串口的属性，并付给collect_fd这个设备
	options.c_cflag |= (CLOCAL | CREAD);//clocal表示忽略modem控制线，cread表示打开接收者
	options.c_cflag &= ~CSIZE;//清空原有字符长度（csize表示原有字符长度掩码）
	options.c_cflag &= ~CRTSCTS;//启用RTS/CTS（硬件）流控制
	options.c_cflag |= CS8;//设置字符长度掩码
	options.c_cflag &= ~CSTOPB;//设置停止位为1个（cstopb表示设置两个停止位）
	options.c_iflag |= IGNPAR;//忽略帧错误和奇偶校验错

	options.c_oflag = 0;//设置输出模式标志
	options.c_lflag = 0;//设置本地模式标志

	//cfsetispeed(&options, B115200);//设置输入波特率
	//cfsetospeed(&options, B115200);//设置输出波特率
	
	cfsetospeed(&options, B9600);//设置输出波特率
	cfsetospeed(&options, B9600);//设置输出波特率
	tcsetattr(fd, TCSANOW, &options);//把上面设置号的属性赋值给collect_fd这个设备，tcsanow表示马上生效

	printf("init gprs over...\n");
	return ;
}

int send(int fd,char *cmgf,char *cmgs,char *message)//发送函数，用于AT指令的发送
{
	printf("&&&&&&&&&&&&&&&&&&&&&&&&&&&&fd : %d", fd);
	int nread,nwrite;
	char buff[128];
	char reply[128];

	memset(buff,0,sizeof(buff));
	strcpy(buff,"AT\r");
	nwrite = write(fd,buff,strlen(buff));
	printf("1_nwrite = %d, %s\n", nwrite, buff);

	memset(reply,0,sizeof(reply));
	sleep(1);
	nread = read(fd,reply,sizeof(reply));
	printf("2_nread = %d, %s\n", nread, reply);
	/*发送方式*/
	memset(buff,0,sizeof(buff));
	strcpy(buff,"AT+CMGF=");
	strcat(buff,cmgf);
	strcat(buff,"\r");
	nwrite = write(fd,buff,strlen(buff));
	printf("3_nwrite = %d, %s\n", nwrite, buff);

	memset(reply,0,sizeof(reply));
	sleep(1);
	nread = read(fd,reply,sizeof(reply));
	printf("4_nread = %d, %s\n", nread, reply);

	memset(buff,0,sizeof(buff));
	strcpy(buff,"AT+CMGS=");
	strcat(buff,cmgs);
	strcat(buff,"\r");
	nwrite = write(fd,buff,strlen(buff));
	printf("5_nwrite = %d, %s\n", nwrite, buff);

	memset(reply,0,sizeof(reply));
	sleep(1);
	nread = read(fd,reply,sizeof(reply));
	printf("6_nread = %d, %s\n", nread, reply);
	/**/
	memset(buff,0,sizeof(buff));
	strcpy(buff,message);
	nwrite = write(fd,buff,strlen(buff));
	printf("7_nwrite = %d, %s\n", nwrite, buff);

	memset(reply,0,sizeof(reply));
	sleep(1);
	nread = read(fd,reply,sizeof(reply));
	printf("8_nread = %d, %s\n", nread, reply);

	printf("call send over\n");	
	return 0;
}
int send_message(int dev_fd ,char *info)//发送处理函数，处理字符串
{
//	system("./gprs_test");

	printf("&&&&&&&&&&&&&&&&&&&&& dev_fd: %d\n", dev_fd);
	printf("&&&&&&&&&&&&&&&&&info : %s\n", info);
	char *cmgf = "1";//文本模式发送
	int conter = 0;
	char cmgs[16] = {'\0'};
	char buf[128];
	
	gprs_serial_init(dev_fd);	//串口初始化
	strcpy(buf,info);
	printf("&&&&&&&&&&&&&&&& %s\n",buf);
	strcat(buf,"\x1a");//ctrl+z 的ASC 值26
	strcat(cmgs,recive_phone);

	send(dev_fd,cmgf,cmgs,buf);
#if 1
	printf("send message success\n");
#endif
	return 0;

	
	}


//向A8主线程中获得GPRS的线程
void *pthread_sms(void *arg)
{
	unsigned char buf;
	//打开设备文件
	if((dev_sms_fd = open(DEV_GPRS, O_RDWR | O_NOCTTY | O_NDELAY)) < 0){
		printf("Fail to open DEV_GSM !\n");
		exit(1);
	}
//	set_phone_number(center_phone, recive_phone);
	printf ("pthread_sms is ok\n");
	while (1)
	{
		pthread_mutex_lock(&mutex_sms);	//申请互斥锁
		pthread_cond_wait(&cond_sms, &mutex_sms);	//等待条件的成立
		buf = dev_sms_mask;
		printf ("pthread_sms is wake up! dev_sms_mask = %c\n", buf);
		pthread_mutex_unlock(&mutex_sms);		//解锁 
		if(SMS_TEM == buf)		//温度过高，即火灾警报
			//send_message(dev_sms_fd, 0);
			send_message(dev_sms_fd, "the temprature is too high");
	
		if(SMS_BRE == buf)		//不明身份人物闯入
		{	
			printf("&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&dev_sms_fd: %d\n",dev_sms_fd);
			send_message(dev_sms_fd, "stranger has come in");
		}
		if(SMS_ILL == buf)		//光照强度超标
			send_message(dev_sms_fd, "the illumination is irregular");
	
		if(SMS_HUM == buf)		//湿度超标
			send_message(dev_sms_fd, "humidity is irregular");

	}	

	return 0;
}


//*******************************************************************
/*
int main()
{
	pthread_mutex_lock(&mutex_sms);		//操作临界资源，先加锁
	set_phone_number("0", recive_no);  
	pthread_mutex_unlock(&mutex_sms);	//解锁
*/	
	//打开设备文件
/*	if((fd = open(DEV_GPRS, O_RDWR | O_NOCTTY | O_NDELAY)) < 0){
		printf("Fail to open DEV_GSM !\n");
		return -1;
	}*/

//	send_message(fd, 0);  //发送消息
/*
	pthread_t sms_id;	//线程id
	if(pthread_create(&sms_id,NULL,pthread_sms,NULL) < 0)	//创建线程
	{
		perror("Fail to pthread_create !\n");
		exit(1);
	}
	printf("Main pthread: pid=%d, tid=%d\n",getpid(), gettid());

	pthread_cond_init(&cond_sms, NULL);		//初始化条件表量

	pthread_mutex_lock(&mutex_sms);		//操作临界资源，先加锁
	pthread_cond_signal(&cond_sms);		//激活一个等待该条件的线程
	pthread_mutex_unlock(&mutex_sms);	//解锁

	pthread_join(sms_id, NULL);		//等待线程结束 
	pthread_detach(sms_id);			//回收线程资源


	return 0;
}
*/
 

