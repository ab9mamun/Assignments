// threadtest.cc 
//	Simple test case for the threads assignment.
//
//	Create two threads, and have them context switch
//	back and forth between themselves by calling Thread::Yield, 
//	to illustratethe inner workings of the thread system.
//
// Copyright (c) 1992-1993 The Regents of the University of California.
// All rights reserved.  See copyright.h for copyright notice and limitation 
// of liability and disclaimer of warranty provisions.

#include "copyright.h"
#include "system.h"

#include "Producer.h"
#include "Consumer.h"
#include "synch.h"

#define TOTAL 10
// testnum is set in main.cc
int testnum = 1;

//----------------------------------------------------------------------
// SimpleThread
// 	Loop 5 times, yielding the CPU to another ready thread 
//	each iteration.
//
//	"which" is simply a number identifying the thread, for debugging
//	purposes.
//----------------------------------------------------------------------

void
SimpleThread(int which)
{
    int num;
    
    for (num = 0; num < 5; num++) {
	printf("*** thread %d looped %d times\n", which, num);
        currentThread->Yield();
    }
}

//----------------------------------------------------------------------
// ThreadTest1
// 	Set up a ping-pong between two threads, by forking a thread 
//	to call SimpleThread, and then calling SimpleThread ourselves.
//----------------------------------------------------------------------

void
ThreadTest1()
{
    DEBUG('t', "Entering ThreadTest1");

    Thread *t = new Thread("forked thread");

    t->Fork(SimpleThread, 1);
    SimpleThread(0);
}

//----------------------------------------------------------------------
// ThreadTest
// 	Invoke a test routine.
//----------------------------------------------------------------------








void runProducer(int id){
		Producer* prod = new Producer(id);
	    int time, item;
	    while(true) {
	    	///wait for an arbitrary time
	    	time = (1+5)*100000000;
	    	for(int i=0; i<time; i++){

	    	};
	    	item = 20+1;
	    	prod->produce(item);
	    	printf("Producer %d produced %d\n", id, item);
	        currentThread->Yield();
	    }
}


void runConsumer(int id){
		Consumer* cons = new Consumer(id);
	    int time, item;
	    while(true) {
	    	///wait for an arbitrary time
	    	time = (1+5)*100000000;
	    	for(int i=0; i<time; i++){

	    	};

	    	item = cons->consume();
	    	item = 20+1;

	    	printf("Consumer %d consumed %d\n", id, item);
	        currentThread->Yield();
	    }
}
















void
ThreadTest()
{

	for(int i=0; i<TOTAL; i++){
		Thread* t = new Thread("Producer thread");
		t->Fork(runProducer, i+1);

		Thread* t2 = new Thread("Consumer thread");
		t2->Fork(runConsumer, TOTAL-i);
	}

	runConsumer(TOTAL+1);

   // switch (testnum) {
   // case 1:
	//ThreadTest1();
	//break;
   // default:
	//printf("No test specified.\n");
	////break;
  //  }
	//(new Condition())->Sleep();

}

