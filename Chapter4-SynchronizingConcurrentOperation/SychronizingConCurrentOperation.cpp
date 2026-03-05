#include "SychronizingConCurrentOperation.h"

#include <cassert>

namespace waiting_for_an_event_or_other_condition {

	namespace waiting_for_a_condition_with_condition_variable
	{
		namespace list_4_1 {
			std::mutex mtx;
			std::queue<data_chunk> data_queue;
			std::condition_variable data_cond;
			bool more_data_to_prepare{ true };
			std::mutex cout_mtx;
		}
	}
	namespace building_a_thread_safe_queue {
		namespace list_4_5 {
			threadsafe_queue<data_chunk> data_queue;
			std::mutex cout_mtx;
			bool more_data_to_prepare{ true };
		}
	}
}

// 4.2 Waiting for one-off events with futures
namespace waiting_for_one_off_events_with_futures {
	// 4.2.1 Returning values from background tasks
	namespace return_value_from_background_task {

		namespace list_4_6 {
			int find_the_answer_to_ltuae() {
				std::this_thread::sleep_for(std::chrono::seconds{ 2 });
				return 42;
			}
			void do_other_stuff() {
				std::cout << "do_other_stuff start\n";
				std::this_thread::sleep_for(std::chrono::milliseconds{ 300 });
				std::cout << "do_other_stuff end\n";
			}
		}

		namespace list_4_7 {
		}
	}

	// 4.2.2 Associating a task with a future
	namespace associating_a_task_with_a_future {
		namespace list_4_9 {

			std::deque<std::packaged_task<void()>> tasks;
			std::mutex mtx;
			bool gui_shutdown_message_recieved{ false };
			void get_and_process_gui_message()
			{
				std::cout << "get_and_process_gui_message()\n";
			}
		}
	}
}
void synchronzing_concurrent_operation_example() {
	
	// 4.1 Waiting for an event or other condition
	{
		using namespace waiting_for_an_event_or_other_condition;

		// 4.1.1 Waiting for a condition with condition variable
		{			
			using namespace waiting_for_a_condition_with_condition_variable;
			{
				using namespace list_4_1;

#if 0
				std::thread t(data_preparation_thread);
				std::vector<std::thread> ts;
				for (int i = 0; i != 10;++i) {
					ts.emplace_back(data_processing_thread);
				}
				t.join();
				for (auto& t : ts)
					t.join();
#endif

			}
		}

		// 4.1.2 Building a thread-safe queue with condition variables
		{			
			using namespace building_a_thread_safe_queue;
			{
				using namespace list_4_5;
#if 0
				std::thread t(data_preparation_thread);
				std::vector<std::thread> ts;
				for (int i = 0; i != 10;++i) {
					ts.emplace_back(data_processing_thread);
				}
				t.join();
				for (auto& t : ts)
					t.join();
#endif

			}

		}
	}

	// 4.2 Waiting for one-off events with futures
	{
		using namespace waiting_for_one_off_events_with_futures;
		// 4.2.1 Returning values from background tasks
		{
			using namespace return_value_from_background_task;
			{
				using namespace list_4_6;
				std::future<int> the_answer = std::async(find_the_answer_to_ltuae);
				do_other_stuff();
				std::cout << "Ths answer is: " << the_answer.get() << "\n";
			}
			{
				using namespace list_4_7;
				X x;
				auto f1 = std::async(&X::foo, &x, 41, "hello");
				auto f2 = std::async(&X::bar, x, "goodbye");

				Y y;

				auto f3 = std::async(Y(), 3.14);

				auto f4 = std::async(std::ref(y), 2.17);
				
				std::async(baz, std::ref(x));

				auto f5 = std::async(move_only());

#if 0
				f5.get();	
				f4.get();
				f3.get();
				f2.get();
				f1.get();	
#endif

				auto f6 = std::async(std::launch::async, Y(), 1.2);
				auto f7 = std::async(std::launch::deferred, baz, std::ref(x));
				auto f8 = std::async(std::launch::deferred | std::launch::async,
					baz, std::ref(x));

				auto f9 = std::async(baz, std::ref(x));

				f7.wait();
			}
		}
		// 4.2.2 Associating a task with a future
		{
			using namespace associating_a_task_with_a_future;
			{
				using namespace list_4_9;
				std::thread gui_bg_thread(gui_thread);
							
				std::thread post_message_thread(
					[] (){
						while (!gui_shutdown_message_recieved) {
							post_task_for_gui_thread([]() {
								auto i = rand() % 100;
								std::cout << "post a message[" << i << "]\n";
								if (42 == i) {
									gui_shutdown_message_recieved = true;
								}
								std::this_thread::sleep_for(std::chrono::milliseconds{ 300 });
								});
						}					
					});
				post_message_thread.join();
				gui_bg_thread.join();
			}
		}

		// 4.2.4 Saving an exception for the future
		{
			using namespace saving_an_exception_for_the_future;

			std::promise<double> except;
			try {
				auto f = std::async(square_root, -1);
				double y = f.get();
			}
			catch (...) {
				//except.set_exception(std::current_exception());
				except.set_exception(std::make_exception_ptr(std::logic_error("foo")));
			}
		}

		// 4.2.5 Waiting from multiple threads
		{
			using namespace waiting_from_multiple_threads;

			std::promise<int> p;
			std::future<int> f(p.get_future());

			assert(f.valid());

			std::shared_future<int> sf(std::move(f));
			//assert(f.valid());//fire
			assert(sf.valid());
		}
	}
}