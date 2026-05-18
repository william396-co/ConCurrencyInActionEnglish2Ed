#include "TestingAndDebuggingMultithreadedApllication.h"

#include <iostream>
void techniques_for_locating_concurrency_related_bugs_example() {
	using namespace testing_and_debugging_multithreaded_application;

	std::cout << __PRETTY_FUNCTION__ << "\n";
	{
		using namespace structing_multithreaded_test_code;
		{
			using namespace list_11_1;
			test_concurrent_push_and_pop_on_empty_queue();
		}
	}
}
