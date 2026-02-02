#include "hello-coroutine.h"

namespace hello_coroutine {

	void simple_coroutine_example()
	{
		coro h = [](int i) -> coro {
			std::cout << "coro constructor\n";
			co_return;
			}(0);

		h.resume();
	}
	coro hello(int max) {
		std::cout << "hello world\n";
		for (int i = 0; i < max;++i) {
			std::cout << "hello " << i << "\n";
			co_await std::suspend_always{};
		}

		std::cout << "hello end\n";
	}
	void hello_example() {
		auto co = hello(10);
		while (co.resume()) {
			std::cout << "hello coroutine suspend\n";
		}		
	}
	void example_generator() {
		auto gen = fibonacci(10);
		for (int i = 0; gen.resume();++i) {
			std::cout << "Fibonacci(" << i << ")=" << gen.get() << "\n";
		}
	}
	Task<int> async_compute_example()
	{
		std::cout << "wait async compute result...\n";
		//int result = co_await async_compute();
		//std::cout << "result:" << result << "\n";
		co_return 1;
	}
}

void hello_coroutine_example() {
	using namespace hello_coroutine;

#if 0
	example_generator();
	simple_coroutine_example();
	async_compute_example();
#else
	hello_example();
#endif


	
}