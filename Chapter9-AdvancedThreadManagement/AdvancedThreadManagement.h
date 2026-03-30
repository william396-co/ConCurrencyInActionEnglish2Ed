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
#include <numeric>
#include <list>
#include <mutex>
#include <future>
#include <condition_variable>

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
			{
			}
			function_wrapper& operator=(function_wrapper&& other)noexcept {
				if (this != &other) {
					impl = std::move(other.impl);
				}
				return *this;
			}
			function_wrapper(function_wrapper const&) = delete;
			function_wrapper& operator=(function_wrapper const&) = delete;
		};
		// 9.1.2 waiting for tasks submitted to a thread pool
		namespace waiting_for_tasks_submitted_thread_pool {

			// list 9.2 A thread pool with waitable tasks
			namespace list_9_2 {
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
					std::future<std::invoke_result_t<Func>>
								submit(Func fn) {
						using result_type = std::invoke_result_t<Func>;

						std::packaged_task<result_type()> task(std::move(fn));
						std::future<result_type> res(task.get_future());
						work_queue.push(std::move(task));
						return res;
					}
				};
			}

			// list 9.3 parallel_accumulate using a thread pool with waitable tasks
			namespace list_9_3 {
				template<typename Iterator, typename T>
				struct accumulate_block {
					void operator()(Iterator first, Iterator last, T& result) {
						result = std::accumulate(first, last, result);
					}
				};

				template<typename Iterator, typename T>
				T parallel_accumulate(Iterator first, Iterator last, T init) {
					unsigned long const length = std::distance(first, last);
					if (!length)return init;

					unsigned long const block_size = 250000;
					unsigned long const num_blocks = (length + block_size - 1) / block_size;
					std::vector<std::future<T>> futures(num_blocks - 1);					
					list_9_2::thread_pool pool;
					Iterator block_start = first;
					for (unsigned long i = 0; i < (num_blocks - 1);++i) {
						Iterator block_end = block_start;
						std::advance(block_end, block_size);
						futures[i] = pool.submit([=] {
							accumulate_block<Iterator, T>()(block_start, block_end);
							});
						block_start = block_end;
					}
					T last_result = accumulate_block<Iterator, T>()(block_start, last);
					T result = init;
					for (unsigned long i = 0; i < num_blocks - 1;++i) {
						result += futures[i].get();
					}
					result += last_result;
					return result;
				}
			}			
		}

		// 9.1.3 Tasks that wait for other task
		namespace tasks_that_wait_for_other_task {

			// list 9.4 An implementation of run_pending_task()
			namespace list_9_4 {
				class thread_pool {
				private:
					std::atomic<bool> done;
					threadsafe_queue<function_wrapper> work_queue;
					std::vector<std::thread> threads;
					join_threads joiner;

					void work_thread() {
						while (!done) {
							run_pending_task();
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

					void run_pending_task() {
						function_wrapper task;
						if (work_queue.try_pop(task)) {
							task();
						}
						else {
							std::this_thread::yield();
						}
					}

					template<typename Func>
					std::future<std::invoke_result_t<Func>>
						submit(Func fn) {
						using result_type = std::invoke_result_t<Func>;

						std::packaged_task<result_type()> task(std::move(fn));
						std::future<result_type> res(task.get_future());
						work_queue.push(std::move(task));
						return res;
					}
				};
			}

			// list 9.5 A thread-pool-based implementation of Quicksort
			namespace list_9_5 {
				template<typename T>
				struct sorter {
					list_9_4::thread_pool pool;

					std::list<T> do_sort(std::list<T>& chunk_data) {
						if (chunk_data.empty())return chunk_data;

						std::list<T> result;
						result.splice(result.begin(), chunk_data, chunk_data.begin());
						T const& partition_val = *result.begin();
						typename std::list<T>::iterator divide_point =
							std::partition(chunk_data.begin(), chunk_data.end(),
								[&](T const& val) {return val < partition_val;}
							);
						std::list<T> new_lower_chunk;
						new_lower_chunk.splice(new_lower_chunk.end(), chunk_data, chunk_data.begin(), divide_point);

						std::future<std::list<T>> new_lower =
							pool.submit(std::bind(&sorter::do_sort, this, std::move(new_lower_chunk)));
						std::list<T> new_higher(do_sort(chunk_data));
						result.splice(result.end(), new_higher);
						while (new_lower.wait_for(std::chrono::seconds{ 0 }) == std::future_status::timeout) {
							pool.run_pending_task();
						}
						result.splice(result.begin(), new_lower.get());
						return result;
					}
				};
				template<typename T>
				std::list<T> parallel_quick_sort(std::list<T>input) {
					if (input.empty())return {};
					sorter<T> s;
					return s.do_sort(input);
				}
			}
		}
		// 9.1.4 Avoiding contention on the work queue
		namespace avoiding_contention_on_the_work_queue {
			// list 9.6 A thread pool with thread-local work queues
			namespace list_9_6 {
				class thread_pool {
				private:
					std::atomic<bool> done;
					std::vector<std::thread> threads;
					join_threads joiner;

					threadsafe_queue<function_wrapper> pool_work_queue;
					using local_queue_type = std::queue<function_wrapper>;
					static thread_local std::unique_ptr<local_queue_type> local_work_queue;

					void work_thread() {
						local_work_queue.reset(new local_queue_type);
						while (!done) {
							run_pending_task();
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

					void run_pending_task() {
						function_wrapper task;
						
						if (local_work_queue && !local_work_queue->empty()) {
							task = std::move(local_work_queue->front());
							local_work_queue->pop();
							task();
						}
						else if (pool_work_queue.try_pop(task)) {
							task();
						}
						else {
							std::this_thread::yield();
						}
					}

					template<typename Func>
					std::future<std::invoke_result_t<Func>>
						submit(Func fn) {
						using result_type = std::invoke_result_t<Func>;

						std::packaged_task<result_type()> task(std::move(fn));
						std::future<result_type> res(task.get_future());
						if (local_work_queue) {
							local_work_queue->push(std::move(task));
						}
						else {
							pool_work_queue.push(std::move(task));
						}
						return res;
					}
				};			
			}
		}
		// 9.1.5 Work stealing
		namespace work_stealing {
			// list 9.7 Lock-based quueue for work stealing
			namespace list_9_7 {
				class work_stealing_queue {
				private:
					using data_type = function_wrapper;
					mutable std::mutex the_mtx;
					std::deque<data_type> the_queue;
				public:
					work_stealing_queue() = default;
					work_stealing_queue(work_stealing_queue const&) = delete;
					work_stealing_queue& operator=(work_stealing_queue const&) = delete;

					void push(data_type data) {
						std::lock_guard lk(the_mtx);
						the_queue.push_front(std::move(data));
					}
					bool empty()const {
						std::lock_guard lk(the_mtx);
						return the_queue.empty();
					}
					bool try_pop(data_type& data) {
						std::lock_guard lk(the_mtx);
						if (the_queue.empty()) {
							return false;
						}
						data = std::move(the_queue.front());
						the_queue.pop_front();
						return true;
					}
					bool try_steal(data_type& data) {
						std::lock_guard lk(the_mtx);
						if (the_queue.empty()) {
							return false;
						}
						data = std::move(the_queue.back());
						the_queue.pop_back();
						return true;
					}
				};
			}

			// list 9.8 A thread pool that uses work stealing
			namespace list_9_8 {
				class thread_pool {
					using task_type = function_wrapper;
					using work_stealing_queue = list_9_7::work_stealing_queue;
					std::atomic<bool> done{};
					threadsafe_queue<task_type> pool_work_queue;
					std::vector<std::unique_ptr<work_stealing_queue>> queues;
					std::vector<std::thread> threads;
					join_threads joiner;
					static thread_local work_stealing_queue* local_work_queue;
					static thread_local unsigned my_index;

					void worker_thread(unsigned my_index_){
						my_index = my_index_;
						local_work_queue = queues[my_index].get();
						while (!done) {
							run_pending_task();
						}
					}

					bool pop_task_from_local_queue(task_type& task) {
						return local_work_queue && local_work_queue->try_pop(task);
					}

					bool pop_task_from_pool_queue(task_type& task) {
						return pool_work_queue.try_pop(task);
					}

					bool pop_task_from_other_queue(task_type& task) {
						for (unsigned i = 0; i != queues.size();++i) {
							unsigned const idx = (my_index + i + 1) % queues.size();
							if (queues[idx]->try_pop(task)) {
								return true;
							}
						}
						return false;
					}
				public:
					thread_pool()
						:done{ false }, joiner{ threads } 
					{
						unsigned const thread_count = std::thread::hardware_concurrency();
						try
						{
							for (unsigned i = 0; i < thread_count;++i) {
								queues.push_back(std::unique_ptr<work_stealing_queue>(new work_stealing_queue));
							}
							for (unsigned i = 0; i < thread_count;++i) {
								threads.push_back(std::thread(&thread_pool::worker_thread, this, i));
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
					void run_pending_task() {
						task_type task;
						if (pop_task_from_local_queue(task)
							|| pop_task_from_pool_queue(task)
							|| pop_task_from_other_queue(task)) {
							task();
						}
						else {
							std::this_thread::yield();
						}
					}
				};
			}
		}
	}

	// 9.2 Interrupting threads
	namespace interrupting_threads {


		class interrupt_flag;

		// 9.2.1 Launching and interrupting another thread
		namespace launching_and_interrupting_another_thread {

			class interrupt_flag {
			public:
				void set() {}
				bool is_set()const { return {}; }
			};
			extern thread_local interrupt_flag this_thread_interrupt_flag;
			// list 9.9 Basic implementation of interruptible_thread
			namespace list_9_9 {
				class interruptible_thread {
					std::thread internal_thread;
					interrupt_flag* flag;
				public:
					template<typename Func>
					interruptible_thread(Func f) {
						std::promise<interrupt_flag*> p;
						internal_thread = std::thread([f, &p]() {
							p.set_value(&this_thread_interrupt_flag);
							f();
							});
						flag = p.get_future().get();
					}
					void interrupt() {
						if (flag) {
							flag->set();
						}
					}
				};
			}
		}

		// 9.2.2 Detecting that a thread has been interrupted
		
		// 9.2.3 Interrupting a condition variable wait
		namespace interrutping_a_cond_var_wait {
			// list 9.11 Using a timeout in interrupting_wait for std::condition_variable
			namespace list_9_11 {
				class interrupt_flag;
				extern thread_local interrupt_flag this_thread_interrupt_flag;
				class interrupt_flag {
					std::atomic<bool> flag;
					std::condition_variable* thread_cond;
					std::mutex set_clear_mutex;
				public:
					interrupt_flag() :thread_cond{} {}
					void set() {
						flag.store(true, std::memory_order_relaxed);
						std::lock_guard lk(set_clear_mutex);
						if (thread_cond) {
							thread_cond->notify_all();
						}
					}
					bool is_set()const {
						return flag.load(std::memory_order_relaxed);
					}
					void set_condition_variable(std::condition_variable& cv) {
						std::lock_guard lk(set_clear_mutex);
						thread_cond = &cv;
					}
					void clear_condition_variable() {
						std::lock_guard lk(set_clear_mutex);
						thread_cond = {};
					}
					struct clear_cv_on_destruct {
						~clear_cv_on_destruct() {
							this_thread_interrupt_flag.clear_condition_variable();
						}
					};
				};
				struct thread_interrupted : public std::exception {
					const char* what()const {
						return "thread interrupted";
					}
				};

				inline void interruption_point() {
					if (this_thread_interrupt_flag.is_set()) {
						throw thread_interrupted{};
					}
				}
#if 0
				void interruptible_wait(std::condition_variable& cv, std::unique_lock<std::mutex>& lk) {
					interruption_point();
					this_thread_interrupt_flag.set_condition_variable(cv);
					interrupt_flag::clear_cv_on_destruct guard;
					interruption_point();
					cv.wait_for(lk, std::chrono::milliseconds{ 1 });
					interruption_point();
				}
#else
				template<typename Predicate>
				void interruptible_wait(std::condition_variable& cv,
					std::unique_lock<std::mutex>& lk, Predicate pred)
				{
					interruption_point();
					this_thread_interrupt_flag.set_condition_variable(cv);
					interrupt_flag::clear_cv_on_destruct guard;
					while (!this_thread_interrupt_flag.is_set() && !pred()) {
						cv.wait_for(lk, std::chrono::milliseconds{ 1 });
					}
					interruption_point();
				}
#endif
			}
		}
	}
}

void advanced_thread_management_example();