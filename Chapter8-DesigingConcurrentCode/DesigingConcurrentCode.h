#pragma once

#include <iostream>
#include <list>
#include <thread>
#include <future>
#include <vector>
#include <atomic>
#include <memory>
#include <chrono>
#include <stack>
#include <numeric>

namespace designing_concurrent_code {

	extern std::mutex iom;

	class join_threads {
	public:
		explicit join_threads(std::vector<std::thread>& threads_) :threads{ threads_ } {

		}
		~join_threads() {
			for (size_t i = 0; i != threads.size();++i) {
				if (threads[i].joinable()) {
					threads[i].join();
				}
			}
		}
	private:
		std::vector<std::thread>& threads;
	};
	// 8.1 Techniques for dividing work between threads
	namespace dividing_work_between_threads {

		// 8.1.1 Dividing data between threads before processing begins

		// 8.1.2 Dividing data recursively
		namespace dividing_data_recursively {

			struct empty_stack : std::exception {
				const char* what()const throw();
			};

			template<typename T>
			class threadsafe_stack {
			public:
				threadsafe_stack() = default;
				threadsafe_stack(threadsafe_stack const& other)
				{
					std::lock_guard lk(other.mtx);
					data = other.data;
				}
				threadsafe_stack& operator=(threadsafe_stack const&) = delete;
				void push(T new_val) {
					std::lock_guard lk(mtx);
					data.push(new_val);
				}
				std::shared_ptr<T> pop() {
					std::lock_guard lk(mtx);
					if (data.empty()) throw empty_stack{};
					std::shared_ptr<T> const res(std::make_shared<T>(data.top()));
					data.pop();
					return res;
				}
				void pop(T& value) {
					std::lock_guard lk(mtx);
					if (data.empty()) throw empty_stack{};
					value = std::move(data.top());
					data.pop();
				}
				bool empty()const {
					std::lock_guard lk(mtx);
					return data.empty();
				}
			private:
				std::stack<T> data;
				mutable std::mutex mtx;
			};

#if 0
			namespace list_8_1 {

				// list 8.1 Parallel Quicksort using a stack of pending chunks to sort
				template<typename T>
				struct sorter {
					struct chunk_to_sort {
						std::list<T> data;
						std::promise<std::list<T>> promise;
					};
					threadsafe_stack<chunk_to_sort> chunks;
					std::vector<std::thread> threads;
					unsigned const max_thread_count;
					std::atomic<bool> end_of_data;

					sorter()
						:max_thread_count(std::thread::hardware_concurrency() - 1),
						end_of_data(false)
					{
					}

					~sorter() {
						end_of_data = true;
						for (auto& t : threads)
							t.join();
					}

					void try_sort_chunk() {
						std::shared_ptr<chunk_to_sort> chunk = chunks.pop();
						if (chunk) {
							sort_chunk(chunk);
						}
					}
					std::list<T> do_sort(std::list<T>& chunk_data)
					{
						if (chunk_data.empty())
							return chunk_data;

						std::list<T> result;
						result.splice(result.begin(), chunk_data, chunk_data.begin());

						T const& partition_val = *result.begin();
						typename std::list<T>::iterator divide_point =
							std::partition(chunk_data.begin(), chunk_data.end(),
								[&](T const& val) {return val < partition_val;});
						chunk_to_sort new_lower_chunk;
						new_lower_chunk.data.splice(new_lower_chunk.data.end(),
							chunk_data, chunk_data.begin(), divide_point);

						std::future<std::list<T>> new_lower =
							new_lower_chunk.promise.get_future();
						chunks.push(std::move(new_lower_chunk));
						if (threads.size() < max_thread_count) {
							threads.push_back(std::thread(&sorter<T>::sort_thread, this));
						}
						std::list<T> new_higher(do_sort(chunk_data));
						result.splice(result.end(), new_higher);
						while (new_lower.wait_for(std::chrono::seconds{ 0 }) != std::future_status::ready) {
							try_sort_chunk();
						}
						result.splice(result.begin(), new_lower.get());
						return result;
					}
					void sort_chunk(std::shared_ptr<chunk_to_sort> const& chunk) {
						chunk->promise.set_value(do_sort(chunk->data));
					}
					void sort_thread() {
						while (!end_of_data) {
							try_sort_chunk();
							std::this_thread::yield();
						}
					}
				};

				template<typename T>
				std::list<T> parallel_quick_sort(std::list<T> input) {
					if (input.empty())
						return input;

					sorter<T> s;
					return s.do_sort(input);
				}
			}
#endif
		}
	}

	// 8.2 Factors affecting the performance of concurrent code
	namespace factors_affecting_the_performance_of_concurrent_code {

		// 8.2.1 How many processors
		// 8.2.2 Data contention and cache ping-pong
		// 8.2.3 False sharing(caches lines)
		// 8.2.4 How close is your data
		// 8.2.5 Oversubscription and excessive task switching
	}
	// 8.3 Designing data structure for multithreaded performance
	namespace designing_data_structure_for_multithreaded_performance {

		// 8.3.1 Dividing array elements for complex operations
		namespace dividing_array_elements_for_complex_operations {

		}
		// Data access patterns in other data structures
	}

	// 8.4 Additional conisderations when designing for concurrency
	namespace additional_consideration_when_designing_for_concurrency {

		// 8.4.1 Exception safety in parallel algorithm
		namespace exception_safety_in_parallel_algorithm {

			
			namespace list_8_2 {
				// 8.2 A navie parallel version of std::accumulate
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

					unsigned long const min_per_thread = 25;
					unsigned long const max_threads =
						(length + min_per_thread - 1) / min_per_thread;
					unsigned long const hardware_threas = std::thread::hardware_concurrency();
					unsigned long const num_threads =
						std::min(hardware_threas != 0 ? hardware_threas : 2, max_threads);
					unsigned long block_size = length / num_threads;
					std::vector<T> results(num_threads);

					std::vector<std::thread> threads(num_threads - 1);
					Iterator block_start = first;
					for (unsigned long i = 0;i < (num_threads - 1);++i) {
						Iterator block_end = block_start;
						std::advance(block_end, block_size);
						threads[i] = std::thread(
							accumulate_block<Iterator, T>(),
							block_start, block_end, std::ref(results[i]));
						block_start = block_end;
					}

					accumulate_block<Iterator, T>()(block_start, last, results[num_threads - 1]);
					std::for_each(threads.begin(), threads.end(), std::mem_fn(&std::thread::join));
					return std::accumulate(results.begin(), results.end(), init);
				}
			}
			namespace list_8_3 {
				// 8.3 A parallel version of std::accumulate using std::packaged_task
				template<typename Iterator, typename T>
				struct accumulate_block {
					T operator()(Iterator first, Iterator last) {
						return std::accumulate(first, last, T{});
					}
				};

				template<typename Iterator, typename T>
				T parallel_accumulate(Iterator first, Iterator last, T init) {

					unsigned long const length = std::distance(first, last);
					if (!length)return init;

					unsigned long const min_per_thread = 25;
					unsigned long const max_threads =
						(length + min_per_thread - 1) / min_per_thread;
					unsigned long const hardware_threas = std::thread::hardware_concurrency();
					unsigned long const num_threads =
						std::min(hardware_threas != 0 ? hardware_threas : 2, max_threads);
					unsigned long block_size = length / num_threads;					

					std::vector<std::future<T>> futures(num_threads - 1);
					std::vector<std::thread> threads(num_threads - 1);
					Iterator block_start = first;
					for (unsigned long i = 0;i < (num_threads - 1);++i) {
						Iterator block_end = block_start;
						std::advance(block_end, block_size);
						std::packaged_task<T(Iterator, Iterator)> task{ accumulate_block<Iterator, T>() };
						futures[i] = task.get_future();
						threads[i] = std::thread(std::move(task),block_start, block_end);
						block_start = block_end;
					}

					T last_result = accumulate_block<Iterator, T>()(block_start, last);								
					std::for_each(threads.begin(), threads.end(), std::mem_fn(&std::thread::join));
					T result = init;
					for (unsigned i = 0; i < num_threads - 1;++i) {
						result += futures[i].get();
					}
					result += last_result;
					return result;
				}
			}
			namespace list_8_4 {
				// list 8.4 An exception-safe parallel version of std::accumulate				
				template<typename Iterator, typename T>
				struct accumulate_block {
					T operator()(Iterator first, Iterator last) {
						return std::accumulate(first, last, T{});
					}
				};

				template<typename Iterator, typename T>
				T parallel_accumulate(Iterator first, Iterator last, T init) {

					unsigned long const length = std::distance(first, last);
					if (!length)return init;

					unsigned long const min_per_thread = 25;
					unsigned long const max_threads =
						(length + min_per_thread - 1) / min_per_thread;
					unsigned long const hardware_threas = std::thread::hardware_concurrency();
					unsigned long const num_threads =
						std::min(hardware_threas != 0 ? hardware_threas : 2, max_threads);
					unsigned long block_size = length / num_threads;

					std::vector<std::future<T>> futures(num_threads - 1);
					std::vector<std::thread> threads(num_threads - 1);
					join_threads joiner(threads);
					Iterator block_start = first;
					for (unsigned long i = 0;i < (num_threads - 1);++i) {
						Iterator block_end = block_start;
						std::advance(block_end, block_size);
						std::packaged_task<T(Iterator, Iterator)> task{ accumulate_block<Iterator, T>() };
						futures[i] = task.get_future();
						threads[i] = std::thread(std::move(task), block_start, block_end);
						block_start = block_end;
					}

					T last_result = accumulate_block<Iterator, T>()(block_start, last);					
					T result = init;
					for (unsigned i = 0; i < num_threads - 1;++i) {
						result += futures[i].get();
					}
					result += last_result;
					return result;
				}
			}
			namespace list_8_5 {
				// list 8.5 An exception-safe parallel version of std::accumulate using std::async
				template<typename Iterator,typename T>
				T parallel_accumulate(Iterator first, Iterator last, T init) {
					unsigned long const length = std::distance(first, last);
					unsigned long const max_chunk_size = 2500000;
					if (length <= max_chunk_size) {
						return std::accumulate(first, last, init);
					}
					else {
						Iterator mid_point = first;
						std::advance(mid_point, length / 2);
						std::future<T> first_half_result
							= std::async(parallel_accumulate<Iterator, T>, first, mid_point, init);
						T second_half_result = parallel_accumulate(mid_point, last, T{});
						return first_half_result.get() + second_half_result;
					}
				}
			}
		}
		// 8.4.2 Scalability and Amdahl's law
		// 8.4.3 Hiding latency with multiple threads
		// 8.4.4 Improving responsiveness with concurrency		
	}
	// 8.5 Designing concurrent code in practice
	namespace designing_concurrent_code_in_practice {

		// 8.5.1 A parallel implementation of std::for_each
		namespace parallel_impl_of_for_each {

			// list 8.7 A parallel version of std::for_each
			namespace list_8_7 {
				template<typename Iterator, typename Func>
				void parallel_for_each(Iterator first, Iterator last, Func f) {

					unsigned long const length = std::distance(first, last);
					if (!length)return;

					unsigned long const min_per_thread = 25;
					unsigned long const max_threads = (length + min_per_thread - 1) / min_per_thread;
					unsigned long const hardware_threads = std::thread::hardware_concurrency();
					unsigned long const num_threads = std::min(hardware_threads != 0 ? hardware_threads : 2, max_threads);

					unsigned long const block_size = length / num_threads;
					std::vector<std::future<void>> futures(num_threads - 1);
					std::vector<std::thread> threads(num_threads - 1);

					Iterator block_start = first;
					for (unsigned i = 0;i < (num_threads - 1);++i) {
						Iterator block_end = block_start;
						std::advance(block_end, block_size);
						std::packaged_task<void()> task{
							[=]() {
								std::for_each(block_start,block_end,f);
							}
						};

						futures[i] = task.get_future();
						threads[i] = std::thread(std::move(task));
						block_start = block_end;
					}

					std::for_each(block_start, last, f);
					for (unsigned i = 0; i < (num_threads - 1);++i)
					{
						futures[i].get();
					}
				}
			}

			// list 8.8 A parallel version of std::for_each using std::async
			namespace list_8_8 {
				template<typename Iterator, typename Func>
				void parallel_for_each(Iterator first, Iterator last, Func f) {
					unsigned long const length = std::distance(first, last);
					if (!length)return;

					unsigned long const min_per_thread = 25;
					if (length < (2 * min_per_thread)) {
						std::for_each(first, last, f);
					}
					else {
						Iterator const mid_point = first + length / 2;
						std::future<void> first_half =
							std::async(&parallel_for_each<Iterator, Func>, first, mid_point, f);
						parallel_for_each(mid_point, last, f);
						first_half.get();
					}
				}
			}
		}

		// 8.5.2 A parallel implementation of std::find
		namespace parallel_impl_of_find {

			// list 8.9 An implementation of a parallel find algorithm
			namespace list_8_9 {
				template<typename Iterator,typename MatchType>
				Iterator parallel_find(Iterator first, Iterator last, MatchType match) {

					struct find_element {
						void operator()(Iterator begin, Iterator end, MatchType match,
							std::promise<Iterator>* result,std::atomic<bool>* done_flag) {
							try
							{
								for (;begin != end && !done_flag->load();++begin) {
									if (*begin == match) {
										result->set_value(begin);
										done_flag->store(true);
										return;
									}
								}
							}
							catch (...)
							{
								try
								{
									result->set_exception(std::current_exception());
									done_flag->store(true);
								}
								catch (...)
								{

								}
							}
						}
					};

					unsigned long const length = std::distance(first, last);
					if (!length)return last;
					
					unsigned long const min_per_thread = 25;
					unsigned long const max_threads = (length + min_per_thread - 1) / min_per_thread;
					unsigned long const hardware_threads = std::thread::hardware_concurrency();
					unsigned long const num_threads = std::min(hardware_threads != 0 ? hardware_threads : 2, max_threads);

					unsigned long const block_size = length / num_threads;
					std::promise<Iterator> result;
					std::atomic<bool> done_flag(false);
					std::vector<std::thread> threads(num_threads - 1); 
					{
						join_threads joiner(threads);
						Iterator block_start = first;
						for (unsigned long i = 0; i != num_threads - 1;++i) {
							Iterator block_end = block_start;
							std::advance(block_end, block_size);
							threads[i] = std::thread(find_element(), block_start, block_end, match, &result, &done_flag);
							block_start = block_end;
						}
						find_element()(block_start, last, match, &result, &done_flag);
						if (!done_flag.load()) {//Not found match
							return last;
						}
						return result.get_future().get();
					}
				}
			}

			// list 8.10 An implementation of a parallel find algorithm using std::async
			namespace list_8_10 {
				template<typename Iterator, typename MatchType>
				Iterator parallel_find_impl(Iterator first, Iterator last, MatchType match, std::atomic<bool>& done) {

					try
					{
						unsigned long const length = std::distance(first, last);
						unsigned long const min_per_thread = 2500000;
						if (length < 2 * min_per_thread) {
							for (;first != last && !done.load();++first) {
								if (*first == match) {
									done = true;
									return first;
								}
							}
							return last;// not found
						}
						else {
							Iterator const mid_point = first + (length / 2);
							std::future<Iterator> async_result =
								std::async(parallel_find_impl<Iterator, MatchType>,	mid_point, last, match, std::ref(done));

								Iterator const direct_result = parallel_find_impl(first, mid_point, match, done);
								// if direct_result equal to mid_point, means not found,
								// try to get result from async_result
								return direct_result == mid_point ? async_result.get() : direct_result;
						}
					}
					catch (...)
					{
						done = true;
						throw;
					}
				}

				template<typename Iterator,typename MatchType>
				Iterator parallel_find(Iterator first, Iterator last, MatchType match) {
					std::atomic<bool> done{ false };
					return parallel_find_impl(first, last, match, done);
				}
			}
		}	
		// 8.5.3 A parallel implementation of std::partial_sum
		namespace parallel_impl_of_partial_sum {

			// list 8.11 Calculating partial sums in parallel by dividing the problem
			namespace list_8_11 {

				template<typename Iterator>
				void parallel_partial_sum(Iterator first, Iterator last) {

					using value_type = typename Iterator::value_type;
					struct process_chunk {

						void operator()(Iterator begin, Iterator last,
							std::future<value_type>* previous_end_value,
							std::promise<value_type>* end_value) {

							try
							{
								Iterator end = last;
								++end;
								std::partial_sum(begin, end, begin);
								if (previous_end_value) {
									const value_type& addend = previous_end_value->get();
									*last += addend;
									if (end_value) {
										end_value->set_value(*last);
									}
									std::for_each(begin, last, [addend](value_type& item) {
										item += addend;
										});
								}
								else if (end_value) {
									end_value->set_value(*last);
								}
							}
							catch (...)
							{
								if (end_value) {
									end_value->set_exception(std::current_exception());
								}
								else {
									throw;
								}
							}
						}
					};

					unsigned long const length = std::distance(first, last);
					if (!length)return;

					unsigned long const min_per_thread = 25;
					unsigned long const max_threads = (length + min_per_thread - 1) / min_per_thread;
					unsigned long const hardware_threads = std::thread::hardware_concurrency();
					unsigned long const num_threads = std::min(hardware_threads != 0 ? hardware_threads : 2, max_threads);
					unsigned long const block_size = length / num_threads;

					std::vector<std::thread> threads(num_threads - 1);
					std::vector<std::promise<value_type>> end_values(num_threads - 1);
					std::vector<std::future<value_type>> previous_end_values;

					join_threads joiner(threads);
					previous_end_values.reserve(num_threads - 1);
					Iterator block_start = first;
					for (unsigned long i = 0; i < num_threads - 1;++i) {
						Iterator block_last = block_start;
						std::advance(block_last, block_size - 1);
						threads[i] = std::thread(process_chunk(), block_start, block_last,
							0 != i ? &previous_end_values[i - 1] : 0, &end_values[i]);
						block_start = block_last;
						++block_start;
						previous_end_values.push_back(end_values[i].get_future());
					}

					Iterator final_element = block_start;
					std::advance(final_element, std::distance(block_start, last) - 1);
					process_chunk()(block_start, final_element, (num_threads > 1) ? &previous_end_values.back() : 0, 0);
				}
			}

		}
	}
}

void designing_concurrent_code_example();
