/*
 * Demonstrate use of semaphores for synchronization.
 */
#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <semaphore.h>

#define NUM_CUSTOMERS 50

void enqueue1(int custnr);
void dequeue1(int *custnr);

void enter_shop();
void sit_on_sofa(int custnr);
void get_up_from_sofa(int custnr);
void sit_in_barber_chair(int custnr);
void leave_barber_chair(int custnr);
void pay(int custnr);
void exit_shop(int custnr);
void cut_hair(int custnr);
void accept_pay(void);

void print_exit(char *s);
void *customer (void* arg);
void *barber(void* arg);
void *cashier(void* arg);

/* barbershop 2 */
// sem_t max_capacity = 20;
// sem_t sofa = 4;
// sem_t barber_chair = 3, coord = 3;
// sem_t mutex1 = 1, mutex2 = 1;
// sem_t cust_ready = 0, leave_b_chair = 0, payment = 0, receipt = 0;
// sem_t finished[50] = {0};
sem_t max_capacity;
sem_t sofa;
sem_t barber_chair, coord;
sem_t mutex1, mutex2;
sem_t cust_ready, leave_b_chair, payment, receipt;
sem_t finished[NUM_CUSTOMERS] ;
int count;

int queue1[NUM_CUSTOMERS] = {0};
int queue1_ptr = 0;


void enqueue1(int custnr)
{
	queue1[queue1_ptr++] = custnr;
}

void dequeue1(int *custnr)
{
	*custnr = queue1[--queue1_ptr];
}


void enter_shop()
{
	printf("\r\nnew customer enter shop");
}

void sit_on_sofa(int custnr)
{
	printf("\r\n%d sit on sofa", custnr);
}

void get_up_from_sofa(int custnr)
{
	printf("\r\n%d get up from sofa", custnr);
}

void sit_in_barber_chair(int custnr)
{
	printf("\r\n%d sit in barber chair", custnr);
}

void leave_barber_chair(int custnr)
{
	printf("\r\n%d leave barber chair", custnr);
}

void pay(int custnr)
{
	printf("\r\n%d pay", custnr);
}

void exit_shop(int custnr)
{
	printf("\r\n%d exit shop", custnr);
}

void cut_hair(int custnr)
{
	printf("\r\n%d cut hair", custnr);
}

void accept_pay(void)
{
	printf("\r\naccept pay");
}


void print_exit(char *s)
{
	printf("%s",s);
	exit(1);
}

/*
 * Thread function to wait on a semaphore.
 */
void *customer (void* arg)
{
	int custnr;

	if (sem_wait (&max_capacity) == -1)
		print_exit("wrong wait on customer\n");

	enter_shop();

	if (sem_wait (&mutex1) == -1)
		print_exit("wrong wait on customer\n");

	count++;
	custnr = count;

	if (sem_post (&mutex1) == -1)
		print_exit("wrong post on customer\n");

	if (sem_wait (&sofa) == -1)
		print_exit("wrong wait on customer\n");

	sit_on_sofa(custnr);

	if (sem_wait (&barber_chair) == -1)
		print_exit("wrong wait on customer\n");

	get_up_from_sofa(custnr);

	if (sem_post (&sofa) == -1)
		print_exit("wrong post on customer\n");

	sit_in_barber_chair(custnr);

	if (sem_wait (&mutex2) == -1)
		print_exit("wrong wait on customer\n");

	enqueue1(custnr);

	if (sem_post (&cust_ready) == -1)
		print_exit("wrong post on customer\n");

	if (sem_post (&mutex2) == -1)
		print_exit("wrong post on customer\n");

	if (sem_wait (&finished[custnr]) == -1)
		print_exit("wrong wait on customer\n");

	leave_barber_chair(custnr);

	if (sem_post (&leave_b_chair) == -1)
		print_exit("wrong post on customer\n");

	pay(custnr);

	if (sem_post (&payment) == -1)
		print_exit("wrong post on customer\n");

	if (sem_wait (&receipt) == -1)
		print_exit("wrong wait on customer\n");

	exit_shop(custnr);

	if (sem_post (&max_capacity) == -1)
		print_exit("wrong post on customer\n");

	return NULL;
}

void *barber(void* arg)
{
	int b_cust;

	while(1)
	{
		if (sem_wait (&cust_ready) == -1)
			print_exit("wrong wait on barber\n");

		if (sem_wait (&mutex2) == -1)
			print_exit("wrong wait on barber\n");

		dequeue1(&b_cust);

		if (sem_post (&mutex2) == -1)
			print_exit("wrong post on barber\n");

		if (sem_wait (&coord) == -1)
			print_exit("wrong wait on barber\n");

		cut_hair(b_cust);

		if (sem_post (&coord) == -1)
			print_exit("wrong post on barber\n");

		if (sem_post (&finished[b_cust]) == -1)
			print_exit("wrong post on barber\n");

		if (sem_wait (&leave_b_chair) == -1)
			print_exit("wrong wait on barber\n");

		if (sem_post (&barber_chair) == -1)
			print_exit("wrong post on barber\n");
	}
}

void *cashier(void* arg)
{

	while(1)
	{
		if (sem_wait (&payment) == -1)
			print_exit("wrong wait on cashier\n");

		if (sem_wait (&coord) == -1)
			print_exit("wrong wait on cashier\n");

		accept_pay();

		if (sem_post (&coord) == -1)
			print_exit("wrong post on cashier\n");

		if (sem_post (&receipt) == -1)
			print_exit("wrong post on cashier\n");
	}
}

int main (int argc, char *argv[])
{
	int thread_count;
	pthread_t sem_cashier[1];
	pthread_t sem_barber[3];
	pthread_t sem_customer[NUM_CUSTOMERS];
	int status;

// sem_t max_capacity = 20;
// sem_t sofa = 4;
// sem_t barber_chair = 3, coord = 3;
// sem_t mutex1 = 1, mutex2 = 1;
// sem_t cust_ready = 0, leave_b_chair = 0, payment = 0, receipt = 0;
// sem_t finished[50] = {0};

	/* initialize semaphore to 0 (3rd parameter) */
	if (sem_init (&max_capacity, 0, 20) == -1)
		print_exit("wrong init semaphore\n");
	if (sem_init (&sofa, 0, 4) == -1)
		print_exit("wrong init semaphore\n");
	if (sem_init (&barber_chair, 0, 3) == -1)
		print_exit("wrong init semaphore\n");
	if (sem_init (&coord, 0, 3) == -1)
		print_exit("wrong init semaphore\n");
	if (sem_init (&mutex1, 0, 1) == -1)
		print_exit("wrong init semaphore\n");
	if (sem_init (&mutex2, 0, 1) == -1)
		print_exit("wrong init semaphore\n");
	if (sem_init (&cust_ready, 0, 0) == -1)
		print_exit("wrong init semaphore\n");
	if (sem_init (&leave_b_chair, 0, 0) == -1)
		print_exit("wrong init semaphore\n");
	if (sem_init (&payment, 0, 0) == -1)
		print_exit("wrong init semaphore\n");
	if (sem_init (&receipt, 0, 0) == -1)
		print_exit("wrong init semaphore\n");
	for (thread_count = 0; thread_count < NUM_CUSTOMERS; thread_count++)
		if (sem_init (&finished[thread_count], 0, 0) == -1)
			print_exit("wrong init semaphore\n");


	/*
	* Create n threads to wait concurrently on the semaphore.
	*/
	for (thread_count = 0; thread_count < 1; thread_count++)
	{
		// int *pnum = (int*)malloc(sizeof(int));
		// *pnum = thread_count;
		status = pthread_create (
			&sem_cashier[thread_count], NULL,
			cashier, NULL);
		if (status != 0)
		{
			printf("Create thread\n");
			exit(1);
		}
	}

	for (thread_count = 0; thread_count < 3; thread_count++)
	{
		// int *pnum = (int*)malloc(sizeof(int));
		// *pnum = thread_count;
		status = pthread_create (
			&sem_barber[thread_count], NULL,
			barber, NULL);
		if (status != 0)
		{
			printf("Create thread\n");
			exit(1);
		}
	}

	for (thread_count = 0; thread_count < NUM_CUSTOMERS; thread_count++)
	{
		// int *pnum = (int*)malloc(sizeof(int));
		// *pnum = thread_count;
		status = pthread_create (
			&sem_customer[thread_count], NULL,
			customer, NULL);
		if (status != 0)
		{
			printf("Create thread\n");
			exit(1);
		}
	}


	/* pause before releasing threads */
	// sleep (2);

	/*
	* Release the waiting threads by posting to the semaphore 
	* once for each waiting thread. 
	*/

	// for (thread_count = 0; thread_count < NUM_CUSTOMERS; thread_count++)
	// {
	// 	printf("Posting from main\n");
	// 	if (sem_post (&semaphore) == -1)
	// 	{
	// 		printf("Post semaphore\n");
	// 		exit(1);
	// 	}
	// }

	/*
	* Wait for all threads to complete.
	*/


	for (thread_count = 0; thread_count < NUM_CUSTOMERS; thread_count++)
	{
		status = pthread_join (sem_customer[thread_count], NULL);
		if (status != 0)
		{
			printf("Join thread\n");
			exit(1);
		}
	}
	
	

	// for (thread_count = 0; thread_count < 1; thread_count++)
	// {
	// 	status = pthread_join (sem_cashier[thread_count], NULL);
	// 	if (status != 0)
	// 	{
	// 		printf("Join thread\n");
	// 		exit(1);
	// 	}
	// }

	// for (thread_count = 0; thread_count < 3; thread_count++)
	// {
	// 	status = pthread_join (sem_barber[thread_count], NULL);
	// 	if (status != 0)
	// 	{
	// 		printf("Join thread\n");
	// 		exit(1);
	// 	}
	// }

	return 0;
}

