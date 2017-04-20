#include "thread.h"
#include "copyright.h"
#include "system.h"

class Producer{
	int id;
public:

	Producer(int id);
	void produce(int item);
	~Producer();

};
