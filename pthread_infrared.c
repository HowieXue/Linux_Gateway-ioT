#include "data_global.h"


extern char dev_infrared_mask;
extern int dev_infrared_fd;
extern int dev_sms_mask;

extern pthread_mutex_t mutex_infrared;
extern pthread_mutex_t mutex_sms;

extern pthread_cond_t cond_infrared;
extern pthread_cond_t cond_sms;

void *pthread_infrared (void *arg)
{
	int len;
	char buf;
	char check = 'x';
	if ((dev_infrared_fd = open (DEV_INFRARED, O_RDWR)) < 0)
	{
		perror ("open infrared");
		exit (-1);
	}
	printf ("pthread_infrared is ok\n");
	while (1)
	{
		buf = 0;
		len = read (dev_infrared_fd, &buf, sizeof (char));
		printf ("Storage %d has been break in\n", buf);
		if (check != buf)
		{
			check = buf;
			pthread_mutex_lock (&mutex_sms);
			dev_sms_mask = SMS_BRE;
			pthread_mutex_unlock (&mutex_sms);
			pthread_cond_signal (&cond_sms);
			sendMsgQueue (3L, 9);
		}
		
	}

	return 0;
}
