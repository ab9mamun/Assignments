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
	bool valid;
	int processId;
	int vpn;
	unsigned char* bytes;

public:
	SwapPage();
	~SwapPage();
	int writeByte(int position, char data);

	void assignPage(int processId, int vpn);
	void invalidate();



};



#endif /* USERPROG_SWAPPAGE_H_ */
