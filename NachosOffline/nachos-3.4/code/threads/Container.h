#include "copyright.h"
#include "system.h"
#include "synch.h"

#define SIZE 10


class Container{
	Lock* lock;
	Condition* prodCondition;
	Condition* consCondition;
	int length;
	int front;
	int rear;
	int size= SIZE;
	int items[SIZE];


public:
	Container();
	void put(int item);
	int get();
	~Container();
};
