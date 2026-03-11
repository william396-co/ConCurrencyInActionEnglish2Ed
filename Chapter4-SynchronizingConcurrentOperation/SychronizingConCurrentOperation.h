#pragma once

#include <mutex>
#include <condition_variable>
#include <thread>
#include <queue>
#include <string>
#include <iostream>
#include <memory>
#include <future>
#include <chrono>
#include <utility>
#include <set>
#include <exception>
#include <cmath>
#include <list>
#include <type_traits>


// 4.1 Waiting for an event or other condition
namespace waiting_for_an_event_or_other_condition {

	// 4.1.1 Waiting for a condition with condition variable
	namespace waiting_for_a_condition_with_condition_variable {
		// List 4.1 Waiting for data to process with std::condition_variable
		namespace list_4_1 {
			class data_chunk {
			public:
				explicit data_chunk(std::string const& data)
					:data{ data }
				{
				}
				std::string const& get_content()const { return data; }
			private:
				std::string data;
			};

			extern std::mutex mtx;
			extern std::queue<data_chunk> data_queue;
			extern std::condition_variable data_cond;
			extern bool more_data_to_prepare;
			extern std::mutex cout_mtx;

			inline void process(data_chunk data) {
				std::lock_guard lock(cout_mtx);
				std::cout << "[" << std::this_thread::get_id() << "] " << data.get_content() << "\n";
			}
			inline bool is_last_chunk(data_chunk const& data) {
				return data.get_content() == "zzzzz";
			}

			inline data_chunk prepare_data() {
				std::string ret;
				auto rnd = rand() % 26 + 'a';
				for (int i = 0; i != 5;++i) {
					ret += rnd;
				}
				return data_chunk{ ret };
			}

			inline void data_preparation_thread() {
				while (more_data_to_prepare) {
					data_chunk const data = prepare_data();
					{
						std::lock_guard lk(mtx);
						data_queue.push(data);
					}
					data_cond.notify_all();
				}
			}
			inline void data_processing_thread() {

				while (true) {
					std::unique_lock lk(mtx);
					data_cond.wait(lk, [] {return !data_queue.empty();});
					auto data = data_queue.front();
					data_queue.pop();
					lk.unlock();
					process(data);
					if (is_last_chunk(data)) {
						more_data_to_prepare = false;
						break;
					}
				}
			}
		}
	}

	// 4.1.2 Building a thread-safe queue with condition variables
	namespace building_a_thread_safe_queue{

		// list 4.5 Full class definition of a thread-safe queuq using condition variables
		namespace list_4_5 {
			class data_chunk {
			public:
				data_chunk() = default;
				explicit data_chunk(std::string const& data)
					:data{ data }
				{
				}
				std::string const& get_content()const { return data; }
			private:
				std::string data;
			};
			template<typename T>
			class threadsafe_queue {
			public:
				threadsafe_queue() = default;
				threadsafe_queue(threadsafe_queue const& other) {
					std::lock_guard lk(other.mtx);
					data_queue = other.data_queue;
				}
				void push(T new_val) {
					std::lock_guard lock(mtx);
					data_queue.push(new_val);
					data_cond.notify_all();
				}				
				void wait_and_pop(T& value) {
					std::unique_lock lk(mtx);
					data_cond.wait(lk, [this] {return !data_queue.empty();});
					value = data_queue.front();
					data_queue.pop();
				}
				std::shared_ptr<T> wait_and_pop() {
					std::unique_lock lk(mtx);
					data_cond.wait(lk, [this] {return !data_queue.empty();});
					std::shared_ptr<T> res(std::make_shared<T>(data_queue.front()));
					data_queue.pop();
					return res;
				}
				bool try_pop(T& value) {
					std::lock_guard lk(mtx);
					if (data_queue.empty())
						return false;
					value = data_queue.front();
					data_queue.pop();
					return true;
				}
				std::shared_ptr<T> try_pop() {
					std::lock_guard lk(mtx);
					if (data_queue.empty())
						return std::shared_ptr<T>();
					std::shared_ptr<T> res(std::make_shared<T>(data_queue.front()));
					data_queue.pop();
					return res;
				}
				bool empty()const {
					std::lock_guard  lk(mtx);
					return data_queue.empty();
				}
			private:
				mutable std::mutex mtx;
				std::queue<T> data_queue;
				std::condition_variable data_cond;
			};

			extern threadsafe_queue<data_chunk> data_queue;
			extern std::mutex cout_mtx;
			extern bool more_data_to_prepare;

			inline void process(data_chunk data) {
				std::lock_guard lock(cout_mtx);
				std::cout << "[" << std::this_thread::get_id() << "] " << data.get_content() << "\n";
			}
			inline bool is_last_chunk(data_chunk const& data) {
				return data.get_content() == "zzzzz";
			}

			inline data_chunk prepare_data() {
				std::string ret;
				auto rnd = rand() % 26 + 'a';
				for (int i = 0; i != 5;++i) {
					ret += rnd;
				}
				{
					std::lock_guard lk(cout_mtx);
					std::cout << "[" << std::this_thread::get_id() << "]prepare_data(" << ret << ")\n";
				}
				return data_chunk{ ret };
			}

			inline void data_preparation_thread() {
				while (more_data_to_prepare) {
					data_chunk const data = prepare_data();
					data_queue.push(data);
				}
			}
			inline void data_processing_thread() {

				while (true) {
					data_chunk data;
					data_queue.wait_and_pop(data);
					process(data);
					if (is_last_chunk(data)) {
						more_data_to_prepare = false;
						break;
					}
				}
			}
		}
	}
}

// 4.2 Waiting for one-off events with futures
namespace waiting_for_one_off_events_with_futures{

	// 4.2.1 Returning values from background tasks
	namespace return_value_from_background_task {
		// list 4.6 Using std::future to get the return value of an asynchronous task
		namespace list_4_6 {
			int find_the_answer_to_ltuae();
			void do_other_stuff();
		}

		// List 4.7 Passing arguments to a function with std::async
		namespace list_4_7 {
			struct X {
				void foo(int x, std::string const& str) {
					std::cout << "foo(" << x << "," << str << ")\n";
				}
				std::string bar(std::string const&str) {
					std::cout << "bar(" << str << ")\n";
					return str;
				}
			};
			struct Y {
				double operator()(double d){
					std::cout << "Y operator(" << d << ")\n";
					return d;
				}
			};
			inline X baz(X& x) {
				std::cout << "baz(x)\n";
				return { x };
			}

			class move_only {
			public:
				move_only() {}
				move_only(move_only&&)noexcept {
					std::cout << "move_only move_constructor\n";
				}
				move_only& operator=(move_only&&)noexcept {
					std::cout << "move_only move_assignment\n";
				}
				move_only(move_only const&) = delete;
				move_only& operator=(move_only const&) = delete;
				void operator()(){
					std::cout << "move_only-> operator()\n";
				}
			};
		}
	}

	// 4.2.2 Associating a task with a future
	namespace associating_a_task_with_a_future {
		//PASSING TASKS BETWEEN THREADS
		// list 4.9 Running code on a GUI thread using std::packaged_task
		namespace list_4_9 {
			extern std::deque<std::packaged_task<void()>> tasks;
			extern std::mutex mtx;
			extern bool gui_shutdown_message_recieved;
			void get_and_process_gui_message();

			inline void gui_thread() {
				while (!gui_shutdown_message_recieved) 
				{
					get_and_process_gui_message();
					std::packaged_task<void()> task;
					{
						std::lock_guard lk(mtx);
						if (tasks.empty())
							continue;
						task = std::move(tasks.front());
						tasks.pop_front();
					}

					task();
				}
			}

			template<typename Func>
			std::future<void> post_task_for_gui_thread(Func f) {
				std::packaged_task<void()> task(f);
				std::future<void> res = task.get_future();
				std::lock_guard lk(mtx);
				tasks.push_back(std::move(task));
				return res;
			}
		}
	}

	// 4.2.3 Making (std::)promise
	namespace making_promise {

		namespace list_4_10 {
			struct payload_type;
			struct data_packet {
				int id;
				std::size_t payload;
			};
			struct outgoing_packet {
				std::promise<bool> promise;
				int id;
				std::size_t payload;
			};
			class payload_type {
				std::size_t payload;
			};
#if 0
			class connection {
			public:
				bool has_incoming_data()const {
					return !incoming_data.empty();
				}
				bool has_outgoing_data()const {
					return !outgoing_data.empty();
				}
				data_packet incoming() {
					auto data = incoming_data.front();
					incoming_data.pop();
					return data;
				}
				outgoing_packet top_of_outgoing_queue() {
					auto data = outgoing_data.front();
					outgoing_data.pop();
					return data;
				}
				void send(payload_type payload) {

				}
				std::promise<payload_type> get_promise(int id) {
					std::promise<payload_type> ret;
					auto it = payload_map.find(id);
					if (it != payload_map.end()) {
						return (it->second);
					}
					return ret;
				}
			private:
				std::queue<data_packet> incoming_data;
				std::queue<outgoing_packet> outgoing_data;
				std::unordered_map<int, size_t> payload_map;
			};
			using connection_set = std::set<connection>;

			inline bool done(connection_set const& connections) {
				return false;
			}

			void process_connections(connection_set& connections) {
				while (!done(connections)) {

					for (auto it = connections.begin(); it != connections.end();++it) {
						if (it.has_incoming_data()) {
							data_packet data = it.incoming();
							std::promise<payload_type> p = it.get_promise(data.id);
							p.set_value(data.payload);
						}
						if (it.has_outgoing_data()) {
							outgoing_packet data = it.top_of_outgoing_queue();
							it.send(data.payload);
							data.promise.set_value(true);
						}
					}
				}
			}
#endif
		}

	}

	// 4.2.4 Saving an exception for the future
	namespace saving_an_exception_for_the_future {

		inline double square_root(double x) {
			if (x < 0)
				throw std::out_of_range("x<0");
			return sqrt(x);
		}
	}

	// 4.2.5 Waiting from multiple threads
	namespace waiting_from_multiple_threads {

	}
} 


// 4.3 Waiting with a time limit
namespace waiting_with_a_time_limit {

	// 4.3.1 Clocks
	// 4.3.2 Durations
	namespace duration {
		inline int some_task() {
			std::cout << "[" << std::chrono::system_clock::now() << "]start some task()\n";
			std::this_thread::sleep_for(std::chrono::milliseconds{ 200 });
			return 42;
		}

		inline void do_some_thing_with(int x) {
			auto millisecond = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
			std::cout << "[" << std::chrono::system_clock::now() << "]do something with(" << x << ")\n";
		}
	}
	// 4.3.3 Time points
	namespace time_points
	{
		extern std::condition_variable cv;
		extern std::mutex mtx;
		extern bool done;
		bool wait_loop();
		void set_done();
	}

	// 4.3.4 Functions that accept timeouts
	namespace functions_that_accept_timeouts {

	}
}

// 4.4 Using Synchronization of operations to simplify code
namespace using_synchronization_of_operation_to_simplify_code {

	// 4.4.1 Functional programming with futures
	namespace function_programming_with_futures {

		// list 4.12 A Sequential implementation of Quicksort
		namespace list_4_12 {
			template<typename T>
			std::list<T> sequential_quick_sort(std::list<T> input) {
				if (input.empty())return {};

				std::list<T> result;
				result.splice(result.begin(), input, input.begin());
				T const& pivot = *result.begin();

				auto divide_point = std::partition(input.begin(), input.end(),
					[&](T const& t) {return t < pivot;});

				std::list<T> lower_part;
				lower_part.splice(lower_part.end(), input, input.begin(), divide_point);

				auto new_lower(sequential_quick_sort(std::move(lower_part)));
				auto new_higher(sequential_quick_sort(std::move(input)));

				result.splice(result.end(), new_higher);
				result.splice(result.begin(), new_lower);
				return result;
			}
		}

		// list 4.13 Parallel Quicksor using futures
		namespace list_4_13 {
			template<typename T>
			std::list<T> parallel_quick_sort(std::list<T> input) {
				if (input.empty())return {};

				std::list<T> result;
				result.splice(result.begin(), input, input.begin());
				T const& pivot = *result.begin();

				auto divide_point = std::partition(input.begin(), input.end(),
					[&](T const& t) {return t < pivot;});

				std::list<T> lower_part;
				lower_part.splice(lower_part.end(), input, input.begin(), divide_point);

				std::future<std::list<T>> new_lower(
					std::async(&parallel_quick_sort<T>,std::move(lower_part)));

				auto new_higher(parallel_quick_sort(std::move(input)));

				result.splice(result.end(), new_higher);
				result.splice(result.begin(), new_lower.get());
				return result;
			}
		}

		// list 4.14 A sample implementation of spawn_task
		namespace list_4_14 
		{
#if 0
			template<typename F,typename... Args>
			std::future<std::invoke_result_t<F,Args...>>
				spawn_task(F&& f, Args&&... args) {
				using result_type = std::invoke_result_t<F, Args...>;
				std::packaged_task<result_type(Args&&)...> task(std::move(f));

				std::future<result_type> res(task.get_future());
				std::thread t(std::move(task), std::move(args)...);
				t.detach();
				return res;
			}
#endif
		}
	}

	//Communicating Sequential Process(CSP)
	// 4.4.2 Synchronizing with message passing
	namespace synchronizing_wiht_message_passing {

		// list 4.15 A simple implementation of ATM logic class
		namespace list_4_15 {

		}

	}
}

void synchronzing_concurrent_operation_example();