#pragma once

#include <coroutine>
#include <iostream>
#include <stdexcept>
#include <functional>
#include <chrono>
#include <thread>
#include <numeric>
#include <cassert>

namespace hello_coroutine {

	namespace Case2
	{
		//https://www.cnblogs.com/RioTian/p/17755013.html
		struct coro {
		public:
			struct promise_type;
			using handle_type = std::coroutine_handle<promise_type>;

		private:
			handle_type handle_;
		public:
			coro(coro const&) = delete;
			coro& operator=(coro const&) = delete;

			explicit coro(handle_type h) :handle_{ h } {}
			~coro() { if (handle_)handle_.destroy(); }
			bool resume() {
				if (!handle_ || handle_.done())
					return false;
				handle_.resume();
				return true;
			}
		public:
			struct promise_type {
				/* data */
				coro get_return_object() {
					return coro(handle_type::from_promise(*this));
				}
#if 1
				std::suspend_always initial_suspend() { return {}; }
#else
				std::suspend_never initial_suspend() { return {}; }
#endif
				std::suspend_always final_suspend()noexcept { return {}; }
				void return_void() {}
				void unhandled_exception() { std::terminate(); }
			};
		};

		void simple_coroutine_example();
		coro hello(int max);
		void hello_example();

		class CoroTask {
		public:
			struct promise_type;
			using CoroHd = std::coroutine_handle<promise_type>;

			explicit CoroTask(CoroHd h) :handle_{ h } {}
			~CoroTask() { if (handle_)handle_.destroy(); }

			CoroTask(CoroTask const&) = delete;
			CoroTask& operator = (CoroTask const&) = delete;

			int GetValue()const { return handle_.promise().value_; }
			bool resume() {
				if (handle_ && !handle_.done()) {
					handle_.resume();
					return true;
				}
				return false;
			}
		private:
			struct iterator {
				CoroHd hd_{};
				explicit iterator(CoroHd hd)
					:hd_{hd}
				{
				}

				void getNext() {
					if (!hd_ || hd_.done()) return;
					hd_.resume();
					if (hd_.done()) { hd_ = nullptr; }
				}

				int operator*()const {
					return hd_.promise().value_;
				}
				iterator operator++() {
					getNext();
					return *this;
				}
				bool operator==(iterator const&)const = default;
			};
		public:
			iterator begin()const {
				if (!handle_ || handle_.done()) {
					return iterator{ nullptr };
				}
				iterator it{ handle_ };
				it.getNext();
				return it;
			}
			iterator end()const {
				return iterator{nullptr};
			}
		public:
			struct promise_type {
				int value_{};

				auto get_return_object() {
					return CoroTask{ CoroHd::from_promise(*this) };
				}

				std::suspend_always initial_suspend() { return {}; }
				std::suspend_always final_suspend()noexcept { return {}; }
				void return_void() {}
				auto yield_value(int value) {
					value_ = value;
					return std::suspend_always{};
				}
				void unhandled_exception() { std::terminate(); }
			};
		private:
			CoroHd handle_;
		};

		inline CoroTask coro_max(int max) {
			std::cout << "coro start max:" << max << "\n";
			for (int i = 0; i <= max;++i) {
				std::cout << "coro index:" << i << "\n";
				co_yield i * i;
			}
			std::cout << "coro end\n";
		}

		void coro_task_example();

		template<typename T>
		class CoGen {
		public:
			struct promise_type;
			using handle_type = std::coroutine_handle<promise_type>;

			explicit CoGen(auto h):handle_{h}{}
			~CoGen() { if (handle_) handle_.destroy(); }

			bool resume() {
				static int i = 0;
				std::cout << " run times：" << ++i << "\n";
				if (!handle_ || handle_.done()) {
					return false;
				}
				handle_.resume();
				return true;
			}
			std::vector<T> getResult() const{
				return handle_.promise().getResult();
			}

		public:
			struct promise_type {
			private:
				std::vector<T> data;
			public:
				auto get_return_object() {
					return CoGen{ handle_type::from_promise(*this) };
				}
				auto initial_suspend() { return std::suspend_always{}; }
				auto final_suspend()noexcept { return std::suspend_always{}; }				
				void unhandled_exception() { std::terminate(); }
				void return_value(std::vector<T> const& d) {
					data = d;
				}
				std::vector<T> const& getResult()const {
					return data;
				}
			};
		private:
			handle_type handle_;
		};

		inline CoGen<int> coroGen(int max) {
			std::cout << __PRETTY_FUNCTION__ << "\n";
			std::vector<int> ret;
			ret.resize(std::abs(max));
			std::iota(ret.begin(), ret.end(), 0);
			for (auto const& v : ret) {
				std::cout << v << "\t";
			}
			std::cout << "\n";
			co_return ret;
		}

		void coroGen_example();
	}

	namespace Case1
	{
		template<typename T>
		struct Genertor {
			struct promise_type;
			using handle_type = std::coroutine_handle<promise_type>;

			struct promise_type {
				T value_;
				std::exception_ptr exception_;

				Genertor get_return_object() {
					return Genertor(handle_type::from_promise(*this));
				}
				std::suspend_always initial_suspend() { return {}; }
				std::suspend_always final_suspend()noexcept { return {}; }
				std::suspend_always yield_value(T value) {
					value_ = value;
					return {};
				}
				void return_void() {}
				void unhandled_exception() { exception_ = std::current_exception(); }
		};

			handle_type handle_;

			explicit Genertor(handle_type h) :handle_(h) {}
			~Genertor() {
				if (handle_) {
					handle_.destroy();
				}
			}

			bool resume() {
				if (!handle_.done()) {
					handle_.resume();
					return !handle_.done();
				}
				return false;
			}

			T get()const {
				return handle_.promise().value_;
			}
		};

		inline Genertor<int> fibonacci(int n) {
			int a = 0, b = 1;
			co_yield a;
			co_yield b;

			for (int i = 2; i != n;++i) {
				int c = a + b;
				a = b;
				b = c;
				co_yield c;
			}
		}
		void example_generator();


		template<typename T>
		struct Task {
			struct promise_type;
			using handle_type = std::coroutine_handle<promise_type>;

			struct promise_type {
				T value_;
				std::exception_ptr exception_;
				std::coroutine_handle<> continuation_;// 继续执行的协成

				Task get_return_object() {
					return Task(handle_type::from_promise(*this));
				}
				std::suspend_always initial_suspend() { return {}; }

				struct final_awaiter {
					bool await_ready()noexcept { return false; }
					void await_suspend(handle_type h)noexcept {
						if (h.promise().continuation_) {
							h.promise().continuation_.resume();
						}
					}
					void await_resume()noexcept {}
				};

				final_awaiter final_suspend()noexcept { return {}; }
				void return_value(T value) {
					value_ = value;
				}
				void unhandled_exception() {
					exception_ = std::current_exception();
				}

				template<typename U>
				void await_transform(U&&) = delete;// 
			};

			handle_type handle_;
			explicit Task(handle_type h) :handle_{ h } {}
			~Task() { if (handle_) handle_.destroy(); }

			struct awaiter {
				handle_type handle_;
				bool await_ready() { return false; }
				handle_type await_suspend(std::coroutine_handle<> cont) {
					handle_.promise().continuation_ = cont;
					return handle_;
				}
				T await_resume() {
					if (handle_.promise().exception_) {
						std::rethrow_exception(handle_.promise().exception_);
					}
					return handle_.promise().value_;
				}
			};

			awaiter operator co_await()
			{
				return awaiter{ handle_ };
			}
		};

		inline Task<int> async_compute()
		{
			std::cout << "start async compute...\n";
			//co_await std::suspend_always{};
			// simulate time cost operation
			std::this_thread::sleep_for(std::chrono::seconds{ 1 });

			std::cout << "finished compute\n";
			co_return 42;
		}
		Task<int> async_compute_example();
	}

	namespace cppref_code
	{
		struct coro {
			struct promise_type;
			using handle_type = std::coroutine_handle<promise_type>;

			struct promise_type
			{
				auto get_return_object() { return coro{ handle_type::from_promise(*this) }; }
#if 1
				std::suspend_always initial_suspend() { std::cout << "initial_suspend always\n"; return {}; }
#else
				std::suspend_never initial_suspend() { std::cout << "initial_suspend never\n"; return {}; }
#endif
				std::suspend_always final_suspend()noexcept { std::cout << "final_suspend\n"; return {}; }
				void return_void() { std::cout << "return_void\n"; }
				void unhandled_exception() {}
			};
			explicit coro(auto h) :handle_{ h } { assert(h); }
			~coro() { if (handle_)handle_.destroy(); }
			bool resume() {
				if (!handle_ || handle_.done())
					return false;
				std::cout << "resume\n";
				handle_.resume();
				return true;
			}
		private:
			handle_type handle_;
		};

		inline void good() {
			coro h = [](int i)->coro {
				std::cout << i << "\n";
				co_return;
				}(42);
			std::cout << "sizof(coro)=" << sizeof(coro) << "\n";
			std::cout << "sizof(coro::handle_type)=" << sizeof(coro::handle_type) << "\n";
			h.resume();
			std::cout << "==================\n";
		}

		struct tunable_coro 
		{
			// A awaiter whose "readiness" is determined via constructor's parameter
			struct tunable_awaiter {
			public:
				explicit tunable_awaiter(bool ready):ready_{ready}{ }
				// Three standard awaiter interface functions:
				bool await_ready() { return ready_; }
				void await_suspend(std::coroutine_handle<>)noexcept {}
				void await_resume()noexcept {}
			private:
				bool ready_{};
			};
			struct promise_type {
				using handle_type = std::coroutine_handle<promise_type>;
			public:
				auto get_return_object() {
					return tunable_coro{ handle_type::from_promise(*this) };
				}
				auto initial_suspend() { return std::suspend_always{}; }
				auto final_suspend()noexcept { return std::suspend_always{}; }
				void return_void(){}
				void unhandled_exception() { std::terminate(); }
				// A user provided transforming function which returns the custom awaiter:
				auto await_transform(std::suspend_always) {
					return tunable_awaiter(!ready_);
				}
				void disable_suspension() { ready_ = false; }
			private:
				bool ready_{true};
			};
		public:
			explicit tunable_coro(auto h) :handle_{ h } { assert(h); }
			~tunable_coro() { if (handle_)handle_.destroy(); }

			tunable_coro(tunable_coro const&) = delete;
			tunable_coro& operator=(tunable_coro const&) = delete;
			tunable_coro(tunable_coro&&) = delete;
			tunable_coro& operator=(tunable_coro&&) = delete;

			void disable_suspension()const {
				if (handle_.done())
					return;
				handle_.promise().disable_suspension();
				//handle_();
			}
			bool operator()()const {
				if (!handle_.done())
					handle_();
				return !handle_.done();
			}
		private:
			promise_type::handle_type handle_;
		};
		inline tunable_coro generate(int n)
		{
			for (int i{}; i != n; ++i)
			{
				std::cout << i << ' ';
				// The awaiter passed to co_await goes to promise_type::await_transform which
				// issues tunable_awaiter that initially causes suspension (returning back to
				// main at each iteration), but after a call to disable_suspension no suspension
				// happens and the loop runs to its end without returning to main().
				co_await std::suspend_always{};
			}
		}

		inline auto switch_2_new_thread(std::jthread& out) {
			struct awaitable {
			private:
				std::jthread* p_out;
			public:
				explicit awaitable(std::jthread* j) :p_out{ j } {}
				~awaitable() { p_out = nullptr; }

				bool await_ready() { std::cout << "await_ready\n"; return false; }
				void await_suspend(std::coroutine_handle<> h) {
					std::jthread& out = *p_out;
					if (out.joinable())
						throw std::runtime_error("Output jthread parameter not empty");
					out = std::jthread([h]() {
						std::cout <<"["<<std::this_thread::get_id()<<"]: h.resume\n";
						h.resume();
						});
					std::cout << "New threadID:" << out.get_id() << "\n";
				}
				void await_resume()noexcept { std::cout << "await_resume\n"; }
			};
			return awaitable{ &out };
		}
		struct Task {
			struct promise_type;
			using handle_type = std::coroutine_handle<promise_type>;

			struct promise_type 
			{
				Task get_return_object() { std::cout << "get_return_object\n"; return {}; }
				std::suspend_never initial_suspend() { std::cout << "initial_suspend\n";  return {}; }
				std::suspend_never final_suspend()noexcept { std::cout << "final_suspend\n";  return {}; }
				void return_void() { std::cout << "return_void\n"; }
				void unhandled_exception() { std::cout << "unhandled_exception\n"; }
				// custom non-throwing overload of new
				void* operator new(std::size_t n) noexcept
				{
					std::cout << "size=" << n << "\n";//72
					if (void* mem = std::malloc(n))
						return mem;
					return nullptr; // allocation failure
				}
			};
		};
		inline Task resuming_on_new_thread(std::jthread& out) {
			std::cout << "Coroutine started on thread:" << std::this_thread::get_id() << "\n";
			co_await switch_2_new_thread(out);
			std::cout<<"Coroutine resumed on thread:"<<std::this_thread::get_id()<<"\n";
			std::cout << "==================\n";
		}
	}
}

void hello_coroutine_example();
