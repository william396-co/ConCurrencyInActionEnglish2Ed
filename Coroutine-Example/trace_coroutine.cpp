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