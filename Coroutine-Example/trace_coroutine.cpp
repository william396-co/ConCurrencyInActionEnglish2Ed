#include "trace_coroutine.h"

namespace Trace_Coroutine
{

	size_t level = 0;
	std::string IDENT = " ";
}

void co_await_example() 
{
	using namespace Trace_Coroutine;
	std::cout << "Start main()\n";
	//auto a = reply();
	//std::cout << a.get() << "\n";
}

void trace_coroutine_example() {

	using namespace print_coroutine;
	test();
	std::cout << "10. come back to caller because of co_await awaiter\n";
	std::this_thread::sleep_for(std::chrono::seconds{ 1 });
}