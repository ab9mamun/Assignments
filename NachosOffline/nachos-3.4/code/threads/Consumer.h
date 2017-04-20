#include "thread.h"
#include "copyright.h"
#include "system.h"

class Consumer{
		int id;
	public:
		Consumer(int id);
			void start();
			void run(int notUsed);
			int consume();
			~Consumer();
};
