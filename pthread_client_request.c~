#include "data_global.h"
#include "dota_cache.h"
#include "sqlite_link_list.h"

extern unsigned char dev_led_mask;
extern unsigned char dev_camera_mask;
extern unsigned char dev_buzzer_mask;
extern unsigned char dev_uart_mask;

extern pthread_cond_t cond_led;
extern pthread_cond_t cond_camera;
extern pthread_cond_t cond_buzzer;
extern pthread_cond_t cond_refresh;
extern pthread_cond_t cond_uart_cmd;
extern pthread_cond_t cond_sqlite;

extern pthread_mutex_t mutex_global;
extern pthread_mutex_t mutex_uart_cmd;
extern pthread_mutex_t mutex_led;
extern pthread_mutex_t mutex_buzzer;
extern pthread_mutex_t mutex_camera;
extern pthread_mutex_t mutex_slinklist;

extern char cgi_status;
extern int msgid;
extern struct env_info_clien_addr all_info_RT; 

extern dota_cache_list m0_cache_head, m0_cache_tail;
extern char recive_phone[12] ;
extern char center_phone[12] ;
struct setEnv
{
	int temMAX;
	int temMIN;
	int humMAX;
	int humMIN;
	int illMAX;
	int illMIN;
};



void *pthread_client_request (void *arg)
{
	key_t key;
	ssize_t msgsize;
	struct msg msgbuf;
	struct setEnv new;
	int sto_no;

	if ((key = ftok ("/app", 'g')) < 0)
	{
		perror ("ftok msgqueue");
		exit (-1);
	}
	if ((msgid = msgget (key, IPC_CREAT | IPC_EXCL | 0666)) < 0)
	{
		perror ("msgget msgid");
		exit (-1);
	}

	m0_cache_head = CreateEmptyCacheList ();
	m0_cache_tail = m0_cache_head;
	unsigned char *m0_temp;

	printf ("pthread_client_request is ok\n");
	while (1)
	{
		bzero (&msgbuf, sizeof (msgbuf));
		//		printf ("wait for the msg\n");
		msgsize = msgrcv (msgid, &msgbuf, sizeof (msgbuf) - sizeof (long), 1L, 0);
		//		printf ("Get %ldL msg\n", msgbuf.msgtype);
		//		printf ("text[0] = %#x\n", msgbuf.text[0]);

		switch (msgbuf.msgtype)
		{
		case 1L:
			{
				pthread_mutex_lock (&mutex_led);
				dev_led_mask = msgbuf.text[0];
				pthread_cond_signal (&cond_led);
				pthread_mutex_unlock (&mutex_led);
				break;
			}
		case 2L:
			{
				pthread_mutex_lock (&mutex_buzzer);
				dev_buzzer_mask = msgbuf.text[0];
				printf("msgbuf.text[0] = %d \n",msgbuf.text[0]);
				pthread_cond_signal (&cond_buzzer);
				pthread_mutex_unlock (&mutex_buzzer);
				break;
			}
		case 3L:
			{
				pthread_mutex_lock (&mutex_camera);
				dev_camera_mask = msgbuf.text[0];
				pthread_cond_signal (&cond_camera);
				pthread_mutex_unlock (&mutex_camera);
				break;
			}
		case 4L:
			{
				//					usleep (200000);
				pthread_mutex_lock (&mutex_uart_cmd);	
				m0_temp = (unsigned char *)malloc (sizeof (unsigned char));
				*m0_temp = msgbuf.text[0];
				printf("msgbuf.text = %d\n",msgbuf.text[0]);
				InsertCacheNode (&m0_cache_tail, m0_temp);
				//					dev_uart_mask = msgbuf.text[0];
				pthread_mutex_unlock (&mutex_uart_cmd);
				pthread_cond_signal (&cond_uart_cmd);
				break;
			}
		case 5L:
			{
				memcpy (&new, msgbuf.text + 1, 24);
				sto_no = msgbuf.text[0] - 48;
				printf ("sto_no = %d temMAX = %d, temMIN = %d, humMAX = %d, hunMIN = %d, illMAX = %d, illMIN = %d\n",
						sto_no, new.temMAX, new.temMIN, new.humMAX, new.humMIN, new.illMAX, new.illMIN);
				pthread_mutex_lock (&mutex_global);
				if (new.temMAX > 0 && new.temMAX > all_info_RT.storage_no[sto_no].temperatureMIN)
				{
					all_info_RT.storage_no[sto_no].temperatureMAX = new.temMAX;
				}
				if (new.temMIN > 0 && new.temMIN < all_info_RT.storage_no[sto_no].temperatureMAX)
				{
					all_info_RT.storage_no[sto_no].temperatureMIN = new.temMIN;
				}
				if (new.humMAX > 0 && new.humMAX > all_info_RT.storage_no[sto_no].humidityMIN)
				{
					all_info_RT.storage_no[sto_no].humidityMAX = new.humMAX;
				}
				if (new.humMIN > 0 && new.humMIN < all_info_RT.storage_no[sto_no].temperatureMAX)
				{
					all_info_RT.storage_no[sto_no].humidityMIN = new.humMIN;
				}
				if (new.illMAX > 0 && new.illMAX > all_info_RT.storage_no[sto_no].illuminationMIN)
				{
					all_info_RT.storage_no[sto_no].illuminationMAX = new.illMAX;
				}
				if (new.illMIN > 0 && new.illMIN < all_info_RT.storage_no[sto_no].illuminationMAX)
				{
					all_info_RT.storage_no[sto_no].illuminationMIN = new.illMIN;
				}
				pthread_mutex_lock (&mutex_slinklist);
					sqlite_InsertLinknode (ENV_UPDATE, all_info_RT, sto_no, 0);//0,0分别是仓库号和货物种类号
				pthread_mutex_unlock (&mutex_slinklist);
				pthread_cond_signal (&cond_sqlite);
				pthread_mutex_unlock (&mutex_global);
				pthread_cond_signal (&cond_refresh);
				break;
			}
#if 1
		case 10L:
			{
				int i = 0 , j = 0 ;
				for(i = 0  ; i < 11; i++)
				{
					recive_phone[i] = msgbuf.text[i]; 	
				}
				recive_phone[i] = '\0';
				printf("**************************recive:%s\n",recive_phone);
				for(j = 0 ;msgbuf.text[i] != '\0' && j < 12; i++, j++)
				{
					center_phone[j] =  msgbuf.text[i];
				}
				center_phone[j] = '\0';
				printf("**************************center:%s\n",center_phone);
				printf("**************************num change over\n");
				break;
			}


#endif 
#if 1
		case 11L:
			{
				char tmp[100] ={0};
strcpy(tmp,msgbuf.text);
	char tmp1[100] = {0};
	char tmp2[100] = {0};
	int i = 0, j= 0;
	for(i = 0 ; tmp[i] != 'f'; i++)
	{
		tmp1[i] = tmp[i];	
	}
	tmp1[i] = '\0';
	printf("tmp1 : %s\n",tmp1);
	i++;
	for(j=0 ; tmp[i] != '\0'; i++, j++)
	{
		tmp2[j] = tmp[i];
	}
	tmp2[j] = '\0';
	printf("tmp2 : %s\n",tmp2);
	system("mkdir /var/run/wpa_supplicant -p");
	char ifcon[100] = "ifconfig wlan0 ";
	strcat(ifcon,tmp1);
	printf("string : %s\n",ifcon);
     	system(ifcon);
        char gw[100] = "route add default gw ";
	strcat(gw,tmp2);
        system(gw);
	printf("gw : %s\n",gw);
		system("wpa_supplicant -B -iwlan0 -c /etc/wpa-psk-tkip.conf");
	printf("wifi is ok\n");
				break;
			}


#endif 




		default : break;
		}
	}

}
