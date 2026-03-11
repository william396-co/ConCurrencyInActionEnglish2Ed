#include <thread>
#include <iostream>

#include "../Chapter1-HelloWorldOfConcurrencyInCpp/HelloConcurrencyWorld.h"

#include "../Chapter2-ManagingThreads/BasicThreadManagement.h"

#include "../Chapter3-SharingDataBetweenThreads/SharingDataBetweenThreads.h"

#include "../Chapter4-SynchronizingConcurrentOperation/SychronizingConCurrentOperation.h"

#include "../Chapter5-TheCppMemoryModelAndOperationOnAtomicTypes/MemoryModelBasics.h"
#include "../Chapter5-TheCppMemoryModelAndOperationOnAtomicTypes/AtomicOperationsAndTypesInCpp.h"

#include "../Coroutine-Example/hello-coroutine.h"
#include "../Coroutine-Example/trace_coroutine.h"
#include "../Coroutine-Example/generator.h"

#include "../atm_example/atm.h"


int main() {

#if 0
	// chapter1
	hello_concurrency_world_example();
	// chapter2
	managing_threads_example();
	// chapter3
	sharing_data_between_threads_example();
	// coroutine example
	co_await_example();
	hello_coroutine_example();
	trace_coroutine_example();
	generator_test();

	// chapter4
	synchronzing_concurrent_operation_example();
	// chapter5
	memory_model_basics_example();
	atomic_op_types_in_cpp_example();
#else
	atm_run_example();
#endif

	return 0;
}
