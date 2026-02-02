#include "hello-coroutine.h"

namespace hello_coroutine {
	namespace Case1 {
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

	namespace Case2 {
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
		void coro_task_example()
		{
			auto co = coro_max(5);
#if 0
			while (co.resume()) {
				std::cout << "get coroutine value:" << co.GetValue() << "\n";
			}
#else
			for (auto const& it : co) {
				std::cout << "get coroutine value:" << it << "\n";
			}
#endif
		}
		void coroGen_example()
		{
			auto task = coroGen(7);
			while (task.resume())
				;
			std::cout << "coroutine end\n";
			for (auto const& val : task.getResult()) {
				std::cout << " " << val;
			}
			std::cout << std::endl;
		}
	}
}

void hello_coroutine_example() {
	using namespace hello_coroutine;
	
	{
		using namespace Case1;
		example_generator();
		async_compute_example();
	}
	{
		using namespace Case2;
		//simple_coroutine_example();
		//hello_example();
		//coro_task_example();
		coroGen_example();
	}
}