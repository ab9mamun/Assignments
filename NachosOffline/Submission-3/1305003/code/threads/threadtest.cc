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

#include <cmath>
#include <ctime>
#include <sstream>

using namespace std;


#define TOTAL 10
#define CONTAINER_SIZE 10
// testnum is set in main.cc
int testnum = 1;


Container* container;

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
		Producer* prod = new Producer(id, container);
	    int time = 0, item;
	    while(true) {
	    	///wait for an arbitrary time
	    	time = (20)*10000000;
	    	for(int i=0; i<time; i++){
	    		///---Waiting before producing another item----------
	    	}
	    	item = id;
	    	prod->produce(item);
	        currentThread->Yield();
	    }
}


void runConsumer(int id){
		Consumer* cons = new Consumer(id, container);
	    int time = 0, item;
	    while(true) {
	    	///wait for an arbitrary time
	    	time = (30)*1000000;
	    	for(int i=0; i<time; i++){
	    		///----Waiting

	    	}

	    	item = cons->consume();

	        currentThread->Yield();
	    }
}




void keepLooping(int x){

	while(true){
		currentThread->Yield();
	}

}











void
ThreadTest()
{
	//srand(time(NULL));

	container = new Container(CONTAINER_SIZE);
	container->print();
	for(int i=0; i<TOTAL; i++){
		stringstream sp, sc;
		sp<<"Producer "<<(i+1);

		Thread* t = new Thread((char*) sp.str().c_str());
		t->Fork(runProducer, i+1);



		sc<<"Consumer "<<(i+1);

		Thread* t2 = new Thread((char*) sc.str().c_str());
		t2->Fork(runConsumer, i+1);
	}

	keepLooping(0);

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

