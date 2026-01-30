#pragma once

#include <coroutine>
#include <iostream>
#include <stdexcept>
#include <functional>
#include <chrono>
#include <thread>

namespace hello_coroutine {
		
	struct coro {
		struct promise_type;	
		using handle_type = std::coroutine_handle<promise_type>;

		struct promise_type {
			coro get_return_object(){
				return coro(handle_type::from_promise(*this));
			}
			std::suspend_always initial_suspend() { return {}; }
			std::suspend_always final_suspend()noexcept { return {}; }
			void return_void() {}
			void unhandled_exception() {}
		};
		handle_type handle_;

		explicit coro(handle_type h) :handle_{ h } {}
		~coro() { if (handle_)handle_.destroy(); }
		bool resume() {
			if (!handle_.done()) {
				handle_.resume();
				return !handle_.done();
			}
			return false;
		}
	};

	void simple_coroutine_example();

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

		explicit Genertor(handle_type h):handle_(h){}
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

void hello_coroutine_example();
