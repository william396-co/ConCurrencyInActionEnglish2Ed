#include <thread>
#include <iostream>

#include "../Chapter1-HelloWorldOfConcurrencyInCpp/HelloConcurrencyWorld.h"
#include "../Chapter2-ManagingThreads/BasicThreadManagement.h"

#include <bitset>

int main() {

#if 0
	hello_concurrency_world_example();
#else
	basic_thread_management_example();
#endif

	return 0;
}