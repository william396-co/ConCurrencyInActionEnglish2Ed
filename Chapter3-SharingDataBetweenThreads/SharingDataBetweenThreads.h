#pragma once

#include <thread>
#include <mutex>
#include <list>
#include <algorithm>
#include <vector>
#include <iostream>
#include <chrono>
#include <string>
#include <exception>
#include <stack>
#include <memory>

// 3.1 Problems with sharing data between threads
namespace problems_with_sharing_data_between_threads {

}
// 3.2 Protecting shared data with mutexes
namespace protecting_shared_data_with_mutexes
{

	// 3.2.1 Using mutexes in C++
	namespace using_mutex_in_cpp {

		// List 3.1 Protecting a list with a mutex
		namespace list_3_1 {

			extern std::list<int> some_list;
			extern std::mutex mtx;
			void add_to_list(int new_val);
			bool list_contains(int value_to_find);

			void test_list_3_1();
		}
	}

	// 3.2.2 Structuring code for protecting shared data
	namespace code_for_protecting_shared_data {
		// List 3.2 Accidentally passing out a reference to proteced data
		namespace list_3_2 {

			class some_data {
			private:
				int a;
				std::string b;
			public:
				void do_something() {
					std::cout << "do_something()\n";
				}
			};
			class data_wrapper {
			public:
				template<typename Func>
				void process_data(Func func) {
					std::lock_guard lock(mtx);
					func(data);
				}
			private:
				std::mutex mtx;
				some_data data;
			};

			extern some_data* unprotected;
			void malicious_function(some_data& protect_data);
			extern data_wrapper x;
			void foo();
		}
	}

	// 3.2.3 Spotting race conditions Inherent in Interfaces
	namespace spotting_race_condition {
		// List 3.4 An outline class definition for a thread-safe stack
		namespace list_3_4 {
			struct empty_stack : std::exception {
				const char* what()const noexcept;
			};

			template<typename T>
			class threadsafe_stack {
			public:
				threadsafe_stack() = default;
				threadsafe_stack(threadsafe_stack const&) = delete;
				threadsafe_stack& operator=(threadsafe_stack const&) = delete;

				void push(T new_val);
				std::shared_ptr<T> pop();
				void pop(T& value);
				bool empty()const;
			};
		}
		// List 3.5 A fleshed-out class definition for a thread-safe stack
		namespace list_3_5 {
			struct empty_stack : std::exception {
				const char* what()const throw();
			};

			template<typename T>
			class threadsafe_stack {
			public:
				threadsafe_stack() = default;
				threadsafe_stack(threadsafe_stack const& other) {
					std::lock_guard lock(mtx);
					data = other.data;
				}
				threadsafe_stack& operator=(threadsafe_stack const&) = delete;

				void push(T new_val) {
					std::lock_guard lock(mtx);
					data.push(std::move(new_val));
				}
				std::shared_ptr<T> pop() {
					std::lock_guard lock(mtx);
					if (data.empty()) throw empty_stack{};
					std::shared_ptr<T> const res(std::make_shared<T>(data.top()));
					data.pop();
					return res;
				}
				void pop(T& value) {
					std::lock_guard lock(mtx);
					if (data.empty()) throw empty_stack{};
					value = data.top();
					data.pop();
				}
				bool empty()const {
					std::lock_guard lock(mtx);
					return data.empty();
				}
			private:
				std::stack<T> data;
				mutable std::mutex mtx;
			};
		}
	}

	// 3.2.4 Deadlock: the problem and a solution
	namespace deadlock_and_solution {
		// list 3.6 Using std::lock() and std::lock_guard() in a swap operation
		namespace list_3_6 {
			class some_big_data {};
			void swap(some_big_data& lhs, some_big_data& rhs);
			class X {
			public:
				X(some_big_data const& sd) :some_detail{ sd } {}
#if __cplusplus >= 201703
				friend void swap(X& lhs, X& rhs) {
					if (&lhs == &rhs)return;
					std::scoped_lock guard(lhs.mtx, rhs.mtx);
					swap(lhs.some_detail, rhs.some_detail);
				}
#else
				friend void swap(X& lhs, X& rhs) {
					if (&lhs == &rhs)return;
					std::lock(lhs.mtx, rhs.mtx);
					std::lock_guard lock_a(lhs.mtx, std::adopt_lock);// transfer mtx ownership to lock_a, then automatically unlock when release
					std::lock_guard lock_b(rhs.mtx, std::adopt_lock);// transfer mtx ownership to lock_b, then automatically unlock when release
					swap(lhs.some_detail, rhs.some_detail);
				}
#endif				
			private:
				some_big_data some_detail;
				std::mutex mtx;
			};
		}
	}

	// 3.2.5 Further guidelines for avoiding deadlock	
	namespace further_guidelines_for_avoid_deadlock 
	{
		// Avoid Nested lock
		// Avoid calling user-supplied code while holding a lock
		// Acquire locks in a fixed order
		// Use a lock hierarchy
		namespace use_a_lock_hierarchy {
			class hierarchy_mutex {
			public:
				explicit hierarchy_mutex(std::size_t level)
					:hierarchy_value(level), previous_hierarchy_value{}
				{}

				void lock() {
					check_for_hierarchy_violation();
					internal_mutex.lock();
					update_hierarchy_value();
				}
				void unlock() {
					if (this_thread_hierarchy_value != hierarchy_value)
						throw std::logic_error("mutex hierarchy violated");
					this_thread_hierarchy_value = previous_hierarchy_value;
					internal_mutex.unlock();
				}
				bool try_lock() {
					check_for_hierarchy_violation();
					if (!internal_mutex.try_lock())
						return false;
					update_hierarchy_value();
					return true;
				}
			private:
				void check_for_hierarchy_violation() {
					if (this_thread_hierarchy_value <= hierarchy_value) {
						throw std::logic_error("mutex hierarchy violated");
					}
				}
				void update_hierarchy_value() {
					previous_hierarchy_value = this_thread_hierarchy_value;
					this_thread_hierarchy_value = hierarchy_value;
				}
				std::mutex internal_mutex;
				std::size_t const hierarchy_value;
				std::size_t previous_hierarchy_value;
				// 每个线程独有 静态变量(thread_a thread_b 不共享)
				static thread_local std::size_t this_thread_hierarchy_value;
			};

			extern hierarchy_mutex high_level_mutex;
			extern hierarchy_mutex low_level_mutex;
			extern hierarchy_mutex other_mutex;
			int do_low_level_stuff();
			inline int low_level_func() {
				std::lock_guard<hierarchy_mutex> lock(low_level_mutex);
				return do_low_level_stuff();
			}

			void high_level_stuff(int some_param);
			inline void high_level_func() {
				std::lock_guard<hierarchy_mutex> lock(high_level_mutex);
				high_level_stuff(low_level_func());
			}
			inline void thread_a() {
				high_level_func();
			}

			void do_other_stuff();
			inline void other_stuff() {
				high_level_func();
				do_other_stuff();
			}
			inline void thread_b() {
				std::lock_guard<hierarchy_mutex> lk(other_mutex);
				other_stuff();
			}
		}		
	}

	// 3.2.6 Flexible locking with std::unique_lock
	namespace flexible_lock {
		// List 3.9 Using std::lock() and std::unique_lock in a swap operation
		namespace list_3_9 {
			class some_big_data {};
			void swap(some_big_data& lhs, some_big_data& rhs);
			class X {
			public:
				X(some_big_data const& sd) :some_detail{ sd } {}
				friend void swap(X& lhs, X& rhs) {
					if (&lhs == &rhs)return;
					std::unique_lock lock_a(lhs.mtx, std::defer_lock);// defer lock(lock later)
					std::unique_lock lock_b(rhs.mtx, std::defer_lock);// defer lock(lock later)
					std::lock(lock_a,lock_b);// multiple std::unique_lock objects pass to std::lock()
					swap(lhs.some_detail, rhs.some_detail);
				}	
			private:
				some_big_data some_detail;
				std::mutex mtx;
			};
		}
	}

	// 3.2.7 Transfering mutex ownership between scopes
	namespace transfering_mutex_ownership {

		inline void prepare_data() {
			std::cout << "start prepare_data\n";
			std::this_thread::sleep_for(std::chrono::milliseconds{ 100 });
			std::cout << "finished prepare_data\n";
		}

		inline std::unique_lock<std::mutex> get_lock() {
			extern std::mutex some_mtx;
			std::unique_lock lk(some_mtx);
			prepare_data();
			return lk;
		}


		inline void do_something() {
			std::cout << "start do something\n";
			std::this_thread::sleep_for(std::chrono::milliseconds{ 100 });
			std::cout << "finished do something\n";
		}
		inline void process_data() {
			std::unique_lock lk(get_lock());
			do_something();
		}
	}
}

void sharing_data_between_threads_example(); 