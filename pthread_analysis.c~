#include "data_global.h"
#include "link_list.h"
#include "sqlite_link_list.h"

struct getEnvMsg
{
	unsigned char sto_no;
	unsigned char tem[2];
	unsigned char hum[2];
	unsigned char x;
	unsigned char y;
	unsigned char z;
	unsigned int ill;
	unsigned int battery;
	unsigned int adc;
};

struct getGoodsMsg
{
	unsigned char sto_no;
	unsigned char io;
	unsigned char goodsno;
	unsigned char goodsnum;
};

extern linklist linkHead;
extern linklist slinkHead;

extern pthread_mutex_t mutex_linklist;
extern pthread_mutex_t mutex_slinklist;
extern pthread_mutex_t mutex_analysis;
extern pthread_mutex_t mutex_global;
extern pthread_mutex_t mutex_sms;
extern pthread_mutex_t mutex_buzzer;

extern pthread_cond_t cond_analysis;
extern pthread_cond_t cond_sqlite;
extern pthread_cond_t cond_refresh;
extern pthread_cond_t cond_buzzer;
extern pthread_cond_t cond_sms;

char tem_alarm_status[STORAGE_NUM] = {0};
char hum_alarm_status[STORAGE_NUM] = {0};
char ill_alarm_status[STORAGE_NUM] = {0};
char beep_status[STORAGE_NUM] = {0};

extern unsigned char dev_sms_mask;

extern int msgid;
extern int dev_buzzer_mask;
extern struct env_info_clien_addr all_info_RT;

float dota_atof (char unitl)
{
	if (unitl > 100)
	{
		return unitl / 1000.0;
	}
	else if (unitl > 10)
	{
		return unitl / 100.0;
	}
	else
	{
		return unitl / 10.0;
	}
}
int dota_atoi (const char * cDecade)
{
	int result = 0;
	if (' ' != cDecade[0])
	{
		result = (cDecade[0] - 48) * 10;
	}
	result += cDecade[1] - 48;
	return result;
}

float dota_adc (unsigned int ratio)
{
	return ((ratio * 3.6) / 1024);
}

int storageAllgood (int sto_no)
{
	if ((tem_alarm_status[sto_no] + hum_alarm_status[sto_no] + ill_alarm_status[sto_no]) == 0)
	{
		return 0;
	}
	return 1;
}

int checkEnv (int sto_no, struct storage_info *p)
{
	char flag = 0;
	static char a8_beep_status = 0;

	if (0 == tem_alarm_status[sto_no])
	{
		if (p->temperature > p->temperatureMAX)
		{
			sendMsgQueue (MSG_LED, MSG_LED_TEM_ON);
			sendMsgQueue (MSG_M0, MSG_M0_FAN_ON3 | sto_no << 6);
			p->fan_status = 3;
			tem_alarm_status[sto_no] = 2;
			flag = 1;
		}
		else if (p->temperature < p->temperatureMIN)
		{
			sendMsgQueue (MSG_LED, MSG_LED_TEM_ON);
			sendMsgQueue (MSG_M0, MSG_M0_FAN_OFF | sto_no << 6);
			p->fan_status = 0;
			tem_alarm_status[sto_no] = 1;
			flag = 1;
		}

		if (flag)
		{
			pthread_mutex_lock (&mutex_sms);
			dev_sms_mask = SMS_TEM;
			pthread_mutex_unlock (&mutex_sms);
			pthread_cond_signal (&cond_sms);
			flag = 0;
			if (beep_status[sto_no] == 0)
			{
				beep_status[sto_no] = 1;
				sendMsgQueue (MSG_M0, MSG_M0_BEEP_ON | sto_no << 6);
			}
			if (a8_beep_status == 0)
			{
				a8_beep_status = 1;
				sendMsgQueue (MSG_BEEP, MSG_M0_BEEP_ON);	
			}
		}
	}
	else 
	{
		if (p->temperature < p->temperatureMAX && p->temperature > p->temperatureMIN)
		{
			sendMsgQueue (MSG_LED, MSG_LED_TEM_OFF);
			sendMsgQueue (MSG_M0, MSG_M0_FAN_OFF | sto_no << 6);
			p->fan_status = 0;
			tem_alarm_status[sto_no] = 0;
			if (!storageAllgood (sto_no))
			{
				beep_status[sto_no] = 0;
				sendMsgQueue (MSG_M0, MSG_M0_BEEP_OFF | sto_no << 6);
				a8_beep_status = 0;
				sendMsgQueue (MSG_BEEP, MSG_BEEP_OFF);
			}
		}
	}

	if (0 == hum_alarm_status[sto_no])
	{
		if (p->humidity > p->humidityMAX)
		{
			sendMsgQueue (MSG_LED, MSG_LED_HUM_ON);
			sendMsgQueue (MSG_M0, MSG_M0_FAN_ON3 | sto_no << 6);
			p->fan_status = 3;
			hum_alarm_status[sto_no] = 2;
			flag = 1;
		}
		else if (p->humidity < p->humidityMIN)
		{
			sendMsgQueue (MSG_LED, MSG_LED_HUM_ON);
			sendMsgQueue (MSG_M0, MSG_M0_FAN_OFF | sto_no << 6);
			p->fan_status = 0;
			hum_alarm_status[sto_no] = 1;
			flag = 1;
		}

		if (flag)
		{
			pthread_mutex_lock (&mutex_sms);
			dev_sms_mask = SMS_HUM;
			pthread_mutex_unlock (&mutex_sms);
			pthread_cond_signal (&cond_sms);
			flag = 0;
			if (beep_status[sto_no] == 0)
			{
				beep_status[sto_no] = 1;
				sendMsgQueue (MSG_M0, MSG_M0_BEEP_ON | sto_no << 6);
			}
			if (a8_beep_status == 0)
			{
				a8_beep_status = 1;
				sendMsgQueue (MSG_BEEP, MSG_M0_BEEP_ON);	
			}

		}
	}
	else
	{
		if (p->humidity < p->humidityMAX && p->humidity > p->humidityMIN)
		{
			sendMsgQueue (MSG_LED, MSG_LED_HUM_OFF);
			sendMsgQueue (MSG_M0, MSG_M0_FAN_OFF | sto_no << 6);
			p->fan_status = 0;
			hum_alarm_status[sto_no] = 0;
			if (!storageAllgood (sto_no))
			{
				beep_status[sto_no] = 0;
				sendMsgQueue (MSG_M0, MSG_M0_BEEP_OFF | sto_no << 6);
				a8_beep_status = 0;
				sendMsgQueue (MSG_BEEP, MSG_BEEP_OFF);

			}
		}
	}

	if (0 == ill_alarm_status[sto_no])
	{
		if (p->illumination > p->illuminationMAX)
		{
			sendMsgQueue (MSG_LED, MSG_LED_ILL_ON);
			sendMsgQueue (MSG_M0, MSG_M0_LED_OFF | sto_no << 6);
			p->led_status = 0;
			ill_alarm_status[sto_no] = 2;
			flag = 1;
		}
		else if (p->illumination < p->illuminationMIN)
		{
			sendMsgQueue (MSG_LED, MSG_LED_ILL_ON);
			sendMsgQueue (MSG_M0, MSG_M0_LED_ON | sto_no << 6);
			p->led_status = 1;
			ill_alarm_status[sto_no] = 1;
			flag = 1;
		}

		if (flag)
		{
			pthread_mutex_lock (&mutex_sms);
			dev_sms_mask = SMS_ILL;
			pthread_mutex_unlock (&mutex_sms);
			pthread_cond_signal (&cond_sms);
			flag = 0;
			if (beep_status[sto_no] == 0)
			{
				beep_status[sto_no] = 1;
				sendMsgQueue (MSG_M0, MSG_M0_BEEP_ON | sto_no << 6);
			}
			if (a8_beep_status == 0)
			{
				a8_beep_status = 1;
				sendMsgQueue (MSG_BEEP, MSG_M0_BEEP_ON);	
			}
		}
	}
	else if (ill_alarm_status[sto_no])
	{
		if (p->illumination < p->illuminationMAX && p->illumination > p->illuminationMIN)
		{
			sendMsgQueue (MSG_LED, MSG_LED_ILL_OFF);
			sendMsgQueue (MSG_M0, MSG_M0_LED_OFF | sto_no << 6);
			p->led_status = 0;
			ill_alarm_status[sto_no] = 0;
			if (!storageAllgood (sto_no))
			{
				beep_status[sto_no] = 0;
				sendMsgQueue (MSG_M0, MSG_M0_BEEP_OFF | sto_no << 6);
				a8_beep_status = 0;
				sendMsgQueue (MSG_BEEP, MSG_BEEP_OFF);
			}
		}
	}



	return 0;
}

void getEnvPackage (link_datatype *buf)
{
	struct getEnvMsg pack;
	memcpy (&pack, buf->text, 20);
	int sto_no = pack.sto_no;

	pthread_mutex_lock (&mutex_global);
	struct storage_info current = all_info_RT.storage_no[sto_no];
	pthread_mutex_unlock (&mutex_global);

	current.storage_status = 1;
	current.x = pack.x;
	current.y = pack.y;
	current.z = pack.z;
	current.temperature = pack.tem[0] + dota_atof (pack.tem[1]);
	current.humidity = pack.hum[0] + dota_atof (pack.hum[1]);
	current.illumination = pack.ill;
	current.battery = dota_adc (pack.battery);
	current.adc = dota_adc (pack.adc);

	printf ("no = %d tem = %0.2f hum = %0.2f ill = %0.2f battery = %0.2f adc = %0.2f x = %d y = %d z = %d\n", sto_no,
			current.temperature, current.humidity, current.illumination, current.battery, current.adc, current.x, current.y, current.z);

	checkEnv (sto_no, &current);	

	pthread_mutex_lock (&mutex_global);
	all_info_RT.storage_no[sto_no] = current;
	pthread_mutex_lock (&mutex_slinklist);
	sqlite_InsertLinknode (COLLECT_INSERTER, all_info_RT, sto_no, 0);//0,0分别是仓库号和货物种类号
	pthread_mutex_unlock (&mutex_slinklist);
	pthread_mutex_unlock (&mutex_global);

	pthread_cond_signal (&cond_refresh);
	pthread_cond_signal (&cond_sqlite);


	return ;
}

void getGoodsPackage (link_datatype *buf)
{
	struct getGoodsMsg pack;
	memcpy (&pack, buf->text, 16);
	int sto_no = pack.sto_no;

	pthread_mutex_lock (&mutex_global);
	struct storage_info current = all_info_RT.storage_no[sto_no];
	pthread_mutex_unlock (&mutex_global);

	current.storage_status = 1;
	current.goods_info[pack.goodsno].goods_type = pack.goodsno;
	current.goods_info[pack.goodsno].goods_count = pack.goodsnum;

	printf ("sto_no = %d, io = %c goods_type = %d, goods_num = %d\n", sto_no, pack.io, current.goods_info[pack.goodsno].goods_type, current.goods_info[pack.goodsno].goods_count);	

	pthread_mutex_lock (&mutex_global);
	all_info_RT.storage_no[sto_no] = current;
	pthread_mutex_lock (&mutex_slinklist);
	if (pack.io == 'I')
	{
		sqlite_InsertLinknode (GOODS_ADD, all_info_RT, sto_no, pack.goodsno);//0,0分别是仓库号和货物种类号
	}
	else if (pack.io == 'O')
	{
		sqlite_InsertLinknode (GOODS_REDUCE, all_info_RT, sto_no, pack.goodsno);//0,0分别是仓库号和货物种类号
	}
	pthread_mutex_unlock (&mutex_slinklist);
	pthread_mutex_unlock (&mutex_global);

	pthread_cond_signal (&cond_refresh);
	pthread_cond_signal (&cond_sqlite);
	return ;
}


void *pthread_analysis (void *arg)
{
	linklist node;
	link_datatype buf;
	printf ("pthread_analysis is ok\n");
	while (1)
	{
		pthread_mutex_lock (&mutex_analysis);
		pthread_cond_wait (&cond_analysis, &mutex_analysis);
		pthread_mutex_unlock (&mutex_analysis);

//		printf ("wake pthread_analysis wake up\n");
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

			if ('e' == buf.msg_type)
			{
				getEnvPackage (&buf);
			}
			else if ('r' == buf.msg_type)
			{
				getGoodsPackage (&buf);
			}
		}
	}
	return 0;
}
