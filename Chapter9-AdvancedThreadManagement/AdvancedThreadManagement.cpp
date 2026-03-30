#include "AdvancedThreadManagement.h"

void advanced_thread_management_example() {

	extern std::mutex iom;
	using namespace advanced_thread_management;

	// 9.1 Thread pools
	{
		using namespace thread_pools;
		{

			// 9.1.1 the simplest possible thread pool
			using namespace simplest_possible_thread_pool;
			{

				{	// list 9.1 Simple thread pool
					using namespace list_9_1;

#if 0
					thread_pool pool;
					for (int i = 0; i != 10;++i) {
						pool.submit([i]() {
							//std::this_thread::sleep_for(std::chrono::milliseconds{ 100 });
							std::cout << "[" << std::this_thread::get_id() << "] idx= " << i << "\n";
							});
					}

					std::this_thread::sleep_for(std::chrono::seconds{ 1 });
#endif
				}
			}
			{
				using namespace waiting_for_tasks_submitted_thread_pool;
				{
					using namespace list_9_2;
					thread_pool pool;
					auto res1= pool.submit([]() {
						 std::lock_guard lock(iom);
						std::cout << "[" << std::this_thread::get_id() << "] running \n";
						});

					int i = 42;
					auto res2 = pool.submit([&i]() {
						std::lock_guard lock(iom);
						std::cout << "[" << std::this_thread::get_id() << "] idx= " << i << "\n";
						std::this_thread::sleep_for(std::chrono::milliseconds{ 100 });
						return 42;
						});

					std::cout << res2.get() << "\n";
					res1.get();
				}
			}
		}

	}
	// 9.2 Interrupting threads
	{
		using namespace interrupting_threads;
		

		// 9.2.1 Launching and interrupting another thread
		{
			using namespace launching_and_interrupting_another_thread;
			thread_local launching_and_interrupting_another_thread::interrupt_flag this_thread_interrupt_flag;
			{
				using namespace list_9_9;				
			}		
		}
		// 9.2.3 Interrupting a condition variable wait
		{
			using namespace interrutping_a_cond_var_wait;

			{
				using namespace list_9_11;
				thread_local list_9_11::interrupt_flag this_thread_interrupt_flag;
			}

		}
	}
}