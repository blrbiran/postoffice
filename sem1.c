/*
 * Demonstrate use of semaphores for synchronization.
 */
#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <semaphore.h>

/* barbershop 2 */
sem_t max_capacity = 20;
sem_t sofa = 4;
sem_t barber_chair = 3, coord = 3;
sem_t mutex1 = 1, mutex2 = 1;
sem_t cust_ready = 0, leave_b_chair = 0, payment = 0, receipt = 0;
sem_t finished[50] = {0};
int count;

/*
 * Thread function to wait on a semaphore.
 */
void *customer (void)
{

  int custnr;
  if (sem_wait (&semaphore) == -1)
  {
      printf("Wait on semaphore\n");
  exit(1);
  }
   printf("Thread %d waiting\n", num);

   
   printf("Thread %d resuming\n", num);
   return NULL;
}

int main (int argc, char *argv[])
{
   int thread_count;
   pthread_t sem_waiters[NUM_THREADS];
   int status;


   /* initialize semaphore to 0 (3rd parameter) */
   if (sem_init (&semaphore, 0, 0) == -1)
   {
      printf("Init semaphore\n");
      exit(1);
   }

   /*
    * Create n threads to wait concurrently on the semaphore.
    */
   for (thread_count = 0; thread_count < NUM_THREADS; thread_count++) {
      int *pnum = (int*)malloc(sizeof(int));
      *pnum = thread_count;
      status = pthread_create (
          &sem_waiters[thread_count], NULL,
          sem_demo, (void*)pnum);
      if (status != 0)
      {
         printf("Create thread\n");
	 exit(1);
      }
   }

   /* pause before releasing threads */
   sleep (2);

   /*
    * Release the waiting threads by posting to the semaphore 
    * once for each waiting thread. 
    */

   for (thread_count = 0; thread_count < NUM_THREADS; thread_count++) {

      printf("Posting from main\n");
      if (sem_post (&semaphore) == -1)
      {
         printf("Post semaphore\n");
	 exit(1);
      }
   }

   /*
    * Wait for all threads to complete.
    */
   for (thread_count = 0; thread_count < NUM_THREADS; thread_count++) {
      status = pthread_join (sem_waiters[thread_count], NULL);
      if (status != 0)
      {
         printf("Join thread\n");
	 exit(1);
      }
   }
   return 0;
}


