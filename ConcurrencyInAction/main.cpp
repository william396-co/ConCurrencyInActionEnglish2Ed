#include <thread>
#include <iostream>

#include "../Chapter1-HelloWorldOfConcurrencyInCpp/HelloConcurrencyWorld.h"

#include "../Chapter2-ManagingThreads/BasicThreadManagement.h"

#include "../Chapter5-TheCppMemoryModelAndOperationOnAtomicTypes/MemoryModelBasics.h"
#include "../Chapter5-TheCppMemoryModelAndOperationOnAtomicTypes/AtomicOperationsAndTypesInCpp.h"


int main() {

#if 0
	hello_concurrency_world_example();
	basic_thread_management_example();
	memory_model_basics_example();
#else
	atomic_op_types_in_cpp_example();
#endif

	return 0;
}