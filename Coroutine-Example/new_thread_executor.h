#pragma once

#include <mutex>
#include <thread>
#include <condition_variable>
#include <coroutine>

class new_thread_context {
public:
	new_thread_context(){}
	~new_thread_context() {
		std::unique_lock lock(mtx_);
		cv_.wait(lock, [&]() {return 0 == activeThreadCnt_;});
	}
private:
	class schedule_op {
	public:
		explicit schedule_op(new_thread_context& context)noexcept 
			:context_(context) {
		}

		bool await_ready() { return false; }
		void await_suspend(std::coroutine_handle<> h)noexcept {
			{
				std::lock_guard lock(context_.mtx_);
				++context_.activeThreadCnt_;
			}
			try {

				std::thread([this, h]()mutable {
					// resume the coroutine
					h.resume();

					std::unique_lock lock(context_.mtx_);
					--context_.activeThreadCnt_;
					std::notify_all_at_thread_exit(context_.cv_, std::move(lock));
					}).detach();
			}
			catch (...) {
				std::lock_guard lock(context_.mtx_);
				--context_.activeThreadCnt_;
				throw;			}
		}

		void await_resume(){}

	private:
		new_thread_context& context_;
	};
public:
	struct executor {
	public:
		explicit executor(new_thread_context& context)noexcept
			: context_{ context }
		{
		}
		schedule_op schedule()noexcept {
			return schedule_op{ context_ };
		}
	private:
		new_thread_context& context_;
	};

	executor get_executor() { return executor{ *this }; }
private:
	std::mutex mtx_;
	std::condition_variable cv_;
	std::size_t activeThreadCnt_{};
};