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
					using namespace list_6_6;

#if 0
					threadsafe_queue<int> iQ;
					std::thread process(process_queue<int>, std::ref(iQ));
					std::thread consume(consume_queue<int>, std::ref(iQ));
					std::thread consume2(consume_queue<int>, std::ref(iQ));
					std::thread process2(process_queue<int>, std::ref(iQ));

					process.join();
					process2.join();
					consume.join();
					consume2.join();
#endif
				}
				{

					using namespace list_6_7;

#if 0
					threadsafe_queue<int> iQ;
					std::thread process(process_queue<int>, std::ref(iQ));
					std::thread consume(consume_queue<int>, std::ref(iQ));
					std::thread consume2(consume_queue<int>, std::ref(iQ));
					std::thread process2(process_queue<int>, std::ref(iQ));

					process.join();
					process2.join();
					consume.join();
					consume2.join();
#endif
				}
			}
		}
	}

	// 6.3 Desiging more complex lock-based data structures
	{
		using namespace desiging_more_comple_lock_based_data_structures;

		{
			using namespace list_6_11;
			
		}

		{
			using namespace list_6_12;
			threadsafe_list<int> slist;
			slist.push_front(1);
			slist.push_front(2);
			slist.push_front(3);

			slist.for_each([](auto item) {
				std::cout << item << "\n";
				});

			slist.remove_if([](auto key) {return key == 3;});

			slist.for_each([](auto item) {
				std::cout << item << "\n";
				});
		}
	}
	
}