#include <thread>
#include <iostream>

#include "../Chapter1-HelloWorldOfConcurrencyInCpp/HelloConcurrencyWorld.h"

#include "../Chapter2-ManagingThreads/BasicThreadManagement.h"

#include "../Chapter5-TheCppMemoryModelAndOperationOnAtomicTypes/MemoryModelBasics.h"
#include "../Chapter5-TheCppMemoryModelAndOperationOnAtomicTypes/AtomicOperationsAndTypesInCpp.h"

#include "../Coroutine-Example/hello-coroutine.h"
#include "../Coroutine-Example/trace_coroutine.h"


int main() {

#if 0
	hello_concurrency_world_example();
	basic_thread_management_example();
	memory_model_basics_example();
	atomic_op_types_in_cpp_example();
#else
	//co_await_example();
	//hello_coroutine_example();
	trace_coroutine_example();
#endif

	return 0;
}