#include "BasicThreadManagement.h"
#include <thread>
#include <iostream>

namespace basic_thread_management 
{
	void do_something() {
		std::cout << "do_something()\n";
	}
	void do_something_else() {
		std::cout << "do_something_else()\n";
	}
	struct background_task {
		void operator()()const {
			do_something();
			do_something_else();
		}
	};


	void launching_a_thread() {
		std::thread my_thread(
			[] (){
				std::cout << "launching a thread\n";
			}
		);
		my_thread.join();
	}

	void run_background_task()
	{
		//background_task t;
		std::jthread f(background_task{});
		std::jthread f2{ background_task() };
		std::jthread f3((background_task()));
	}
}

void basic_thread_management_example() {

	using namespace basic_thread_management;
	launching_a_thread();
	run_background_task();
}