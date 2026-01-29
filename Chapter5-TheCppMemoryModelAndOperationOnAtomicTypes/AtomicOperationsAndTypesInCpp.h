#pragma once

#include <atomic>
#include <thread>
#include <iostream>


namespace atomic_operation_and_types_in_cpp {

	struct Widget {
		int x;
		char y;
		double d;
	};
	struct BigData {
		double d;
		double dx;
		int x;
	};

	template<typename T>
	void print_lock_free() {
		std::atomic<T> x;

		std::cout << "sizeof(" << typeid(T).name() << ")=" << sizeof(T) << "\n";
		if (x.is_lock_free()) {
			std::cout << typeid(T).name() << " is lock free\n";
		}
		else {
			std::cout << typeid(T).name() << " is Not lock free\n";
		}
	}

	class SpinLock {
	public:
		SpinLock()
			:f{} {
		}
		void lock() {
			while (!f.test_and_set(std::memory_order_acquire))
				;
		}
		void unlock() {
			f.clear(std::memory_order_release);
		}
	private:
		std::atomic_flag f{};
	};
}

void atomic_op_types_in_cpp_example();