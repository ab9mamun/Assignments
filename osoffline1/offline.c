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
#define PASSWORD_LIST_SIZE 200

#define STU_COUNT 30
#define STU_PROCESS_COUNT 45

#define NEXT_MEETING_TIME 5
#define APPROVED_BY_B 1
#define NOT_FOUND_BY_B 0
#define DUPLICATE_FOUND_BY_B 2


///---------------------let's define a class (struct :-/ ) for students------------------------------------
typedef struct {
    int id;
    int meetAgainAfter;   ///B or D will set this time if student's application is not found---------------------------
    char password[PASSWORD_SIZE];

    sem_t full;
    sem_t empty;
    pthread_mutex_t lock;

} Student;

///----------------------we need a class (struct :-/ ) for passwords too ---------------------
typedef struct {
    int stdId;
    char password[PASSWORD_SIZE];
} Password;


///------------formalities aka prototypes--------------------
void generateAndAddPassword(int);


///-----------------------let's now define the containers------------------------------------
Student* BQ[BQ_SIZE];
int appQ[APPQ_SIZE];
int dupFilter[DUP_FILTER_SIZE];
int dupStudents[DUP_STUDENTS_SIZE];
Student* DQ[DQ_SIZE];
Password passwords[PASSWORD_LIST_SIZE];


int frontBQ =0;
int rearBQ =0;

int frontAppQ =0;
int rearAppQ =0;

int lengthDupFilter =0;
int lengthDupStudents = 0;

int frontDQ = 0;
int rearDQ =0;

int lengthPasswords;

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

///--------------students communication with B & D-------------------
void setMeetAgain(Student* stu, int nextTime){
    sem_wait(&stu->empty);
    pthread_mutex_lock(&stu->lock);

    stu->meetAgainAfter = nextTime;

    pthread_mutex_unlock(&stu->lock);
    sem_post(&stu->full);
}

int getMeetAgain(Student* stu){
    sem_wait(&stu->full);
    pthread_mutex_lock(&stu->lock);

    int x = stu->meetAgainAfter;

    pthread_mutex_unlock(&stu->lock);
    sem_post(&stu->empty);

    return x;
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
///----duplicate students---------
int isAlreadyDuplicate(int id){
    int i;
    for(i=0; i<lengthDupStudents; i++){
        if(dupStudents[i]==id) return 1;
    }
    return 0;
}
void addDuplicate(int id){
    if(lengthDupStudents==DUP_STUDENTS_SIZE) {printf("Bug in addDuplicate"); return;}
    dupStudents[lengthDupStudents++] = id;
}

///----duplicate filter ---------
void addToDupFilter(int id){

    pthread_mutex_lock(&lockDupFilter);

    if(lengthDupFilter==DUP_FILTER_SIZE) {printf("Bug in addToDupFilter");return;}
    dupStudents[lengthDupFilter++] = id;

    pthread_mutex_unlock(&lockDupFilter);
}

int checkDupFilter(Student* stu){   ///returns 1 if approved. 0 otherwise

    int id = stu->id;

    if(isAlreadyDuplicate(id)) {
            setMeetAgain(stu, 0);
            return NOT_FOUND_BY_B;
    }

    pthread_mutex_lock(&lockDupFilter);

    int i, index, j, k, val;
    index = -1;
    val = APPROVED_BY_B;
    for(i=0; i<lengthDupFilter; i++){
        if(dupFilter[i]==id){
            index = i;   ///found
            setMeetAgain(stu, 0);                  ///no need to keep the student locked for so long-----------------
            break;
        }
    }
    if(index<0) {
        pthread_mutex_unlock(&lockDupFilter);
        setMeetAgain(stu, NEXT_MEETING_TIME);
        return ;  ///as not found, next meeting is needed...
    }
    for(i=index+1; i<lengthDupFilter; i++){
        if(dupFilter[i]==id){   ///duplicate entry found-----------------------------------
                val = DUPLICATE_FOUND_BY_B;           ///it will be used to represent duplicate

                for(j=lengthDupFilter-1; j>=i; j--){              ///at first remove the occurrences after the first one
                    if(dupFilter[j]==id)
                        dupFilter[j] = dupFilter[--lengthDupFilter];
                }
                addToDupFilter(id);
                break;
        }
    }
    dupFilter[index] = dupFilter[--lengthDupFilter];  ///remove the first occurrence irrespective of it is a duplicate or not

     pthread_mutex_unlock(&lockDupFilter);
     return val;  ///if not duplicate, then approve

}

/// password------------------
void addPassword(int id, char pass[]){
    pthread_mutex_lock(&lockPasswords);

    if(lengthPasswords==PASSWORD_LIST_SIZE) {printf("Bug in addPassword"); return;}   ///bug found. so, why should i release the lock??

    passwords[lengthPasswords].stdId = id;
    strcpy(passwords[lengthPasswords].password, pass);
    lengthPasswords++;

    pthread_mutex_unlock(&lockPasswords);
}

int getPassword(int id, char whereToSave[]){
    pthread_mutex_lock(&lockPasswords);
    int i,  found = 0;
    for(i=0; i<lengthPasswords; i++){
            if(passwords[i].stdId==id){

                strcpy(whereToSave, passwords[i].password);
            }
            found =1;
            break;
    }
    pthread_mutex_unlock(&lockPasswords);
    return found;
}


///------------------------------------------------------------utility methods-----------------------------------------
void generateAndAddPassword(int id){
    /**
        0-9: 48-57  (total 10)
        A-Z: 65-90   (total 26)
        a-z: 97-122  (total 26)

        Total: 26+26+10 = 62 valid characters...
    */
    char buffer[PASSWORD_SIZE];
    int i;
    char c;

    for(i=0; i<PASSWORD_SIZE-1; i++){
        c = rand()%62;
        if(c<10) ///it's a number
            c+= '0';
        else if(c<36)
            c+= 'A';
        else
            c+= 'a';

        buffer[i] = c;
    }
    buffer[PASSWORD_SIZE-1] = 0;

    addPassword(id, buffer);

}

///-----------student initiator--------------------------
void init_student(Student* stu){
    stu->meetAgainAfter = 0;

    sem_init(&stu->empty, 0, 1);
    sem_init(&stu->full, 0, 0);
    pthread_mutex_init(&stu->lock,0);
}



///----------------------------------run methods of the threads----------------------------------------------

void * run_ACE(void * arg) {
    char name = *(char* ) arg;
    int id;
    while(1){
            id = dequeueAppQ();
            addToDupFilter(id);
            printf("Teacher %c has approved the application of Student %d\n\n", name, id);
    }

}

void * run_B(void * arg) {
     char name = *(char* ) arg;
     Student * stu;
     int id;
     int approved;
     while(1){

        sleep(3);

        stu = dequeueBQ();
        id = stu->id;
        approved = checkDupFilter(stu);

        if(approved==APPROVED_BY_B) {
                printf("-----------------------------------------------------------------------------------------\n\
                       <APPROVED> Teacher %c has approved the application of Student %d\n\
                       -----------------------------------------------------------------------------------------\n", name, id);
                generateAndAddPassword(id);
        }
        else if(approved==DUPLICATE_FOUND_BY_B){
                 printf("-----------------------------------------------------------------------------------------\n\
                       <REJECTED> Teacher %c has found duplicate applications from Student %d\n\
                       -----------------------------------------------------------------------------------------\n", name, id);
        }
        else {
                printf("Teacher %c has not found the application of Student %d\n\n", name, id);
        }
     }
}

void * run_D(void * arg) {
    char name = *(char* ) arg;
    Student * stu;
    int id;
    int found;
    char pass[PASSWORD_SIZE];
    while(1){
        stu= dequeueDQ();
        id = stu->id;
        found =  getPassword(id,pass);
        if(found) {
                strcpy(stu->password, pass);
                setMeetAgain(stu, 0);
        }
        else setMeetAgain(stu, NEXT_MEETING_TIME);
    }
}

void * run_Student(void *arg){
    int id = *(int* ) arg;
    Student* stu = (Student*) malloc(sizeof(Student));
    stu->id = id;
    init_student(stu);
    int nextMeet;
    enqueueAppQ(id);
    while(1){
        enqueueBQ(stu);
        nextMeet = getMeetAgain(stu);
        if(nextMeet==0) break;
        sleep(nextMeet);
    }
    while(1){
        enqueueDQ(stu);
        printf("Student %d has asked for password to Teacher D", id);
        nextMeet = getMeetAgain(stu);
        if(nextMeet==0) break;


        sleep(nextMeet);
    }
    printf("-----------------------------------------------------------------------------------------\n\
                       <SUCCESS> Student %d has got password: %s\n\
                       -----------------------------------------------------------------------------------------\n", id, stu->password);

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
