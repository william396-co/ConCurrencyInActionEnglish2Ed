#include "generator.h"
#include <iostream>


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

generator<uint64_t> gen_icebag(unsigned n) {
	if (0 == n)co_return;
	auto temp = n;
	while (temp != 1) {
		if (0 == (temp % 2)) {// Å¼Êý
			temp /= 2;
			co_yield temp;
		}
		else {
			temp = 3 * temp + 1;
			co_yield temp;
		}
	}
}

void generator_test() 
{
#if 0
	for (auto const& i : gen_ints(10)) {
		std::cout << i << "\n";
	}
	std::cout << "done\n";
#else
	auto co = gen_fibonacci(10);
	int i = 0;
	for(auto const&r:co){
		std::cout << "fib(" << i++ << ")=" << r << "\n";
	}


	auto c2 = gen_icebag(198);
	for (auto const& r : c2) {
		std::cout << r << "\t";
	}

#endif
}