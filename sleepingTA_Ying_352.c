#include <pthread.h>
#include <semaphore.h>
#include <stdio.h>

/* the maximum time (in seconds) to sleep */
#define MAX_SLEEP_TIME 3
/* number of potential students */
#define NUM_OF_STUDENTS 4
#define NUM_OF_HELPS 2
/* number of available seats */
#define NUM_OF_SEATS 2

/* Before using any mutex and semaphore, one must initialize it. 
One must destroy any mutex and semaphore before the process terminates */

/* mutex declarations, a global variable */
pthread_mutex_t mutex_lock; /* protect the global variable waiting_student */
/* semaphore declarations, global variables */
sem_t students_sem; /* TA waits for a student to show up, student notifies TA his/her arrival*/
sem_t ta_sem;/* student waits for TA to help, TA notifies student he/she is ready to help*/
/* the number of waiting students, a global variable */
int waiting_students;

/* To simplify the situation, when TA helps a student, only the TA thread invokes sleep() while the student thread does not invoke sleep().*/

/* For simplicity, each student thread terminates after getting help twice from the TA. 
The TA is not aware of the number of students, nor does the TA keep track of how many helps are yet to be offered to students. */

/* Note one does not need to have a waiting student queue. 
As long as there is available seat, the student can simply invoke wait on the proper semaphore. 
When the TA offers the help by invoking signal on the proper semaphore, the default Pthread implementation on Linux removes a waiting student thread in FIFO order.*/

void help(void)
{
	int seed = time(NULL);
	while(1)
	{
		sem_wait(&students_sem);
		
    	int time_of_help = (rand_r(&seed) % MAX_SLEEP_TIME) + 1;
		// critical section: TA is helping a student, only the TA thread invokes sleep() to simplify the situation
		// unlock mutex and reduce waiting_students
		pthread_mutex_lock(&mutex_lock); // lock
		waiting_students--;
		printf("Helping a student for %d seconds, # of waiting_students = %d\n", time_of_help, waiting_students);
		sem_post(&ta_sem); // call the student to come, OK to call, won't block mutex
		pthread_mutex_unlock(&mutex_lock); // unlock

		sleep(time_of_help); // then help the student
	}
	

}

int being_seated(int student_id)
{
	int result = 0;
	pthread_mutex_lock(&mutex_lock); // lock

	// level 2 semaphore for students to keep programming or wait for the TA on the seats
	if(waiting_students < NUM_OF_SEATS)
	{
		sem_post(&students_sem);
		waiting_students++;
		result = 1;
		printf("%d takes a seat, # of waiting students = %d\n", student_id, waiting_students);
		pthread_mutex_unlock(&mutex_lock); // unlock
		sem_wait(&ta_sem);
	}
	else
	{
		int seed = time(NULL);
    	int time_of_sleep = (rand_r(&seed) % MAX_SLEEP_TIME) + 1;
		
		printf("Student %d programming for %d seconds\n", student_id, time_of_sleep);
		pthread_mutex_unlock(&mutex_lock); // unlock
		sleep(time_of_sleep);
	}
	return result;
}

void to_be_seated(int* student_id)
{
	
	int num_of_help = 0;
	while(num_of_help < NUM_OF_HELPS)
	{
		if(being_seated(*student_id) == 1)
		{
			num_of_help++;
		}
	}
}

int main(void)
{
	printf("CS149 SleepingTA from Ying Wang\n");

	pthread_t TA;
	pthread_t students[NUM_OF_STUDENTS];

	pthread_mutex_init(&mutex_lock, NULL);
	// pthread_mutex_lock(&mutex_lock);
	// no other thread to fight
	waiting_students = 0;

	sem_init(&ta_sem, 0, 0); // 0, if more than 0 in this case, can't garantee the order
	sem_init(&students_sem, 0, 0); // 0

	for(int i = 0; i < NUM_OF_STUDENTS; i++)
	{
		pthread_create(&students[i], NULL, &to_be_seated, &i);

	}

	pthread_create(&TA, NULL, &help , NULL); 

	/* After all student threads terminate, the program cancels the TA thread by calling pthread_cancel() and then the entire program terminates. */

	for(int i = 0; i < NUM_OF_STUDENTS; i++)
	{
		pthread_join(students[i], NULL);
	}

	pthread_cancel(TA);

	// mutex.destroy + sem.destroy
	sem_destroy(&ta_sem);
	sem_destroy(&students_sem);
	pthread_mutex_destroy(&mutex_lock);
	
	return 0;
}