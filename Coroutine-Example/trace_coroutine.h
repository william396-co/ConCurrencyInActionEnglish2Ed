#pragma once

#include <coroutine>
#include <iostream>
#include <chrono>
#include <string>
#include <thread>

/*
* https://lewissbaker.github.io/2017/09/25/coroutine-theory
* Lewis Baker C++ 协程提案作者
*/

namespace Trace_Coroutine {

	extern size_t level;
	extern std::string IDENT;

	class Trace {
	public:
		Trace() {
			in_level();
		}
		~Trace() {
			level = -1;
		}	
	private:
		void in_level() {
			level += 1;
			std::string res(IDENT);
			for (size_t i = 0; i != level;++i) {
				res.append(IDENT);
			}
			std::cout << res;
		}
	};

	template<typename T>
	class sync {
		struct promise_type;
		using handle_type = std::coroutine_handle<promise_type>;
		handle_type coro;

		explicit sync(handle_type h) :coro{ h } {
			Trace t;
			std::cout << "Create a sync object\n";
		}
		sync(sync const&) = delete;
		sync(sync&& other)noexcept
			: coro{ other.coro }
		{
			other.coro = nullptr;
		}

		sync& operator=(sync const&) = delete;
		sync& operator=(sync&& other)noexcept {
			if (this != &other) {
				coro = other.coro;
				other.coro = nullptr;
			}
			return *this;
		}

		~sync() {
			Trace t;
			std::cout << "Sync gone\n";
			if (coro) {
				coro.destroy();
			}
		}

		T get() {
			Trace t;
			std::cout << "We got asked for the return value\n";
			return coro.promise().value;
		}

		struct promise_type {
			T value;
			promise_type() {
				Trace t;
				std::cout << "promise created\n";
			}
			~promise_type() {
				Trace t;
				std::cout << "Promised died\n";
			}
			auto get_return_object() {
				Trace t;
				std::cout << "Send back a sync\n";
				return sync<T>(handle_type::from_promise(*this));
			}
			auto initial_suspend() {
				Trace t;
				std::cout << "Started the coroutine, don't stop now!\n";
				return std::suspend_never{};
			}
			auto return_value(T v) {
				Trace t;
				std::cout << "Got an answer of"<<v<<"\n";
				value = v;
				return std::suspend_never{};
			}
			auto final_suspend()noexcept {
				Trace t;
				std::cout << "Finished the coro\n";
				return std::suspend_always{};
			}
			void unhandled_exception() {
				std::exit(1);
			}
		};
	};

	template<typename T>
	struct lazy {
		struct promise_type;
		using handle_type = std::coroutine_handle<promise_type>;
		handle_type coro;

		explicit lazy(handle_type h) :coro{ h } {
			Trace t;
			std::cout << "Created a lazy object\n";
		}
		lazy(lazy const&) = delete;
		lazy(lazy&&other)noexcept
			:coro{other.coro}
		{
			other.coro = nullptr;
		}

		lazy& operator=(lazy const&) = delete;
		lazy& operator=(lazy&& other)noexcept {
			if (this != &other) {
				coro = other.coro;
				other.coro = nullptr;
			}
			return *this;
		}

		~lazy() {
			Trace t;
			std::cout << "lazy gone\n";
			if (coro) {
				coro.destroy();
			}
		}

		T get() {
			Trace t;
			std::cout << "We got asked for the return value\n";
			return coro.promise().value;
		}

		struct promise_type {
			T value;
			promise_type() {
				Trace t;
				std::cout << "promise created\n";
			}
			~promise_type() {
				Trace t;
				std::cout << "Promised died\n";
			}
			auto get_return_object() {
				Trace t;
				std::cout << "Send back a sync\n";
				return lazy<T>{handle_type::from_promise(*this)};
			}
			auto initial_suspend() {
				Trace t;
				std::cout << "Started the coroutine, don't stop now!\n";
				return std::suspend_always{};
			}
			void return_value(T v) {
				Trace t;
				std::cout << "Got an answer of" << v << "\n";
				value = v;
				//return std::suspend_never{};
			}
			auto final_suspend()noexcept {
				Trace t;
				std::cout << "Finished the coro\n";
				return std::suspend_always{};
			}
			void unhandled_exception() {
				std::exit(1);
			}
		};

		bool await_ready() {
			const auto ready = coro.done();
			Trace t;
			std::cout << "Await " << (ready ? "is ready" : "isn't ready") << "\n";
			return coro.done();
		}
		void await_suspend(std::coroutine_handle<> awaiting)
		{
			{
				Trace t;
				std::cout << "About to resume the lazy\n";
				coro.resume();
			}
			Trace t;
			std::cout << "About to resume the awaiter\n";
			awaiting.resume();
		}
		void await_resume() {
			const auto r = coro.promise().value;
			Trace t;
			std::cout << "Await value is returned:" << r << "\n";
			return r;
		}
	};
	inline lazy<std::string> read_data()
	{
		Trace t;
		std::cout << "Reading data..." << std::endl;
		co_return "billion$!";
	}
	inline lazy<std::string> write_data() {

		Trace t;
		std::cout << "Write data...\n";
		co_return "I'm rich";
	}
#if 0
	inline sync<int> reply()
	{
		std::cout << "Started await_answer" << std::endl;
		auto a = co_await read_data();
		auto r = a.get();
		std::cout << "Data we got is " << r << std::endl;
		auto v = co_await write_data();
		auto r2 = v.get();
		std::cout << "write result is " << r2 << std::endl;
		co_return 42;
	}
#endif
}

// https://zhuanlan.zhihu.com/p/497224333
namespace print_coroutine
{
	struct task 
	{
		struct promise_type {
			promise_type() {
				std::cout << "1. create promise type\n";
			}
			~promise_type() {
				std::cout << "16. destructor promise type\n";
			}
			auto get_return_object() {
				std::cout << "2. create coroutine return object,and the coroutine is created now\n";
				return task{ std::coroutine_handle<promise_type>::from_promise(*this) };
			}
			std::suspend_never initial_suspend() { 
				std::cout << "3. do you want suspend the current coroutine?\n";
				std::cout << "4. don't suspend because return std::suspend_never, so continue to execute coroutine body\n";
				return {};
			}
			std::suspend_never final_suspend()noexcept {
				std::cout << "14. coroutine body finished,do you want to suspend the current coroutine?\n";
				std::cout << "15. don't suspend because return std::suspend_never,and the coroutine automatically destroyed,bye\n";
				return {};
			}
			void return_void() {
				std::cout << "13. coroutine don't return value,so return_void is called\n";
			}
			void unhandled_exception() {}
		};
		std::coroutine_handle<promise_type> handle_;
	};

	struct awaitable 
	{
		bool await_ready() {
			std::cout << "6. do you want suspend current coroutine?\n";
			std::cout << "7. yes, suspend because awaiter.await_ready() return false\n";
			return false;
		}
		void await_suspend(std::coroutine_handle<task::promise_type> h) {
			std::cout << "8. execute awaiter.await_suspend()\n";
			std::thread([h]()mutable { h();}).detach();
			std::cout << "9. a new thread launch,and will return back to caller\n";
		}
		void await_resume()noexcept {
			std::cout << "11. await_resume()\n";
		}
	};

	inline task test() {
		std::cout << "5. begin to execute coroutine body,the thread id =" << std::this_thread::get_id() << "\n";
		co_await awaitable{};
		std::cout << "12. coroutine resumed,continue execute coroutine body now,the thread id = " << std::this_thread::get_id() << "\n";
	}

}

void co_await_example();

void trace_coroutine_example();