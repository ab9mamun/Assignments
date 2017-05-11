/*
 * ProcessTable.h
 *
 *  Created on: May 11, 2017
 *      Author: mamun
 */

#ifndef USERPROG_PROCESSTABLE_H_
#define USERPROG_PROCESSTABLE_H_

#include "copyright.h"
#include "system.h"
#include "bitmap.h"
#include "list.h"
#include "synch.h"
#include "HashTable.h"

using namespace std;

class ProcessTable{

HashTable* table;
BitMap* map;
Lock* lock;
int length;
public:
/* Create a table to hold at most "size" entries. */
	ProcessTable(int size);
	/* Allocate a table slot for "object", returning the "index" of the
	   allocated entry; otherwise, return -1 if no free slots are available. */
	int Alloc(void *object);

	/* Retrieve the object from table slot at "index", or NULL if that
	   slot has not been allocated. */
	void *Get(int index);

	/* Free the table slot at index. */
	void Release(int index);
	bool empty();

	~ProcessTable();

};



#endif /* USERPROG_PROCESSTABLE_H_ */
