#pragma once

#include <iostream>
#include <thread>
#include <atomic>
#include <functional>
#include <vector>
#include <memory>
#include <chrono>
#include <future>
#include <type_traits>

#include "../Common/threadsafe_queue.h"
#include "../Common/join_thread.h"

namespace advanced_thread_management {

	// 9.1 Thread pools
	namespace thread_pools {
		// 9.1.1 the simplest possible thread pool
		namespace simplest_possible_thread_pool {
			// list 9.1 Simple thread pool
			namespace list_9_1 {

				class thread_pool {
				private:
					std::atomic<bool> done;					
					threadsafe_queue<std::function<void()>> work_queue;		
					std::vector<std::thread> threads;
					join_threads joiner;

					void work_thread() {
						while (!done) {
							std::function<void()> task;
							if (work_queue.try_pop(task)) {
								task();
							}
							else {
								std::this_thread::yield();
							}
						}

					}
				public:
					thread_pool()
						: done{ false }, joiner{ threads } 
					{
						unsigned long const thread_count = std::thread::hardware_concurrency();
						try
						{
							for (unsigned i = 0; i < thread_count;++i) {
								threads.push_back(std::thread(&thread_pool::work_thread, this));
							}
						}
						catch (...)
						{
							done = true;
							throw;
						}
					}
					~thread_pool() {
						done = true;
					}

					template<typename Func>
					void submit(Func fn) {
						work_queue.push(std::function<void()>(fn));
					}
				};
			}
		}
		// 9.1.2 waiting for tasks submitted to a thread pool
		namespace waiting_for_tasks_submitted_thread_pool {
			// list 9.2 A thread pool with waitable tasks
			namespace list_9_2 {
				class function_wrapper {
					struct impl_base {
						virtual void call() = 0;
						virtual ~impl_base() {}
					};
					std::unique_ptr<impl_base> impl;
					template<typename Func>
					struct impl_type : impl_base {
						Func fn;
						impl_type(Func&& fn_)
							:fn(std::move(fn_))
						{
						}
						void call() override { fn(); }
					};
				public:
					template<typename Func>
					function_wrapper(Func&& fn)
						:impl(new impl_type<Func>(std::move(fn)))
					{
					}
					void operator()() { impl->call(); }
					function_wrapper() = default;
					function_wrapper(function_wrapper&& other)noexcept :
						impl(std::move(other.impl))
					{ }
					function_wrapper& operator=(function_wrapper&& other)noexcept {
						if (this != &other) {
							impl = std::move(other.impl);
						}
						return *this;
					}
					function_wrapper(function_wrapper const&) = delete;
					function_wrapper& operator=(function_wrapper const&) = delete;
				};
				class thread_pool {
				private:
					std::atomic<bool> done;
					threadsafe_queue<function_wrapper> work_queue;
					std::vector<std::thread> threads;
					join_threads joiner;

					void work_thread() {
						while (!done) {
							function_wrapper task;
							if (work_queue.try_pop(task)) {
								task();
							}
							else {
								std::this_thread::yield();
							}
						}

					}
				public:
					thread_pool()
						: done{ false }, joiner{ threads }
					{
						unsigned long const thread_count = std::thread::hardware_concurrency();
						try
						{
							for (unsigned i = 0; i < thread_count;++i) {
								threads.push_back(std::thread(&thread_pool::work_thread, this));
							}
						}
						catch (...)
						{
							done = true;
							throw;
						}
					}
					~thread_pool() {
						done = true;
					}

					template<typename Func>
					std::future<std::result_of_t<Func()>>
								submit(Func fn) {
						typedef typename std::result_of_t<Func()> result_type;

						std::packaged_task<result_type()> task(std::move(fn));
						std::future<result_type> res(task.get_future());
						work_queue.push(std::move(task));
						return res;
					}
				};
			}
		}
	}
}

void advanced_thread_management_example();