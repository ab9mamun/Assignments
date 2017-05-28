/*
 * SwapPage.h
 *
 *  Created on: May 28, 2017
 *      Author: mamun
 */

#ifndef USERPROG_SWAPPAGE_H_
#define USERPROG_SWAPPAGE_H_

#include "system.h"
#include "copyright.h"

class SwapPage{

private:
	char* bytes;

public:
	SwapPage();
	~SwapPage();
	int writeByte(int position, char data);
	int writePage(int physicalPage);
	int readPage(int physicalPage);

};



#endif /* USERPROG_SWAPPAGE_H_ */
