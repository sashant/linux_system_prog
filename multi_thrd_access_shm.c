#include<stdio.h>
#include<stdlib.h>
#include<pthread.h>
#include<sys/ipc.h>
#include<sys/shm.h>
#include<sys/types.h>
#include<unistd.h>
#define NUM 50

struct thr_arg
{
	int x;
	int *sh_mem;
};

//char *shm;
pthread_mutex_t lock;

void *thr_func(void *arg)
{
	int i;
	pthread_mutex_lock(&lock);
	i = ((struct thr_arg *)arg)->x;
	//printf("%d, ",i);
	((struct thr_arg *)arg)->sh_mem[i-1] = i;
	pthread_mutex_unlock(&lock);
	//printf("writing success\n");
	return ;
}

int main()
{
	pthread_t tid[NUM];
	struct thr_arg ta[NUM];
	int *shm;
	int key, shmid, i;
        if (pthread_mutex_init(&lock, NULL) != 0)
	{
		printf("\nerror in mutex init");
	}
	key = ftok("/dev/null", 0);
	shmid = shmget(key, sizeof(int)*NUM, IPC_CREAT | 0666);
        if (shmid < 0)
	{
		perror("shmget");
		return 1;
	}
	
	shm = (int *)shmat(shmid, NULL, 0);
	if (shm == NULL)
	{
		perror("shmat");
		return 1;
	}
	
	for (i = 0; i < NUM; i++)
	{
		ta[i].x = i+1;
		ta[i].sh_mem = shm;
		//printf("%d: ",ta[i].x);
		pthread_create(&tid[i], NULL, thr_func, &ta[i]);
	}
	for (i = 0; i<NUM; i++)
	{
		pthread_join(tid[i],NULL);
	}
	//printf("\n\n");
	for(i = 0; i<NUM; i++)
	{
		printf("%d, ",shm[i]);
	}
	//shm[i]='\0';
	//printf("%s\n\n",shm);
	printf("\n");
	shmdt((void *)shm);
	shmctl(shmid, IPC_RMID,NULL);
	return 0;
}
