#pragma once

#include <thread>
#include <mutex>
#include <condition_variable>
#include <atomic>
#include <future>
#include <iostream>
#include <execution>
#include <shared_mutex>
#include <exception>
#include <stdexcept>
#include <cassert>
#include "../Common/threadsafe_queue.h"

namespace testing_and_debugging_multithreaded_application {

	// 11.1 Types of concurrency-related bugs
	// 11.1.1 Unwanted blocking
	// 11.1.2 Race conditons

	// 11.2 Techniques for locating concurrency-related bugs
	// 11.2.1 Reviewing code to locate potential bugs
	  // QUESTIONS TO THINK ABOUT WHEN  REVIEWING MULTITHREADED CODE
	/*
	* . Which data needs to be protected from concurrent access?
	* . How do you ensure that the data is protected?
	* . Where in the code could other threads be at this time ?
	* . Which mutexes does this thread hold?
	* . Which mutexes might other thread hold?
	* . Are there any ordering requirements between the operation done in
	*   this thread and those done in another? How are those requirements enforeced?
	* . Is the data loaded by this thread still valid? Could it have been modified by
	*   other thread?
	* . If you assume that another thread could be modifying the data, what would 
	*   that mean and how could you ensure that this never happens?
	*/

	// 11.2.2 Locating concurrency-related bugs by testing
	// 11.2.3 Designing for testability
	// 11.2.4 Multithreaded testing techniques
	// BRUTE-FORCE TESTING
	// COMBINATION SIMULATION TESTING(simulates software)
	// DETECTING PROGRAMS EXPOSED BY TESTS WITH A SPECIAL LIBRARY

	// 11.2.5 Structing multithreaded test code
	namespace structing_multithreaded_test_code	{
		// An example test for concurrent push() and pop() calls on a queue
		namespace list_11_1 {

			inline void test_concurrent_push_and_pop_on_empty_queue() {
				threadsafe_queue<int> q;
				std::promise<void> go, push_ready, pop_ready;
				std::shared_future<void> ready(go.get_future());
				std::future<void> push_done;
				std::future<int> pop_done;

				try
				{
					push_done = std::async(std::launch::async,
						[&q, ready, &push_ready]() {
							push_ready.set_value();
							ready.wait();
							q.push(42);
						});

					pop_done = std::async(std::launch::async,
						[&q, ready, &pop_ready]() {
							pop_ready.set_value();
							ready.wait();
							return q.pop();
						});

					push_ready.get_future().wait();
					pop_ready.get_future().wait();
					go.set_value();
					push_done.get();

					assert(pop_done.get() == 42);
					assert(q.empty());
				}
				catch (...)
				{
					go.set_value();
					throw;
				}
			}

		}
	}
	// 11.2.6 Testing the performance of multithreaded code
	
}

void techniques_for_locating_concurrency_related_bugs_example();