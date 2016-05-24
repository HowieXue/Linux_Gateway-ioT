#include "data_global.h"

extern unsigned char dev_led_mask;
extern int dev_led_fd;

extern pthread_mutex_t mutex_led;

extern pthread_cond_t cond_led;

void *pthread_led (void *arg)
{
	int led_no;
	unsigned char led_set;
	if ((dev_led_fd = open (DEV_LED, O_RDWR | O_NONBLOCK)) < 0 )
	{
		printf ("Cann't open file /dev/led\n");
		exit (-1);
	}
	printf ("pthread_led is ok\n");
	while (1)
	{
		pthread_mutex_lock (&mutex_led);
		pthread_cond_wait (&cond_led, &mutex_led);
		led_set = dev_led_mask;
		pthread_mutex_unlock (&mutex_led);
//		printf ("pthread_led is wake up\n");
		switch (led_set & 0xf0)
		{
			case 0x10:
				{
					led_no = 0;
					break;
				}
			case 0x20:
				{
					led_no = 1;
					break;
				}
			case 0x40:
				{
					led_no = 2;
					break;
				}
			case 0x80:
				{
					led_no = 3;
					break;
				}
			default :break;
		}
		led_set &= 0x0f;
		if (led_set & (0x1 << led_no))
		{
			ioctl (dev_led_fd, LED_ON, led_no);
		}
		else
		{
			ioctl (dev_led_fd, LED_OFF, led_no);
		}
	}
}
