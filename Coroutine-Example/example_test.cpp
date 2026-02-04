#include "generator.h"
#include <iostream>
#include <thread>
#include <chrono>

#include "sync_wait.h"
#include "task.h"
#include "new_thread_executor.h"


generator<uint64_t> gen_fibonacci(unsigned int n)
{
	if (0 == n) co_return;
	if (n > 94)
		throw std::runtime_error("Too big Fibonacci sequence, Elements would overflow");
	co_yield 0;

	if (0 == 1)
		co_return;
	co_yield 1;

	if (2 == n)
		co_return;
	uint64_t a = 0;
	uint64_t b = 1;
	for (size_t i = 2;i != n;++i) {
		uint64_t s = a + b;
		co_yield s;
		a = b;
		b = s;
	}
}

/*
* 冰雹猜想（考拉兹猜想）数列
规则：从一个正整数n开始：如果n是偶数，则下一步变成 n/2；如果n是奇数，则下一步变成 3n+1。

示例（从7开始）：7 → 22 → 11 → 34 → 17 → 52 → 26 → 13 → 40 → 20 → 10 → 5 → 16 → 8 → 4 → 2 → 1 → 4, 2, 1... 循环。

有趣之处：冰雹猜想（未被证明）断言，从任何正整数出发，最终都会落入 4, 2, 1 的循环。数列的轨迹像冰雹在云中上下起伏，最终落到地面（1），其变化完全不可预测，是混沌系统的简单例子。
*/
generator<uint64_t> gen_icebag(unsigned n) {
	if (0 == n)co_return;
	auto temp = n;
	while (temp != 1) {
		if (0 == (temp % 2)) {// 偶数
			temp /= 2;
			co_yield temp;
		}
		else {
			temp = 3 * temp + 1;
			co_yield temp;
		}
	}
}

task<int> compute_meaning_of_life(new_thread_context::executor ex) {
	co_await ex.schedule();
	
	std::this_thread::sleep_for(std::chrono::seconds{ 1 });
	co_return 42;
}
task<void> run(new_thread_context::executor ex) {
	int result = co_await compute_meaning_of_life(ex);
	std::cout << "meaning of life is " << result << "\n";
}



void generator_test() 
{
#if 0
	for (auto const& i : gen_ints(10)) {
		std::cout << i << "\n";
	}
	std::cout << "done\n";
	auto co = gen_fibonacci(10);
	int i = 0;
	for(auto const&r:co){
		std::cout << "fib(" << i++ << ")=" << r << "\n";
	}


	auto c2 = gen_icebag(198);
	for (auto const& r : c2) {
		std::cout << r << "\t";
	}
#else
	new_thread_context context;
	sync_wait(run(context.get_executor()));
#endif
}