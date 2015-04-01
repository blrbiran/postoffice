/*
 * Demonstrate use of semaphores for synchronization.
 */
#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <semaphore.h>
 #include <time.h>

#define NUM_CUSTOMERS 50

void enqueue1(int cust_num);
void dequeue1(int *cust_num);

void enter_office(int cust_num);
void leave_postal_chair(int cust_num, int post_type);
void exit_office(int cust_num);

void post(int worker_num, int cust_num, int post_type);

void print_exit(char *s);

void SetRandSeed(void);
int GetRandom(void);

void *customer (void* arg);
void *worker(void* arg);
void *cashier(void* arg);

/* workershop 2 */
// sem_t max_capacity = 10;
// sem_t postal_chair = 3;
// sem_t mutex = 1;
// sem_t scales = 1;
// sem_t cust_ready = 0, leave_p_chair = 0;
// sem_t finished[50] = {0};
sem_t max_capacity;
sem_t postal_chair;
sem_t mutex;
sem_t scales;
sem_t cust_ready, leave_p_chair;
sem_t finished[NUM_CUSTOMERS] ;
int count;

int queue1[100] = {0};
int queue1_ptr = 0;


void enqueue1(int cust_num)
{
	queue1[queue1_ptr++] = cust_num;
}

void dequeue1(int *cust_num)
{
	*cust_num = queue1[--queue1_ptr];
}


void enter_office(int cust_num)
{
	printf("\r\nCustomer %d enter post office", cust_num);
}

void leave_postal_chair(int cust_num, int post_type)
{
	switch(post_type)
	{
	case 1:
		printf("\r\nCustomer %d finished buying stamps", cust_num);
		break;
	case 2:
		printf("\r\nCustomer %d finished mailing a letter", cust_num);
		break;
	case 3:
		printf("\r\nCustomer %d finished mailing a package", cust_num);
		break;
	default:
		break;
	}
}

void exit_office(int cust_num)
{
	printf("\r\nCustomer %d leaves post office", cust_num);
}

// void post(int worker_num, int cust_num, int post_type)
// {
// 	printf("\r\nPostal worker %d serving customer %d", worker_num, cust_num);

// 	switch(post_type)
// 	{
// 	case 1:
// 		printf("\r\nCustomer %d asks postal worker %d to buy stamps", cust_num, worker_num);
// 		sleep(1);
// 		break;
// 	case 2:
// 		printf("\r\nCustomer %d asks postal worker %d to mail a letter", cust_num, worker_num);
// 		sleep(1);
// 		break;
// 	case 3:
// 		printf("\r\nCustomer %d asks postal worker %d to mail a package", cust_num, worker_num);

// 		if (sem_wait (&scales) == -1)
// 			print_exit("Error in wait on customer\n");

// 		printf("\r\nScales in use by postal worker %d", worker_num);
// 		sleep(2);
// 		printf("\r\nScales released by postal worker %d", worker_num);

// 		if (sem_post (&scales) == -1)
// 			print_exit("Error in post on customer\n");
// 		break;
// 	default:
// 		break;
// 	}

// 	printf("\r\nPostal worker %d finished serving customer %d", worker_num, cust_num);

// }



void print_exit(char *s)
{
	printf("%s",s);
	exit(1);
}

void SetRandSeed(void)
{
	srand((unsigned int)time(NULL));
}

int GetRandom(void)
{
	int n;
	n = rand()%3 + 1;
	return n;
}

/*
 * Thread function to wait on a semaphore.
 */
void *customer (void *arg)
{
	int *pnum = (int *)arg;
	int cust_num = *pnum;
	free(arg);

	int post_type = GetRandom();	// 1. buy stamps 2. mail a letter 3. mail a package

	printf("\r\nCustomer %d created", cust_num);

	if (sem_wait (&max_capacity) == -1)
		print_exit("Error in wait on customer\n");

	enter_office(cust_num);

	if (sem_wait (&postal_chair) == -1)
		print_exit("Error in wait on customer\n");

	// sit_in_postal_chair(cust_num);

	if (sem_wait (&mutex) == -1)
		print_exit("Error in wait on customer\n");

	enqueue1(cust_num);
	enqueue1(post_type);

	if (sem_post (&cust_ready) == -1)
		print_exit("Error in post on customer\n");

	if (sem_post (&mutex) == -1)
		print_exit("Error in post on customer\n");

	if (sem_wait (&finished[cust_num]) == -1)
		print_exit("Error in wait on customer\n");

	leave_postal_chair(cust_num, post_type);

	if (sem_post (&leave_p_chair) == -1)
		print_exit("Error in post on customer\n");

	exit_office(cust_num);

	if (sem_post (&max_capacity) == -1)
		print_exit("Error in post on customer\n");

	return NULL;
}

void *worker(void* arg)
{
	int *pnum = (int *)arg;
	int worker_num = *pnum;
	free(arg);

	int cust_num;
	int post_type;

	printf("\r\nWorker %d created", worker_num);

	while(1)
	{
		if (sem_wait (&cust_ready) == -1)
			print_exit("Error in wait on worker\n");

		if (sem_wait (&mutex) == -1)
			print_exit("Error in wait on worker\n");

		dequeue1(&post_type);
		dequeue1(&cust_num);

		if (sem_post (&mutex) == -1)
			print_exit("Error in post on worker\n");

		// post(worker_num, cust_num, post_type);
		printf("\r\nPostal worker %d serving customer %d", worker_num, cust_num);

		switch(post_type)
		{
		case 1:
			printf("\r\nCustomer %d asks postal worker %d to buy stamps", cust_num, worker_num);
			sleep(1);
			break;
		case 2:
			printf("\r\nCustomer %d asks postal worker %d to mail a letter", cust_num, worker_num);
			sleep(1.5);
			break;
		case 3:
			printf("\r\nCustomer %d asks postal worker %d to mail a package", cust_num, worker_num);

			if (sem_wait (&scales) == -1)
				print_exit("Error in wait on customer\n");

			printf("\r\nScales in use by postal worker %d", worker_num);
			sleep(2);
			printf("\r\nScales released by postal worker %d", worker_num);

			if (sem_post (&scales) == -1)
				print_exit("Error in post on customer\n");
			break;
		default:
			break;
		}

		printf("\r\nPostal worker %d finished serving customer %d", worker_num, cust_num);

		if (sem_post (&finished[cust_num]) == -1)
			print_exit("Error in post on worker\n");

		if (sem_wait (&leave_p_chair) == -1)
			print_exit("Error in wait on worker\n");

		if (sem_post (&postal_chair) == -1)
			print_exit("Error in post on worker\n");

	}
}


int main (int argc, char *argv[])
{
	int thread_count;
	// pthread_t sem_cashier[1];
	pthread_t sem_worker[3];
	pthread_t sem_customer[NUM_CUSTOMERS];
	int status;

	SetRandSeed();		// set random seed for GetRandom()

// sem_t max_capacity = 10;
// sem_t postal_chair = 3;
// sem_t mutex = 1;
// sem_t scales = 1;
// sem_t cust_ready = 0, leave_p_chair = 0;
// sem_t finished[50] = {0};

	/* initialize semaphore to 0 (3rd parameter) */
	if (sem_init (&max_capacity, 0, 10) == -1)
		print_exit("Error in init semaphore\n");
	if (sem_init (&postal_chair, 0, 3) == -1)
		print_exit("Error in init semaphore\n");
	if (sem_init (&mutex, 0, 1) == -1)
		print_exit("Error in init semaphore\n");
	if (sem_init (&scales, 0, 1) == -1)
		print_exit("Error in init semaphore\n");
	if (sem_init (&cust_ready, 0, 0) == -1)
		print_exit("Error in init semaphore\n");
	if (sem_init (&leave_p_chair, 0, 0) == -1)
		print_exit("Error in init semaphore\n");
	for (thread_count = 0; thread_count < NUM_CUSTOMERS; thread_count++)
		if (sem_init (&finished[thread_count], 0, 0) == -1)
			print_exit("Error in init semaphore\n");


	/*
	* Create n threads to wait concurrently on the semaphore.
	*/
	for (thread_count = 0; thread_count < 3; thread_count++)
	{
		int *pnum = (int*)malloc(sizeof(int));
		*pnum = thread_count;

		status = pthread_create (
			&sem_worker[thread_count], NULL,
			worker, (void*)pnum);
		if (status != 0)
		{
			printf("Create thread\n");
			exit(1);
		}
	}

	for (thread_count = 0; thread_count < NUM_CUSTOMERS; thread_count++)
	{
		int *pnum = (int*)malloc(sizeof(int));
		*pnum = thread_count;

		status = pthread_create (
			&sem_customer[thread_count], NULL,
			customer, (void*)pnum);
		if (status != 0)
		{
			printf("Create thread\n");
			exit(1);
		}
	}


	/* pause before releasing threads */
	// sleep (2);

	


	/*
	* Wait for all threads to complete.
	*/


	for (thread_count = 0; thread_count < NUM_CUSTOMERS; thread_count++)
	{
		status = pthread_join (sem_customer[thread_count], NULL);
		if (status != 0)
		{
			printf("Error in join thread\n");
			exit(1);
		}
		else
			printf("\r\nJoined customer %d", thread_count);
	}
	
	



	return 0;
}

