//sem_destroy(&sem_Mcounter); //to destroy the semaphore

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <time.h>
#include <semaphore.h>
#define BUFFER_SIZE 4

sem_t sem_Mcounter;    ///semaphore of Mcounter
sem_t sem_buffer;     ///semaphore of buffer
sem_t sem_full;
sem_t sem_empty;

int counter=0;
int buffer[BUFFER_SIZE];
int index=0;         ///index of the buffer
int index2=0;        ///index in collector


int lower,upper;     ///these 3 lines for random number generation
lower=1;
upper=7;
int randomNumber;


void *Mcounter(void *i)
{


    int a= *((int *)i);  ///a is the number of the thread, here casting the *i to int




    while(1)  ///infinite loop of Mcounter
    {
        randomNumber = (rand() % (upper - lower + 1)) + lower;   ///random generation for numbers
       // printf("\nrand num is %d\n",randomNumber);
        sleep(randomNumber);

        printf("Counter thread %d:received message\n",a);
        printf("Counter thread %d:waiting to write \n",a);

        sem_wait(&sem_Mcounter);  ///wait (semaphore of Mcounter thread)

        counter++;

        printf("Counter thread %d:now adding to mcounter,counter value is %d\n",a,counter);

        sem_post(&sem_Mcounter);  ///signal (semaphore of Mcounter)


    }



}



void *monitor(void *parameter1)
{


    int temp;                                ///to store value of counter in local variable


    while(1)  ///infinite loop of monitor
    {
        randomNumber = (rand() % (upper - lower + 1)) + lower; ///random generation for numbers

       // printf("\nrand num is %d\n",randomNumber);

        sleep(randomNumber);

        printf("\nMonitor thread:waiting to read count\n");

        sem_wait(&sem_Mcounter);  ///sem wait of counter

        printf("\nMonitor thread:reading a count value of %d\n",counter);

        temp=counter;

        sem_post(&sem_Mcounter);  ///signal of counter

///check here if empty semaphore = 0 if 0 full buffer
        int sem_value;

        int sem_getvalue(sem_t *sem_empty, int *sem_value);

        if(sem_value==0)
            printf("\nMonitor thread: buffer is full !\n");

        /*PRODUCER*/
        sem_wait(&sem_empty);

        sem_wait(&sem_buffer);   ///sem wait of buffer

        index++;

        index=index% BUFFER_SIZE;

        buffer[index]= temp;  ///append in buffer

    //    printf("\n(not required)buffer[%d]=%d\n",index,counter);

        counter=0;   ///reset the counter

        printf("\nMonitor thread writing to buffer at position:%d\n",index);

        sem_post(&sem_buffer); ///sem signal buffer
        sem_post(&sem_full);







    }



}

void *collector(void *parameter2)
{

   // int valueRead;



    while(1)  ///infinite loop
    {
        randomNumber = (rand() % (upper - lower + 1)) + lower; ///random generation for numbers
       // printf("\nrand num is %d\n",randomNumber);

        sleep(randomNumber);

        ///check if sem_full is 0 buffer is empty

        int sem_value;

        int sem_getvalue(sem_t *sem_full, int *sem_value);
        if(sem_value==0)
            printf("\nCollector thread: buffer is empty\n");


        /*consumer*/

        sem_wait(&sem_full);

        sem_wait(&sem_buffer);

        index2++;
        index2=index2%BUFFER_SIZE;

       // valueRead =buffer[index2];
        printf("\nCollector thread:readding from buffer at position %d\n ", index2);


        sem_post(&sem_buffer);

        sem_post(&sem_empty);

      //  printf("\n(not required) value from buffer is: %d\n",valueRead); //







    }


}








int main()
{
    int numberOfMessages;  ///number of messages=number of threads

    printf("Enter number of messages:");
    scanf("%d",&numberOfMessages);

    sem_init(&sem_Mcounter, 0, 1);
    ///0 to indicate it's shared bet threads not processes //1 is the initial value of semaphore

    sem_init(&sem_buffer, 0, 1);               ///initial value of buffer's semaphore is 1
    sem_init(&sem_full, 0, 0);                ///initial value of full semaphore is 0
    sem_init(&sem_empty, 0, BUFFER_SIZE);    ///initial value of empty semaphore is buffer size



    pthread_t mCounterThreads[numberOfMessages];  ///for Mcounter threads
    pthread_t mMonitor;                   ///for monitor thread
    pthread_t mmCollector;                  ///for collector thread


    int i;
    for(i=1; i<=numberOfMessages; i++) ///to create N counter threads
    {
        int *arg= malloc (sizeof(*arg));
        *arg=i;

        pthread_create(&mCounterThreads[i],NULL,&Mcounter,arg);

    }


    pthread_create(&mMonitor,NULL,&monitor,NULL);    ///thread of monitor


    pthread_create(&mmCollector,NULL,&collector,NULL);  ///thread of collector



    for(i=1; i<=numberOfMessages; i++)      ///to join N counter threads
    {

        pthread_join(mCounterThreads[i],NULL);
    }


    pthread_join(mMonitor,NULL);   ///join thread of monitor

    pthread_join(mmCollector,NULL);   ///join thread of collector


    return 0;
}
