#include "HelloConcurrencyWorld.h"

#include <iostream>
#include <thread>
void hello_concurrency_world_example() 
{
#if __cplusplus > 201703

	std::jthread t([]() {
		std::cout << "Hello Concurrency World\n";
		});

#else

	std::thread t([]() {
		std::cout << "Hello Concurrency World\n";
		});
	t.join();
#endif


}