#include "data_global.h"

extern int dev_buzzer_fd;
extern char dev_buzzer_mask;

extern pthread_mutex_t mutex_buzzer;

extern pthread_cond_t cond_buzzer;

void *pthread_buzzer (void *arg)
{
	unsigned char set_buzzer;
	if ((dev_buzzer_fd = open (DEV_BUZZER, O_RDWR | O_NONBLOCK)) < 0)
	{
		printf ("Cann't open file /dev/beep\n");
		exit (-1);
	}
	printf ("pthread_buzzer is ok\n");
	while (1)
	{
		pthread_mutex_lock (&mutex_buzzer);
		pthread_cond_wait (&cond_buzzer, &mutex_buzzer);
		set_buzzer = dev_buzzer_mask;
		printf ("pthread_buzzer is wake up\n");
		if (0 == set_buzzer)
		{
			ioctl (dev_buzzer_fd, BUZZER_OFF, 0);
			printf("beep off\n");
		}
		else
		{
			printf("beep on\n");
			ioctl (dev_buzzer_fd, BUZZER_ON, 0);
		}
		pthread_mutex_unlock (&mutex_buzzer);
	}

	return 0;
}
