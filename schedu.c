#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <sys/queue.h>

//Compile command: gcc schedu.c -o schedu -Wall -lpthread
/*
	Code for Custom Queue referenced from
	https://www.geeksforgeeks.org/queue-linked-list-implementation/
	
*/
//Job and Ready Queue as global variables
struct queue* job;
struct queue* ready;

//Struct to represent the process containing the process' pid, time and a pointer to the next process
struct process{
        int pid;
        int time;
        struct process* next;
};

//struct to represent the queue (job and ready)
//struct contains reference to the first element of the queue, the last element, the number of elements(processes) currently in the queue and the maximum capacity the queue can hold
struct queue{
	struct process* first;
        struct process* last;
	int numProc;
	int capacity;
};

//Creates a new process given the process id, and the process time
//sets the process given the process id, process time and setting the next pointer to null
struct process* newProcess(int p, int t){
        struct process* temp = (struct process*)malloc(sizeof(struct process));
        temp->pid = p;
	temp->time = t;
        temp->next=NULL;
        return temp;
}

//Creates a new queue given a maximum capacity setting the number of elements to 0
struct queue* makeQueue(int c){
       	struct queue* q = (struct queue*)malloc(sizeof(struct queue));
        q->first = q->last = NULL;
	q->numProc = 0; 
	q->capacity = c;
        return q;
}

//Enqueues a process to the queue given said queue, process pid, and process time.
//Increases the number of processes and sets the next accordingly
void enqueue(struct queue* processes, int p, int t){
        struct process* temp = newProcess(p, t);
	//take value of the number of processes from the queue and save it ina  variable
	int cap = processes->numProc;
	//increment the number
	cap++;
	//update the number of processes
	processes->numProc = cap;
	//if the queue is empty
        if(processes->last==NULL){
		//the only element will have the last and first pointed to it
                processes->last=processes->first=temp;
        }
        else{
		
                processes->last->next = temp;
                processes->last=temp;

        }
}

//Dequeues a process given a queue
//sets the first element to the next one to remove the element from the queue
// decrements the number of processes by 1
struct process* dequeue(struct queue* q){
	//if empty then there is nothing to dequeue
        if(q->first == NULL){
                return NULL;
        }
	// get the number of processes and decrement that number
	int n = q->numProc;
	n--;
	//update the number of processes for the designated queue
	q->numProc = n;
	
	//keep a reference of the process to dequeue
	struct process* temp = q->first;
	//remove the reference to the first process by saving the first element as the next element of first
        q->first = q->first->next;
	//if the first element is null then the last element must be null	
        if(q->first == NULL){
                q->last = NULL;
        }
	//return the dequeue element
	return temp;
}

//Function to print the job queue by iterating across a temp queue and resetting the first element of the job queue back to the original first element
void printJob(){
	if(job->last == NULL){
		printf("EMPTY\n");
	}
	else{
		struct queue* temp = makeQueue(100);
		temp = job;
		struct process* first = job->first;

		while(temp->first->pid!=temp->last->pid){
			printf("[Process %d: Time %d], ",temp->first->pid ,temp->first->time);
			temp->first = temp->first->next;
		}
		printf("[Process %d: Time %d]\n",temp->first->pid ,temp->first->time);
		job->first = first;
	}
}

//Function to print the ready queue by iterating across a temp queue and resetting the first element of the job queue back to the original first element
void printReady(){
	
	struct queue* temp = makeQueue(100);
	struct process* first = ready->first;
	if(ready->first==NULL){
		printf("EMPTY\n");
	}
	else{
	temp = ready;
	first = ready->first;
	while(temp->first->pid!=temp->last->pid){
		printf("[Process %d: Time %d], ",temp->first->pid ,temp->first->time);
		temp->first = temp->first->next;
	}
	printf("[Process %d: Time %d]\n",temp->first->pid ,temp->first->time);

	ready->first = first;

	}

}

//The long term scheduler thread function
void *lts(void *ptr){
	int i;
	struct process* tempProc;
	//print the job queue
	printf("[LTS] Job Queue: ");
	printJob();
	//print the ready queue
	printf("[LTS] Ready Queue: ");
	printReady();

//Transfer from job to ready
	for(i=0; i<=5;i++){
		//if the job queue is empty break out of the loop
		if(job->last == NULL){
			break;
		}
		//if the ready queue has reached capacity then break out of the loop
		if(ready->numProc == ready->capacity){
			printf("[LTS] Ready Queue is Full cannot enter anymore\n");
			break;
		}
		//dequeue from the job queue
		tempProc = dequeue(job);
		printf("[LTS] Process %d removed from the Job Queue and inserted into the Ready Queue \n", tempProc->pid);
		//enqueue the dequeued process to the ready queue
		enqueue(ready, tempProc->pid, tempProc->time);
	}	
	//print the job queue
	printf("[LTS] Job Queue: ");
	printJob();
	//print the ready queue
	printf("[LTS] Ready Queue: ");
	printReady();
	//exit the thread returning null
	pthread_exit(NULL);
	
}
//The short term scheduler thread function
void *sts(void *ptr){
	struct process* tempProc;
	int i;
	//print the job queue
	printf("[STS] Job Queue: ");
	printJob();

//print the ready queue
	printf("[STS] Ready Queue: ");
	printReady();

//execute the processes in Ready Queue
	int tempTime;	
	//loop to the number of processes in the ready queue
	for(i=0; i<ready->numProc; i++){
		//dequeue from ready
		tempProc = dequeue(ready);
		printf("[STS] Process %d is now executing.\n", tempProc->pid);

		
		//If the time is less than or equal to two then the process will terminate
		if(tempProc->time <= 2){
			printf("[STS] Process %d terminated.\n", tempProc->pid);
		}
		else{
			//subtract two from the time and update the process' time
			tempTime = tempProc->time;
			tempTime = tempTime-2;
			tempProc->time = tempTime;
			//enqueue the process back into the ready queue
			enqueue(ready, tempProc->pid, tempProc->time);
			printf("[STS] Process %d with remaining Time %d enqueued to the Ready Queue.\n", tempProc->pid, tempProc->time);
		}
	}

//Display Job Queue again
	printf("[STS] Job Queue: ");
	printJob();

//Display Ready Queue again
	printf("[STS] Ready Queue: ");
	printReady();
//Exit the thread returning null
	pthread_exit(NULL);
}


int main(){
	//create the job queue with a capacity of 100
	job = makeQueue(100);
	//create the ready queue with the capacity of 5
	ready = makeQueue(5);
	int i;
	
	
	srand(time(0));
	int random;
	for(i=1;i<=100;i++){
		//create a random integer ranging from 1 to 30
		random = rand()%30+1;
		//enqueue to the job queue processes with incrementing pid and a random time
		//ranging from 1 to 30
		enqueue(job, i, random);
		printf("[Kernel] Process %d created with Time = %d\n", job->last->pid, job->last->time);
	}
	
	pthread_t lThread, sThread;
	//notify that the long term scheduler thread has been invoked and call to the LTS thread function
	printf("[Kernel] Long Term Scheduler Invoked.\n");
	pthread_create(&lThread, NULL, lts, NULL);
	//wait for the LTS thread to finish
	pthread_join(lThread, NULL);
	//notify that the short term scheduler thread has been invoke and call to the STS thread function
	printf("[Kernel] Short Term Scheduler Invoked.\n");
	pthread_create(&sThread, NULL, sts, NULL);
	//wait for the STS thread to finish
	pthread_join(sThread, NULL);
	
	//loop until both the job queue and ready queue are empty
	while(job->last!= NULL || ready->last!=NULL){
		//notify that the long term scheduler thread has been invoked and call to the 			LTS thread function
		printf("[Kernel] Long Term Scheduler Invoked.\n");
		pthread_create(&lThread, NULL, lts, NULL);
		//wait for the LTS thread to finish
		pthread_join(lThread, NULL);
		//notify that the short term scheduler thread has been invoke and call to the 			STS thread function
		printf("[Kernel] Short Term Scheduler Invoked.\n");
		pthread_create(&sThread, NULL, sts, NULL);
		//wait for the STS thread to finish
		pthread_join(sThread, NULL);

	}

	return 0;
}



