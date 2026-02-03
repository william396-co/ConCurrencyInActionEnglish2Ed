#pragma once

#include <coroutine>
//#include <experimental/coroutine>
#include <iostream>
#include <chrono>
#include <string>

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

void co_await_example();