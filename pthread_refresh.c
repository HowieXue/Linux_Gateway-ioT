#include "sem.h"
#include "data_global.h"
#include "sqlite_link_list.h"

#define N 1024

extern pthread_mutex_t mutex_refresh;
extern pthread_mutex_t mutex_refresh_updata;
extern pthread_mutex_t mutex_global;
extern pthread_mutex_t mutex_slinklist;
extern pthread_cond_t cond_refresh;
extern pthread_cond_t cond_refresh_updata;
extern pthread_cond_t cond_sqlite;

extern char cgi_status;
extern char qt_status;
extern int shmid; 
extern int semid;
extern struct env_info_clien_addr all_info_RT;

struct shm_addr
{
	char cgi_status;
	char qt_status;
	struct env_info_clien_addr rt_status;
};

void *pthread_refresh (void *arg)
{
	key_t key_info;
	int shmid, semid;

	struct shm_addr *shm_buf;

	if ((key_info = ftok ("/app", 'i')) < 0)
	{
		perror ("ftok info");
		exit (-1);
	}
	printf ("key = %d\n", key_info);

	if ((semid = semget (key_info, 1, IPC_CREAT | IPC_EXCL |0666)) < 0)
	{
		if (errno == EEXIST)
		{
			semid = semget (key_info, 1, 0666);
		}
		else
		{
			perror ("semget");
			exit (-1);
		}
	}
	else
	{
		init_sem (semid, 0, 1);
	}

	if ((shmid = shmget (key_info, N, IPC_CREAT | IPC_EXCL | 0666)) < 0)
	{
		if (errno == EEXIST)
		{
			shmid = shmget (key_info, N, 0666);
			shm_buf = (struct shm_addr *)shmat (shmid, NULL, 0);
			
		}
		else
		{
			perror ("shmget");
			exit (-1);
		}

	}
	else
	{
		if ((shm_buf = (struct shm_addr *)shmat (shmid, NULL, 0)) == (void *)-1)
		{
			perror ("shmat");
			exit (-1);
		}
	}

	bzero (shm_buf, sizeof (struct shm_addr));
	printf ("pthread_refresh is ok\n");

	while (1)
	{
		pthread_mutex_lock (&mutex_refresh);
		pthread_cond_wait (&cond_refresh, &mutex_refresh);
//		printf ("pthread_refresh is wake up\n");
		sem_p (semid, 0);
		pthread_mutex_lock (&mutex_global);
		shm_buf->rt_status = all_info_RT;
		pthread_mutex_unlock (&mutex_global);
		sem_v (semid, 0);
		pthread_mutex_unlock (&mutex_refresh);
	}
	return 0;
}

