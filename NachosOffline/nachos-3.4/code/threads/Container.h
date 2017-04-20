#ifndef CONTAINER_H
#define CONTAINER_H


#include "copyright.h"
#include "system.h"
#include "synch.h"

#define DEFAULT_SIZE 10


class Container{
	Lock* lock;
	Condition* prodCondition;
	Condition* consCondition;
	int length;
	int front;
	int rear;
	int size;
	int* items;


public:
	Container();
	Container(int size);

	void put(int item);
	int get();
	~Container();
};



#endif //CONTAINER_H
