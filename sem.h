#ifndef __SEM_H__
#define __SEM_H__

#include "data_global.h"

union semun {
	int              val;    /* Value for SETVAL */
	struct semid_ds *buf;    /* Buffer for IPC_STAT, IPC_SET */
	unsigned short  *array;  /* Array for GETALL, SETALL */
	struct seminfo  *__buf;  /* Buffer for IPC_INFO
								(Linux-specific) */
};

int init_sem(int semid, int num, int val)
{
	union semun myun;
	myun.val = val;
	if(semctl(semid, num, SETVAL, myun) < 0)
	{
		perror("semctl");
		exit(1);
	}
	return 0;
}

int sem_p(int semid, int num)
{
	struct sembuf mybuf;
	mybuf.sem_num = num;
	mybuf.sem_op = -1;
	mybuf.sem_flg = SEM_UNDO;
	if(semop(semid, &mybuf, 1) < 0)
	{
		perror("semop");
		exit(1);
	}

	return 0;
}

int sem_v(int semid, int num)
{
	struct sembuf mybuf;
	mybuf.sem_num = num;
	mybuf.sem_op = 1;
	mybuf.sem_flg = SEM_UNDO;
	if(semop(semid, &mybuf, 1) < 0)
	{
		perror("semop");
		exit(1);
	}

	return 0;
}

#endif
