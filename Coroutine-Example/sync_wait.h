#pragma once

#include <coroutine>
#include <mutex>
#include <condition_variable>
#include <utility>
#include <cassert>

#include "task.h"

struct sync_wait_task {
	struct promise_type;
	using handle_t = std::coroutine_handle<promise_type>;
	struct promise_type {
		auto get_return_object() {
			return sync_wait_task{ handle_t::from_promise(*this) };
		}
		auto initial_suspend() { return std::suspend_always{}; }
		auto final_suspend()noexcept {
			struct awaiter {
				bool await_ready()noexcept { return false; }
				void await_suspend(handle_t h)noexcept {
					auto& promise = h.promise();
					std::lock_guard lock(promise.mtx_);
					promise.done_ = true;
					promise.cv_.notify_one();
				}
				void await_resume()noexcept{}
			};
			return awaiter{};
		}
		void return_void() {}
		void unhandled_exception() { error_ = std::current_exception(); }
		void wait() {
			std::unique_lock lock(mtx_);
			cv_.wait(lock, [&]() {return done_;});
			if (error_) {
				std::rethrow_exception(error_);
			}
		}
	private:
		std::mutex mtx_;
		std::condition_variable cv_;
		bool done_{ false };
		std::exception_ptr error_;
	};	
	explicit sync_wait_task(auto h)noexcept :coro_{ h } { assert(h); }
	sync_wait_task(sync_wait_task&& other)noexcept : coro_(std::exchange(other.coro_, {})){}
	~sync_wait_task() { if (coro_)coro_.destroy(); }

	void wait() {
		coro_.resume();
		coro_.promise().wait();
	}
private:
	handle_t coro_;
};

inline void sync_wait(task<void>&& t) {
	[&]()->sync_wait_task {
		co_await std::move(t);
	}().wait();
}