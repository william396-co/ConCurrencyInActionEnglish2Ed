#include "DesigningLockFreeConcurrentDataStructures.h"


namespace desiging_lockfree_concurrent_data_structures {
	std::mutex iom;
}


void desiging_lockfree_concurrent_data_structure_example() {

	using namespace desiging_lockfree_concurrent_data_structures;
	{
		using namespace definition_and_consequence;
	}
	{
		using namespace example_of_lockfree_data_structure;

		{
			using namespace writing_a_threadsafe_stack;
			{
				using namespace list_7_2;
#if 0
				lock_free_stack<int> stk;
				std::thread t(test_free_stack, std::ref(stk), 1, 5);
				std::thread t2(test_free_stack, std::ref(stk), 6, 10);
				t2.join();
				t.join();


				int cnt = 9;
				while (--cnt) {
					int val = 0;
					stk.pop(val);
					std::cout << val << "\n";
				}
#endif
			}
			{
				using namespace list_7_3;
#if 0
				lock_free_stack<int> stk;
				std::thread t(test_free_stack, std::ref(stk), 1, 5);
				std::thread t2(test_free_stack, std::ref(stk), 6, 10);
				t2.join();
				t.join();


				int cnt = 9;
				while (--cnt) {
					auto val = stk.pop();
					std::cout << *val << "\n";
				}
#endif
			}
		}
		{
			using namespace managing_memory_in_lockfree_data_structure;
			{
				using namespace list_7_4;
				lock_free_stack<int> stk;
				std::thread t(test_free_stack, std::ref(stk), 1, 5);
				std::thread t2(test_free_stack, std::ref(stk), 6, 10);
				t2.join();
				t.join();


				std::atomic<int> cnt = 10;
				auto pop_func = [](std::atomic<int>&cnt, lock_free_stack<int>&stk) {
					while (cnt.fetch_sub(1)) {
						auto val = stk.pop();
						if (val.get()) {
							std::lock_guard lk(iom);
							std::cout << "[" << std::this_thread::get_id() << "] " << *val << "\n";
						}
					}
				};

				std::thread t3(pop_func, std::ref(cnt), std::ref(stk));
				std::thread t4(pop_func, std::ref(cnt), std::ref(stk));
				//std::thread t5(pop_func, std::ref(cnt), std::ref(stk));
				//std::thread t6(pop_func, std::ref(cnt), std::ref(stk));
				t4.join();
				t3.join();
				//t5.join();
				//t6.join();
			}
		}
	}
}