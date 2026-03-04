#pragma once

#include <thread>
#include <iostream>
#include <string>
#include <memory>
#include <vector>
#include <algorithm>
#include <numeric>
#include <cassert>

// 2.1 Basic thread management
namespace basic_thread_management	
{
	class thread_guard {
	public:
		explicit thread_guard(std::thread&t):t{t}{}
		~thread_guard() {
			if (t.joinable()) {
				t.join();
			}
		}
		thread_guard(thread_guard const&) = delete;
		thread_guard& operator=(thread_guard const&) = delete;
	private:
		std::thread& t;
	};

	// 2.1.1 Launching a thread
	namespace launching_a_thread {

		inline void do_something() {
			std::cout << "do_something()\n";
		}
		inline void do_something_else() {
			std::cout << "do_something_else()\n";
		}
		struct background_task {
			void operator()()const {
				do_something();
				do_something_else();
			}
		};
		void launching_a_thread_example();
		void run_background_task();
		//Listing 2.1 A function that returns while a thread still has access
		// to local variable
		namespace list_2_1 {
			inline  void do_something(int x) {
				std::cout << "do something(" << x << ")\n";
			}
			struct func {
			public:
				explicit func(int& i_) :i{ i_ } {}
				void operator()() const{
					for (size_t j = 0;j < 10000;++j) {
						do_something(i);
					}
				}
			private:
				int& i;
			};
			inline void oops() {
				int some_local_state = 0;
				func my_func(some_local_state);
				std::thread t(my_func);
				t.detach();
			}
		}
	}
	// 2.1.2 Waiting for a thread to complete
	// 2.1.3 Waiting In exceptional circumstances
	namespace waiting_in_exceptional_circumstances 
	{
		namespace list_2_2 {
			struct func {
			public:
				func(int i_) :i{ i_ } {}
				void operator()() const{
					std::cout << "func(" << i << ")\n";

				}
			private:
				int i;
			};
			inline void f() {
				int some_local_var =42;
				func my_func(some_local_var);
				std::thread t(my_func);
				try {
					// do something in current thread
					//std::this_thread::sleep_for(std::chrono::milliseconds{ 100 });
#if 0
					int i = 0;
					auto r = 100 / i;
#endif
				}
				catch (...) {
					t.join();
					throw;
				}
				t.join();
			}	
		}
	}
	// 2.1.4 Running threads in the background
	namespace running_threads_in_background
	{
		void back_ground_test();
		namespace list_2_4 {
						
#if 0
			void edit_document(std::string const& filename) {
				open_document_and_display_gui(filename);
				while (!done_editing()) {
					if (cmd.type == open_new_document) {
						std::string const new_name = get_file_from_user();
						std::thread t(edit_document, new_name);
						t.detach();
					}
					else {
						process_user_input(cmd);
					}

				}
			}
#endif
		}

	}
}

// 2.2 Passing argument to a thread function
namespace passing_arg_to_thread_func
{
	void f(int i, std::string const& s);
	void oops(int some_param);

	struct widget {};
	void update_data_for_widget(uint32_t w_id, widget& data);
	void oops_again(uint32_t w_id);

	class X {
	public:
		void do_lengthy_work(int x,std::string const&s);
	};

	void passing_this_with_memfn();

	struct Big_Obj {
	public:
		explicit Big_Obj(std::string const& name) :name{ name } {}
		std::string const& getName()const { return name; }
	private:
		std::string name;
	};
	void process_big_object(std::unique_ptr<Big_Obj>);
	void passing_arg_by_move();
}

// 2.3 Transferring ownership of a thread
namespace transferring_ownership_of_a_thread 
{
	void move_thread_example();
	// Returning a std::thread from a function
	namespace list_2_5 {

		inline void some_func() { std::cout << "some func\n"; }
		inline void some_other_func(int x) {
			std::cout << "some other func(" << x << ")\n";
		}
#if 0
		inline std::thread f() {
			return std::thread(some_func);
		}
		inline std::thread g(){
			std::thread t(some_other_func, 42);
			return t;
		}
#else
		inline void f(std::thread t) {
			t.join();
		}

		inline void g() {
			f(std::thread(some_func));
			std::thread t(some_other_func, 33);
			f(std::move(t));
		}

#endif
	}
	// scoped_thread and example usage
	namespace list_2_6 {
		class scope_thread {
		public:
			explicit scope_thread(std::thread t_):t{std::move(t_)}{
				if (!t.joinable()) {
					throw std::logic_error("No thread");
				}
			}
			~scope_thread() {
				t.join();

			}
			scope_thread(scope_thread const&) = delete;
			scope_thread& operator=(scope_thread const&) = delete;
		private:
			std::thread t;
		};
		struct func {
		public:
			explicit func(int& i) :x{ i } {}
			void operator()()const {
				for (int i = 0;i != 10;++i) {
					std::cout << "func(" << x << ")\n";
				}
			}
		private:
			int& x;
		};
		inline void f() {
			int some_var = 32;
			scope_thread t{ std::thread(func(some_var)) };
			// do something in current thread
			std::this_thread::sleep_for(std::chrono::milliseconds{ 100 });
			std::cout << "finished current thread\n";
		}
	}
	// A joining_thread class
	namespace list_2_7 {
		class joining_thread {
		public:
			joining_thread()noexcept = default;
			template<typename Callable,typename...Args>
			explicit joining_thread(Callable&& func, Args&&...args)
				:t(std::forward<Callable>(func), std::forward<Args>(args)...)
			{}
			explicit joining_thread(std::thread t_)noexcept
				:t(std::move(t_))
			{}

			joining_thread(joining_thread&& other)noexcept:
				t(std::move(other.t)) {

			}
			joining_thread& operator=(joining_thread&& other)noexcept {
				if (joinable())
					join();
				t = std::move(other.t);
				return *this;
			}

			joining_thread& operator=(joining_thread other)noexcept {
				if (joinable())
					join();

				t = std::move(other.t);
				return *this;
			}
			~joining_thread() {
				if (joinable()) {
					join();
				}
			}

			void swap(joining_thread& other)noexcept {
				t.swap(other.t);
			}

			bool joinable()const noexcept {
				return t.joinable();
			}
			void join() {
				t.join();
			}
			void detach() {
				t.detach();
			}
			std::thread& as_thread()noexcept {
				return t;
			}
			const std::thread& as_thread()const noexcept {
				return t;
			}
		private:
			std::thread t;
		};

		inline void do_work(int idx) {
			std::cout << "do_work with joining_thread(" << idx << ")\n";
		}
		inline void f() {
#if 0
			std::vector<std::jthread> ts;
#else
			std::vector<joining_thread> ts;
#endif
			for (int i = 0; i != 5;++i) {
				ts.emplace_back(do_work, i);
			}
		}
	}

	// list 2.8 Spawns some threads and waits for them to finish
	namespace list_2_8{
		inline void do_work(int idx) {
			std::cout << "do_work(" << idx << ")\n";
		}
		inline void f() {
			std::vector<std::thread> ts;
			for (int i = 0; i != 5;++i) {
				ts.emplace_back(do_work,i);
			}
			for (auto& t : ts) {
				t.join();
			}
		}
	}
}

// 2.4 Choosing the number of threads at runtime
namespace chapter_2_4 {

	// List 2.9 A navie parallel version of std::accumulate
	namespace list_2_9 {

		template<typename Iterator, typename T>
		struct accumulate_block {
			void operator()(Iterator first, Iterator last, T& result) {
				result = std::accumulate(first, last, result);
			}
		};

		template<typename Iterator, typename T>
		T parallel_accumulate(Iterator first, Iterator last, T init) {
			size_t const len = std::distance(first, last);
			if (!len)return init;

			size_t const min_per_thread = 25;
			size_t const max_threads = (len + min_per_thread - 1) / min_per_thread;
			size_t const hardware_threads = std::thread::hardware_concurrency();
			size_t const num_threads = std::min(hardware_threads != 0 ? hardware_threads : 2, max_threads);
			size_t const block_size = len / num_threads;
			std::vector<T> results(num_threads);
			std::vector<std::thread> threads(num_threads - 1);
			Iterator block_start = first;
			for (size_t i = 0; i < (num_threads - 1);++i) {
				Iterator block_end = block_start;
				std::advance(block_end, block_size);
#if 0
				threads[i] = std::thread(accumulate_block<Iterator, T>(), block_start, block_end, std::ref(results[i]));
#else
				threads[i] = std::thread([](Iterator first, Iterator last, T& result) {
					result = std::accumulate(first, last, result);},
					block_start, block_end, std::ref(results[i]));
#endif
				block_start = block_end;
			}
#if 0
			accumulate_block<Iterator, T>()(block_start, last, results[num_threads - 1]);
#else
			[](Iterator first, Iterator last, T& result) {
				result = std::accumulate(first, last, result);}(block_start, last, results[num_threads - 1]);
#endif
				for (auto& t : threads)
					t.join();

				return std::accumulate(results.begin(), results.end(), init);
		}

		inline void parallel_accumulate_example() {
			std::vector<int> vi;
			srand(time(nullptr));
			auto ret = 0;
			for (int i = 0; i != 100000;++i) {
				auto tmp = rand() % 10000;
				vi.emplace_back(tmp);
				ret += tmp;
			}
			auto res = parallel_accumulate<>(vi.begin(), vi.end(), 0);
			std::cout << "ret=" << ret << "  vi=" << res << "\n";
			assert(res == ret);
		}
	}
}

// 2.5 Identifying threads
namespace identifying_threads 
{

}

void managing_threads_example();
