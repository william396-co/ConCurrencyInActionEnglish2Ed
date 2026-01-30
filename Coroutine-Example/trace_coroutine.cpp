#include "trace_coroutine.h"

namespace Trace_Coroutine
{

}

void co_await_example() 
{
	using namespace Trace_Coroutine;
	std::cout << "Start main()\n";
	auto a = reply();
	return a.get();
}