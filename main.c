#include <stdio.h>
#include <pthread.h>

pthread_mutex_t reinit_fifo_lock = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t	reinit_fifo_done_cond = PTHREAD_COND_INITIALIZER;
static int reinit_fifo_symbol = 0;

void * recv_cmd(void * p)
{
	pthread_mutex_lock(&reinit_fifo_lock);
	reinit_fifo_symbol =  1;
	printf("%s , %d , reinit_fifo_symbol = 1 \n" , __func__ , __LINE__);
	pthread_mutex_unlock(&reinit_fifo_lock);
	printf("%s , %d ,done \n" , __func__ , __LINE__);

}

void * read_pipe(void * p)
{
	while(1)
	{
		if(reinit_fifo_symbol == 1)
		{
			pthread_mutex_lock(&reinit_fifo_lock);
			while(reinit_fifo_symbol > 0)
			{
				printf("%s , %d in wait \n" , __func__ , __LINE__);
				reinit_fifo_symbol = 2;
				pthread_cond_wait(&reinit_fifo_done_cond , &reinit_fifo_lock);
			}

			pthread_mutex_unlock(&reinit_fifo_lock);
			printf("%s , %d ,done \n" , __func__ , __LINE__);
		}

		sleep(1);

	}
}

void * read_fifo(void * p)
{
	while(1)
	{
		if(reinit_fifo_symbol == 2)
		{
			printf("%s , %d in reinit \n" , __func__ , __LINE__);

			pthread_mutex_lock(&reinit_fifo_lock);
			//reinit_fifo();
			//reinit_empty_fifo();
			//read_pipe_64k();
			reinit_fifo_symbol =  0;
			pthread_cond_signal(&reinit_fifo_done_cond);
			pthread_mutex_unlock(&reinit_fifo_lock);
			printf("%s , %d ,done \n" , __func__ , __LINE__);
		}

		sleep(1);
	}
}



int main()
{
	int i;
	pthread_t tid[3];

	pthread_create(&tid[0] , NULL , recv_cmd , NULL);
	pthread_create(&tid[1] , NULL , read_pipe , NULL);
	pthread_create(&tid[2] , NULL , read_fifo , NULL);

	//	for(i = 0 ; i < 3 ; i++)
	//		pthread_join(&tid[i],NULL);
	for( ; ; )
	{
		printf("symbol : %d\n" , reinit_fifo_symbol);
		sleep(1);
	}

	return 0;
}
