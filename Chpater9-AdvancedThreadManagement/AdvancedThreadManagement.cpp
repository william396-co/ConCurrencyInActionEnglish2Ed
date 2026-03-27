#include "AdvancedThreadManagement.h"

void advanced_thread_management_example() {

	using namespace advanced_thread_management;

	{
		// 9.1 Thread pools
		using namespace thread_pools;
		{

			// 9.1.1 the simplest possible thread pool
			using namespace simplest_possible_thread_pool;
			{

				{	// list 9.1 Simple thread pool
					using namespace list_9_1;

					thread_pool pool;
					for (int i = 0; i != 10;++i) {
						pool.submit([i]() {
							//std::this_thread::sleep_for(std::chrono::milliseconds{ 100 });
							std::cout << "[" << std::this_thread::get_id() << "] idx= " << i << "\n";
							});
					}

					std::this_thread::sleep_for(std::chrono::seconds{ 1 });
				}
			}
			{
				using namespace waiting_for_tasks_submitted_thread_pool;
				{
					using namespace list_9_2;
					thread_pool pool;
					 pool.submit([]() {
						std::cout << "[" << std::this_thread::get_id() << "] \n";
						});

					auto res2 = pool.submit([](int i) {
						std::cout << "[" << std::this_thread::get_id() << "] idx= " << i << "\n";
						std::this_thread::sleep_for(std::chrono::milliseconds{ 100 });
						return 42;
						});

					std::cout << res2.get() << "\n";
				}
			}
		}
	}
}