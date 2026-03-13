#include "DesigningLockbasedConcurrentDataStructures.h"

#include <iostream>

namespace designing_lock_based_data_structure {
	std::mutex iom;
	namespace lock_based_concurrent_data_structures {

		namespace list_6_1 {

			const char* empty_stack::what() const throw()
			{
				return "empty stack";
			}
		}
	}
}

void desiging_lockbased_concurrent_data_structure_example() {

	using namespace designing_lock_based_data_structure;

	// 6.2 Lock-based concurrent data structures
	{
		using namespace lock_based_concurrent_data_structures;
		{

			// 6.2.1 A thread-safe stack using locks
			{
				using namespace list_6_1;

#if 0
				threadsafe_stack<int> iStk;
				std::thread process(process_stack<int>,std::ref(iStk));
				std::thread consume(consume_stack<int>,std::ref(iStk));
				std::thread consume2(consume_stack<int>, std::ref(iStk));

				process.join();
				consume.join();
				consume2.join();
#endif
			}

			//
			{
				using namespace thread_safe_queue_example;
				{
					using namespace list_6_2;

#if 0
					threadsafe_queue<int> iQ;
					std::thread process(process_queue<int>, std::ref(iQ));
					std::thread consume(consume_queue<int>, std::ref(iQ));
					std::thread consume2(consume_queue<int>, std::ref(iQ));

					process.join();
					consume.join();
					consume2.join();
#endif
				}
				{
					using namespace list_6_3;

					threadsafe_queue<int> iQ;
					std::thread process(process_queue<int>, std::ref(iQ));
					std::thread consume(consume_queue<int>, std::ref(iQ));
					std::thread consume2(consume_queue<int>, std::ref(iQ));

					process.join();
					consume.join();
					consume2.join();
				}
			}
		}
	}
	
}