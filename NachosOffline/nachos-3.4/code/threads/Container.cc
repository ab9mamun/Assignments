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
}

void Container::put(int item){

	lock->Acquire();
	while(length==size){
		prodCondition->Wait(lock);
	}
	items[rear] = item;
	rear = (rear+1)%size;
	length++;

	if(length==1) consCondition->Signal(lock);

	lock->Release();
}

int Container::get(){

	lock->Acquire();

	while(length==0){
		consCondition->Wait(lock);
	}

	int item = items[front];
	front = (front+1)%size;
	length--;

	if(length==size-1) prodCondition->Signal(lock);

	lock->Release();

	return item;

}











Container::~Container(){
	delete[] items;
	delete lock;
	delete consCondition;
	delete prodCondition;
}
