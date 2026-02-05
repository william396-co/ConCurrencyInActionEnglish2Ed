#pragma once

#include <coroutine>
#include <iterator>
#include <cassert>
#include <type_traits>

#include "manual_lifetime.h"

template<typename Ref,typename Value = std::decay_t<Ref>>
class generator 
{
public:
	class promise_type {
	public:
		promise_type()noexcept {}
		~promise_type()noexcept { clear_value(); }

		void clear_value() {
			if (hasValue_) {
				hasValue_ = false;
				ref_.destruct();
			}
		}

		auto get_return_object() {
			return generator<Ref>{handle_t::from_promise(*this)};
		}
		std::suspend_always initial_suspend() { return {}; }
		std::suspend_always final_suspend()noexcept { return {}; }
		std::suspend_always yield_value(Ref ref)
			noexcept(std::is_nothrow_move_constructible_v<Ref>)
		{
			ref_.construct(std::move(ref));
			hasValue_ = true;
			return {};
		}
		void return_void(){}
		void unhandled_exception() { throw; }
		Ref get() { return ref_.get(); }
	private:
		manual_lifetime<Ref> ref_;
		bool hasValue_{ false };
	};

	using handle_t = std::coroutine_handle<promise_type>;
	generator(generator&& g)noexcept : coro_(std::exchange(g.coro_, {})) {

	}
	~generator() {
		if (coro_) {
			coro_.destroy();
		}
	}

	struct sentinel {};
	class iterator {
	public:
		using reference = Ref;
		using value_type = Value;
		using distance_type = size_t;
		using pointer = std::add_pointer<Ref>;
		using iterator_category = std::input_iterator_tag;

		iterator()noexcept{}
		explicit iterator(handle_t c) :coro_{ c } {
			assert(c);
		}
		reference operator*()const {
			return coro_.promise().get();
		}
		// pre incr
		iterator& operator++() {
			coro_.promise().clear_value();
			coro_.resume();
			return *this;
		}
		// post incr
		void operator++(int) {
			coro_.promise().clear_value();
			coro_.resume();
		}
		bool operator == (sentinel)const noexcept {
			return coro_.done();
		}
		bool operator != (sentinel)const noexcept {
			return !coro_.done();
		}
	private:
		handle_t coro_;
	};

	iterator begin() {
		coro_.resume();
		return iterator{ coro_ };
	}
	sentinel end() {
		return {};
	}

private:

	explicit generator(handle_t h) :coro_{ h } { assert(h); }
	handle_t coro_;
};

inline generator<int> gen_ints(int max) 
{
	for (int i = 0; i != max;++i) {
		co_yield i;
	}
}
generator<uint64_t> gen_fibonacci(unsigned int n);

void generator_test();