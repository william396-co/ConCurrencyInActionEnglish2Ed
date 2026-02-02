#include "AtomicOperationsAndTypesInCpp.h"


#include <string>
void atomic_op_types_in_cpp_example() 
{
	using namespace atomic_operation_and_types_in_cpp;

	std::cout << __PRETTY_FUNCTION__ << "\n";

	print_lock_free<int>();
	print_lock_free<bool>();
	print_lock_free<void*>();
	print_lock_free<char>();
	print_lock_free<unsigned char>();
	print_lock_free<Widget>();// true
	print_lock_free<BigData>();// false,

	{
		std::atomic<int> x = 2;
		auto rx = x.load();

	}

	{
		// 5.2.2 Operations on std::atomic_flag

		std::atomic_flag f = ATOMIC_FLAG_INIT;
		f.clear(std::memory_order_relaxed);
		auto x = f.test_and_set(std::memory_order_relaxed);
		std::cout << std::boolalpha << x << " f=" << f.test() << "\n";

	}

	{// 5.2.3 Operations on std::atomic<bool>
		std::atomic<bool> b{ true };
		b = false;
		b.store(false, std::memory_order_release);
		auto c = b.load(std::memory_order_acquire);
	}
}