#include "Container.h"
#include "copyright.h"
#include "system.h"

Container::Container(){
	lock = new Lock("Container lock");
	prodCondition = new Condition("Producer condition");
	consCondition = new Condition("Consumer condition");

	size = DEFAULT_SIZE;
	length = 0;
	front = 0;
	rear = 0;
	items = new int[size];

	printf("Container created. Size: %d\n", size);
}

Container::Container(int size){
	lock = new Lock("Container lock");
	prodCondition = new Condition("Producer condition");
	consCondition = new Condition("Consumer condition");

	this->size = size;
	length = 0;
	front = 0;
	rear = 0;
	items = new int[size];
	printf("Container created. Size: %d\n", size);
}

void Container::put(int item){

	lock->Acquire();
	while(length==size){
	//	printf("Producer %d is going to sleep\n", prodId);
		prodCondition->Wait(lock);
		//printf("Producer %d woke up\n", prodId);
	}

	///now produce----------------
	for(int i=0; i<(15)*10000000; i++){
			///----------PRODUCING------------------------------
	}

	items[rear] = item;
	rear = (rear+1)%size;
	length++;

///	printf("Consumer %d consumed Food %d\n", prodId, item); ///what the hell was that????

	printf("\n%s produced Food %d\n", currentThread->getName(), item);
	print();

	if(length==1) consCondition->Broadcast(lock);

	lock->Release();
}

int Container::get(){

	lock->Acquire();

	while(length==0){
		//printf("Consumer %d is going to sleep\n", consId);
		consCondition->Wait(lock);
		//printf("Consumer %d woke up\n", consId);
	}

	///now consume item---------
	for(int i=0; i<(7)*10000000; i++){
		///--------------------CONSUMING-------------------------------------
	}

	int item = items[front];
	front = (front+1)%size;
	length--;

	printf("\n%s consumed Food %d\n", currentThread->getName(), item);
	print();

	if(length==size-1) prodCondition->Broadcast(lock);

	lock->Release();

	return item;

}



void Container::print(){
	printf("Container: ");
	for(int i=0; i<length; i++){
		printf("%d ", items[(front+i)%size]);
	}
	if(length==size) printf("<<FULL>>");
	else if(length==0) printf("<<EMPTY>>");
	printf("\n\n");
}







Container::~Container(){
	delete[] items;
	delete lock;
	delete consCondition;
	delete prodCondition;
}
