#include "thread.h"
#include "copyright.h"
#include "system.h"

#include "Container.h"

class Consumer{
		int id;
		Container* container;
	public:
		Consumer(int id, Container* container);

		int consume();
		~Consumer();
};
