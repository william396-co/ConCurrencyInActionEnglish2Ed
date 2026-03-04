#include "SharingDataBetweenThreads.h"

// 3.2 Protecting shared data with mutexes
namespace protecting_shared_data_with_mutexes
{
	namespace using_mutex_in_cpp {
		namespace list_3_1 {
			std::list<int> some_list;
			std::mutex mtx;

			void add_to_list(int new_val) {
				std::lock_guard lock(mtx);
				some_list.emplace_back(new_val);
			}
			bool list_contains(int value_to_find) {
				std::lock_guard lock(mtx);
				return std::find(some_list.begin(), some_list.end(),
					value_to_find) != some_list.end();
			}

			void test_list_3_1() {
				std::vector<std::thread> ts;
				ts.emplace_back([]() {
					for (size_t i = 0; i != 10;++i) {
						add_to_list(i * i);
						std::this_thread::sleep_for(std::chrono::milliseconds{ 1 });
					}
					});
				ts.emplace_back([]() {
					for (size_t i = 10; i != 0;--i) {
						if (list_contains(i * i)) {
							std::cout << "[" << i * i << "] found in list\n";
						}
						else {
							std::cout << "[" << i * i << "] not found in list\n";
						}
					}
					});

				for (auto& t : ts)
					t.join();
			}
		}
	}
	namespace code_for_protecting_shared_data {
		namespace list_3_2 {

			some_data* unprotected;
			void malicious_function(some_data& protect_data) {
				unprotected = &protect_data;
			}
			data_wrapper x;
			void foo() {
				x.process_data(malicious_function);
				unprotected->do_something();
			}
		}
	}
	namespace deadlock_and_solution {
		namespace list_3_6 {
			void swap(some_big_data& lhs, some_big_data& rhs) {
				std::swap(lhs, rhs);
			}
		}
	}
	namespace further_guidelines_for_avoid_deadlock {
		namespace use_a_lock_hierarchy {
			
			thread_local std::size_t hierarchy_mutex::this_thread_hierarchy_value(UINT64_MAX);

			hierarchy_mutex high_level_mutex(10000);
			hierarchy_mutex low_level_mutex(5000);
			hierarchy_mutex other_mutex(6000);
			int do_low_level_stuff()
			{
				std::cout << "do_low_level_stuff\n";
				return 42;
			}
			void high_level_stuff(int some_param)
			{
				std::cout << "high_level_stuff(" << some_param << ")\n";
			}
			void do_other_stuff()
			{
				std::cout << "do_other_stuff()\n";
			}
		}
	}
	namespace flexible_lock {
		namespace list_3_9 {
			void swap(some_big_data& lhs, some_big_data& rhs) {
				std::swap(lhs, rhs);
			}
		}
	}
	namespace transfering_mutex_ownership {
		std::mutex some_mtx;
	}
}

// 3.3 Alternative facilities for protecting shared data
namespace alternative_facilities_for_protecting_shared_data {
	// 3.3.1 Protecting shared data during initialization
	namespace protecting_shared_data_during_initialization {
		namespace list_3_11 {
			std::shared_ptr<some_resource> resource_ptr;
			std::mutex resource_mtx;
			std::once_flag resource_flag;
		}
	}

	// 3.3.2 Protecting rarely updated data structures
	namespace protecting_rarely_updated_data {

	}

	// 3.3.3 Recursive locking
	namespace recursive_locking {

	}
}

void sharing_data_between_threads_example() {
	// 3.1
	{
		using namespace problems_with_sharing_data_between_threads;
	}

	// 3.2 Protecting shared data with mutexes
	{
		using namespace protecting_shared_data_with_mutexes;
		// 3.2.1 Using mutexes in C++
		{
			using namespace using_mutex_in_cpp;
			{
				using namespace list_3_1;
				test_list_3_1();
			}
		}
		// 3.2.2 Structuring code for protecting shared data
		{
			using namespace code_for_protecting_shared_data;
			{
				using namespace list_3_2;
				foo();
			}
		}
		// 3.2.3 Spotting race conditions Inherent in Interfaces
		{
			using namespace spotting_race_condition;
		}
		// 3.2.4 Deadlock: the problem and a solution
		{
			using namespace deadlock_and_solution;
			{
				using namespace list_3_6;
			}
		}
		// 3.2.5 Further guidelines for avoiding deadlock
		{
			using namespace further_guidelines_for_avoid_deadlock;
			// Use a lock hierarchy
			{
				using namespace use_a_lock_hierarchy;
				
#if 1
				std::thread t1(thread_a);
				t1.join();
#else

				std::thread t(thread_b);//failed
				t.join();
#endif
			}
		}


		// 3.2.7 Transfering mutex ownership between scopes
		{
			using namespace transfering_mutex_ownership;
			
			std::vector<std::thread> ts;
			for (int i = 0;i != 3;++i) {
				ts.emplace_back(process_data);
			}
			for (auto& t : ts)
				t.join();
		}
		// 3.2.8 Locking at an appropriate granularity
		{
			using namespace locking_at_an_appropriate_granularity;

		}
	}

	// 3.3 Alternative facilities for protecting shared data
	{
		using namespace alternative_facilities_for_protecting_shared_data;

		// 3.3.1 Protecting shared data during initialization
		{
			using namespace protecting_shared_data_during_initialization;

			
			{
				using namespace list_3_11;
				std::vector<std::thread> ts;
				for (int i = 0; i != 10;++i) {
					ts.emplace_back(foo);
					//ts.emplace_back(undefined_behavior_with_double_checked_lock);
				}
				for (auto& t : ts)
					t.join();

			}
		}

		// 3.3.2 Protecting rarely updated data structures
		{
			using namespace protecting_rarely_updated_data;
			{
				using namespace list_3_13;				

			}
		}
	}
}