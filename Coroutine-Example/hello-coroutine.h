#pragma once

#include <coroutine>
#include <iostream>
#include <stdexcept>
#include <functional>
#include <chrono>
#include <thread>
#include <numeric>

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
				std::suspend_always initial_suspend()noexcept { return {}; }
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
}

void hello_coroutine_example();
