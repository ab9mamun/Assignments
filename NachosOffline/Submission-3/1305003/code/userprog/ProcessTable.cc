/*
 * ProcessTable.cc
 *
 *  Created on: May 11, 2017
 *      Author: mamun
 */
#include "ProcessTable.h"


/* Create a table to hold at most "size" entries. */
ProcessTable::ProcessTable(int size) {
	length = 0;
	table = new HashTable(size);
	map = new BitMap(size);
	lock = new Lock("Process Table lock");
}

/* Allocate a table slot for "object", returning the "index" of the
   allocated entry; otherwise, return -1 if no free slots are available. */
int ProcessTable::Alloc(void *object){
	lock->Acquire();

	int index = map->Find();
	if(index>=0){
		table->put(index, object);
		length++;
	}

	lock->Release();
	return index;
}

/* Retrieve the object from table slot at "index", or NULL if that
   slot has not been allocated. */
void * ProcessTable::Get(int index) {
	lock->Acquire();
	void* val = table->get(index);
	lock->Release();
	return val;
}

/* Free the table slot at index. */
void ProcessTable::Release(int index) {
	//printf("inside release process with index %d\n", index);
	lock->Acquire();
//	printf("inside release process with index %d\n", index);
	if(length>0){
	//	printf("inside release process with index %d\n", index);
		map->Clear(index);
		//printf("inside release process with index %d\n", index);
		table->remove(index);
		//printf("inside release process with index %d\n", index);
		length--;
	}
	lock->Release();
}

bool ProcessTable::empty(){
	lock->Acquire();
	bool result =  (length==0);
	lock->Release();
	return result;
}

ProcessTable::~ProcessTable() {
	delete table;
	delete map;
	delete lock;
}

