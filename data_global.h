#ifndef __DATA_GLOBAL__H__
#define __DATA_GLOBAL__H__

#include <stdio.h>
#include <errno.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <pthread.h>
#include <termios.h>
#include <syscall.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/msg.h>
#include <sys/sem.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/ioctl.h>
#include <linux/fs.h>
#include <linux/ioctl.h>
#include "dota_cache.h"
#include "netinet/in.h"
#include "sys/socket.h"



#define DEV_GPRS 			"/dev/s3c2410_serial2"
#define DEV_ZIGBEE 			"/dev/ttySAC3"           //  arm_gate_com3
#define DEV_LED				"/dev/dota_led"
#define DEV_BUZZER			"/dev/dota_beep"
#define DEV_INFRARED		        "/dev/dota_keyboard"
#define DEV_CAMERA			"/tmp/webcom"
#define SQLITE_OPEN			"/home/y/warehouse.db"

#define REMOTE_IP                        "192.168.1.99" 
#define PORT                              5678 

#define LED_ON 				_IO('L', 10)
#define LED_OFF 			_IO('L', 11)
#define BUZZER_OFF			_IO('B', 11)	
#define BUZZER_ON			_IO('B', 10)

//#define BUZZER_OFF			_IO('B', 1)	
//#define BUZZER_ON			_IO('B', 0)
#define MSGTYPE_ENV			'e'
#define MSGTYPE_GOODS 		        'g'
#define MSGTYPE_ACK			'a'

#define STORAGE_NUM 		         5

#define GOODS_NUM			10
#define GOODS_IN 			'I'
#define GOODS_OUT			'O'

#define SMS_TEM				't'
#define SMS_HUM				'h'
#define SMS_ILL				'i'
#define SMS_BRE				'b'

#define QUEUE_MSG_LEN		        32

#define MSG_LED				1L
#define MSG_LED_TEM_ON		0x22
#define MSG_LED_TEM_OFF		0x20
#define MSG_LED_HUM_ON		0x44
#define MSG_LED_HUM_OFF		0x40
#define MSG_LED_ILL_ON		0x88
#define MSG_LED_ILL_OFF		0x80

#define MSG_BEEP			2L
#define MSG_BEEP_OFF		        0
#define MSG_BEEP_ON			1

#define MSG_M0				4L
#define MSG_CONNECT_SUCCESS 'Y'
#define MSG_M0_FAN_OFF		0x00
#define MSG_M0_FAN_ON1		0x01
#define MSG_M0_FAN_ON2		0x02
#define MSG_M0_FAN_ON3		0x03

#define MSG_M0_BEEP_OFF		0x10
#define MSG_M0_BEEP_ON		0x11
#define MSG_M0_BEEP_AU_OFF	0x12
#define MSG_M0_BEEP_AU_ON	0x13

#define MSG_M0_LED_OFF		0x20
#define MSG_M0_LED_ON		0x21

#define MSG_M0_SEG_ON		0x30
#define MSG_M0_SEG_OFF		0x3f

#define ENV_UPDATE			0x00
#define ENV_GET				0x01
#define COLLECT_INSERTER	0x10
#define COLLECT_TIME_GET	0x11
#define COLLECT_CURRENT_GET 0x12
#define GOODS_ADD			0x20
#define GOODS_REDUCE		0x21
#define GOODS_GET			0x22

extern void *pthread_sqlite (void *);			//数据库线程
extern void *pthread_analysis (void *);			//数据解析线程
extern void *pthread_transfer (void *);			//数据接收线程
extern void *pthread_client_send (void *);		//client send线程
extern void *pthread_uart_cmd (void *);			//命令发送线程
extern void *pthread_client_request (void *);    	//接收CGI，QT请求
extern void *pthread_infrared (void *);			//红外线程，用按键模拟
extern void *pthread_buzzer (void *);			//蜂鸣器控制线程
extern void *pthread_led (void *);			//LED控制线程
extern void *pthread_camera (void *);			//摄像头线程
extern void *pthread_sms (void *);			//发送短信线程
extern void *pthread_refresh (void *);	                //共享内存数据刷新线程

extern void sendMsgQueue (long, unsigned char);

struct storage_goods_info
{
	unsigned char goods_type;
	unsigned int goods_count;
};

struct storage_info
{
	unsigned char storage_status;				// 0:open 1:close
	unsigned char led_status;
	unsigned char buzzer_status;
	unsigned char fan_status;
	unsigned char seg_status;
	signed char x;
	signed char y;
	signed char z;
	char samplingTime[20];
	float temperature;
	float temperatureMIN;
	float temperatureMAX;
	float humidity;
	float humidityMIN;
	float humidityMAX;
	float illumination;
	float illuminationMIN;
	float illuminationMAX;
	float battery;
	float adc;
	float adcMIN;
	struct storage_goods_info goods_info[GOODS_NUM];
};

struct env_info_clien_addr
{
	struct storage_info storage_no[STORAGE_NUM];	
};

struct sqlite_operation
{
	int table_select_mask;
	int env_operation_mask;
	int table_operation_mask;
	int goods_operation_mask;
};

struct msg
{
	long type;
	long msgtype;
	unsigned char text[QUEUE_MSG_LEN];
};

#endif
