/**
Assumptions:
1. If a student meets B, but B doesn't find his/her id in the duplicateFilter, then B tells him/her to come later. The student will sleep for a certain time and meet B again.
2. If B gets multiple applications from same student id in the duplicateFilter when the corresponding student is meeting B, only then all his/her applications will be discarded/ignored.
3. Student will meet D in a Queue of a defined size..



*/
#include<stdio.h>
#include <stdlib.h>
#include <string.h>
#include<pthread.h>
#include<semaphore.h>
#include <windows.h>
#include<stdlib.h>
#include <time.h>
#include<unistd.h>
#include <math.h>

///-----------------------let's define some constants-------------------------------------
#define PASSWORD_SIZE 9                                 ///this means the passwords will be of 8 characters. cause we need a place for '\0'

#define APPQ_SIZE 10
#define BQ_SIZE 1
#define DQ_SIZE 1
#define DUP_FILTER_SIZE 200
#define DUP_STUDENTS_SIZE 200

#define STU_COUNT 30
#define STU_PROCESS_COUNT 45


///---------------------let's define a class (struct :-/ ) for students------------------------------------
typedef struct {
    int id;
    int meetAgainAfter;   ///B or D will set this time if student's application is not found---------------------------
    char password[PASSWORD_SIZE];

    sem_t full;
    sem_t empty;
    pthread_mutex_t lock;

} Student;


///-----------------------let's now define the containers------------------------------------
Student* BQ[BQ_SIZE];
int appQ[APPQ_SIZE];
int dupFilter[DUP_FILTER_SIZE];
int dupStudents[DUP_STUDENTS_SIZE];
Student* DQ[DQ_SIZE];


int frontBQ =0;
int rearBQ =0;

int frontAppQ =0;
int rearAppQ =0;

int lengthDupFilter =0;
int lengthDupStudents = 0;

int frontDQ = 0;
int rearDQ =0;

///----------------let's declare some semaphores for the containers----------------------
sem_t emptyAppQ;
sem_t fullAppQ;
sem_t emptyBQ;
sem_t fullBQ;
sem_t emptyDQ;
sem_t fullDQ;

pthread_mutex_t lockAppQ;
pthread_mutex_t lockBQ;
pthread_mutex_t lockDupFilter;
pthread_mutex_t lockPasswords;
pthread_mutex_t lockDQ;

///---------------let's define an initiator for them---------------------------

void init_semaphore()
{
	sem_init(&emptyAppQ,0,APPQ_SIZE);
	sem_init(&fullAppQ,0,0);

	sem_init(&emptyBQ,0,BQ_SIZE);
	sem_init(&fullBQ, 0, 0);

	sem_init(&emptyDQ, 0, DQ_SIZE);
	sem_init(&fullDQ, 0, 0);

	pthread_mutex_init(&lockAppQ,0);
	pthread_mutex_init(&lockBQ, 0);
	pthread_mutex_init(&lockDupFilter, 0);
	pthread_mutex_init(&lockPasswords, 0);
}

///----------define a library for windows users--------------------------------------
void sleep(unsigned int seconds)
{
    int mseconds = seconds*1000;
    clock_t goal = mseconds + clock();
    while (goal > clock());
}

///-------------------let's define some methods for containers--------------------------------------------------------------------------------------------------------------------------------------

///----appQ methods------
void enqueueAppQ(int id){
        sem_wait(&emptyAppQ);  ///wait for a place to be free in the queue--increase #empty
        pthread_mutex_lock(&lockAppQ);  ///aquire lock before writing on the queue

        int newRear = (rearAppQ+1)%APPQ_SIZE;
        if(newRear==frontAppQ) {printf("bug found at enqueueAppQ"); return;}     ///in any case if length>size

        appQ[newRear] = id; ///insertion done

		pthread_mutex_unlock(&lockAppQ);  ///now unlock the queue
		sem_post(&fullAppQ);  ///decrease #full
}

int dequeueAppQ(){
        sem_wait(&fullAppQ);  ///wait for a place to be free in the queue--increase #empty
		pthread_mutex_lock(&lockAppQ);  ///aquire lock before writing on the queue

		if(rearAppQ==frontAppQ) {printf("bug found at dequeueAppQ"); return;}     ///in any case if length<0


       int id = appQ[frontAppQ]; ///dequeue successful
        frontAppQ = (frontAppQ+1)%APPQ_SIZE;

		pthread_mutex_unlock(&lockAppQ);  ///now unlock the queue
		sem_post(&emptyAppQ);  ///decrease #full

		return id;

}

///BQ methods -----------------
void enqueueBQ(Student* stu){
        sem_wait(&emptyBQ);  ///wait for a place to be free in the queue--increase #empty
        pthread_mutex_lock(&lockBQ);  ///aquire lock before writing on the queue

        int newRear = (rearBQ+1)%BQ_SIZE;
        if(newRear==frontBQ) {printf("bug found at enqueueBQ"); return;}     ///in any case if length>size

        BQ[newRear] = stu; ///insertion done

		pthread_mutex_unlock(&lockBQ);  ///now unlock the queue
		sem_post(&fullBQ);  ///decrease #full
}

int dequeueBQ(){
        sem_wait(&fullBQ);  ///wait for a place to be free in the queue--increase #empty
		pthread_mutex_lock(&lockBQ);  ///aquire lock before writing on the queue

		if(rearBQ==frontBQ) {printf("bug found at dequeueBQ"); return;}     ///in any case if length<0


       Student* stu = BQ[frontBQ]; ///dequeue successful
        frontBQ = (frontBQ+1)%BQ_SIZE;

		pthread_mutex_unlock(&lockBQ);  ///now unlock the queue
		sem_post(&emptyBQ);  ///decrease #full

		return stu;

}

///methods for DQ ---------
void enqueueDQ(Student* stu){
        sem_wait(&emptyDQ);  ///wait for a place to be free in the queue--increase #empty
        pthread_mutex_lock(&lockDQ);  ///aquire lock before writing on the queue

        int newRear = (rearDQ+1)%DQ_SIZE;
        if(newRear==frontDQ) {printf("bug found at enqueueDQ"); return;}     ///in any case if length>size

        DQ[newRear] = stu; ///insertion done

		pthread_mutex_unlock(&lockDQ);  ///now unlock the queue
		sem_post(&fullDQ);  ///decrease #full
}

int dequeueDQ(){
        sem_wait(&fullDQ);  ///wait for a place to be free in the queue--increase #empty
		pthread_mutex_lock(&lockDQ);  ///aquire lock before writing on the queue

		if(rearDQ==frontDQ) {printf("bug found at dequeueDQ"); return;}     ///in any case if length<0


       Student* stu = DQ[frontDQ]; ///dequeue successful
        frontBQ = (frontDQ+1)%DQ_SIZE;

		pthread_mutex_unlock(&lockDQ);  ///now unlock the queue
		sem_post(&emptyDQ);  ///decrease #full

		return stu;

}
///----duplicate filter-------





void * run_ACE(void * arg) {
    char name = *(char* ) arg;

    printf("I am %c\n", name);
}

void * run_B(void * arg) {
     char name = *(char* ) arg;

    printf("I am %c\n", name);
}

void * run_D(void * arg) {
 char name = *(char* ) arg;

    printf("I am %c\n", name);
}
void * run_Student(void *arg){
 int id = *(int* ) arg;

    printf("I am %d\n", id);
}


int main(void)
{
    ///-------------------declare the teacher threads---------------------------
	pthread_t A;
	pthread_t B;
	pthread_t C;
	pthread_t D;
	pthread_t E;
    int i, j, k, l, m, n;


	init_semaphore();
	srand(time(NULL));

    char* teacherNames = "ACEBD";


///------------------------create the teacher threads---------------------------------
	pthread_create(&A,NULL,run_ACE,(void*) &teacherNames[0] );
	pthread_create(&C, NULL, run_ACE, (void*) &teacherNames[1]);
    pthread_create(&E, NULL, run_ACE, (void*) &teacherNames[2]);

	pthread_create(&B,NULL,run_B,(void*) &teacherNames[3] );
	pthread_create(&D,NULL,run_D,(void*) &teacherNames[4] );

	///-----------let's now create the student threads-----------------------------
	for(i=1; i<=STU_PROCESS_COUNT; i++) {
            int id = rand()%STU_COUNT+1;
            pthread_t t;
            pthread_create(&t,NULL,run_Student,(void*) &id );

	}


	while(1){
        sleep(3);
        //printf("i'm here");
	};
	return 0;
}
