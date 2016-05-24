#include "data_global.h"

extern unsigned char dev_uart_mask;
extern int dev_uart_fd;

extern pthread_cond_t cond_uart_cmd;

extern pthread_mutex_t mutex_uart_cmd;

extern dota_cache_list m0_cache_head, m0_cache_tail;

void *pthread_uart_cmd (void *arg)
{
	unsigned char *uart_p = NULL;
	dota_cache_list uart_cache_p = NULL;
	printf ("pthread_uart_cmd is ok\n");
	while (1)
	{
		pthread_mutex_lock (&mutex_uart_cmd);
		pthread_cond_wait (&cond_uart_cmd, &mutex_uart_cmd);
		while ((uart_cache_p = GetCacheNode (m0_cache_head, &m0_cache_tail)) != NULL)
		{
			pthread_mutex_unlock (&mutex_uart_cmd);
			uart_p = (unsigned char *)uart_cache_p->data;
			dev_uart_mask = *uart_p;
			write (dev_uart_fd, &dev_uart_mask, 1);
			free (uart_p);
			uart_p = NULL;
			free (uart_cache_p);
			uart_cache_p = NULL;
			usleep (200000);
			pthread_mutex_lock (&mutex_uart_cmd);
		}
		pthread_mutex_unlock (&mutex_uart_cmd);
	}
}
