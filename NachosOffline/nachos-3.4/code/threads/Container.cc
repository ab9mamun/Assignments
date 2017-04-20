#include "Container.h"
#include "copyright.h"
#include "system.h"

Container::Container(){
	lock = new Lock("Container lock");
	prodCondition = new Condition("Producer condition");
	consCondition = new Condition("Consumer condition");

	length = 0;
	front = 0;
	rear = 0;
}

void Container::put(int item){

	if(length==size){
		return;
	}
	else {
		items[rear] = item;
		rear = (rear+1)%size;
		length++;
	}
}

int Container::get(){
	if(length==0){
		return -1;
	}
	else {
		int item = items[front];
		front = (front+1)%size;
		length--;
		return item;
	}
}
