#include <thread>
#include <iostream>

#include "../Chapter1-HelloWorldOfConcurrencyInCpp/HelloConcurrencyWorld.h"

#include "../Chapter2-ManagingThreads/BasicThreadManagement.h"

#include "../Chapter3-SharingDataBetweenThreads/SharingDataBetweenThreads.h"

#include "../Chapter4-SynchronizingConcurrentOperation/SychronizingConCurrentOperation.h"
#include "../ATM_example/atm.h"

#include "../Chapter5-TheCppMemoryModelAndOperationOnAtomicTypes/MemoryModelBasics.h"
#include "../Chapter5-TheCppMemoryModelAndOperationOnAtomicTypes/AtomicOperationsAndTypesInCpp.h"

#include "../Coroutine-Example/hello-coroutine.h"
#include "../Coroutine-Example/trace_coroutine.h"
#include "../Coroutine-Example/generator.h"

#include "../Chapter6-DesigningLockbasedConcurrentDataStructures/DesigningLockbasedConcurrentDataStructures.h"
#include "../Chapter7-DesigningLockFreeConcurrentDataStructures/DesigningLockFreeConcurrentDataStructures.h"
#include "../Chapter8-DesigingConcurrentCode/DesigingConcurrentCode.h"
#include "../Chpater9-AdvancedThreadManagement/AdvancedThreadManagement.h"

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

	atm_run_example();
	// chapter4
	synchronzing_concurrent_operation_example();
	// chapter5
	memory_model_basics_example();
	atomic_op_types_in_cpp_example();

	// chapter6
	desiging_lockbased_concurrent_data_structure_example();
	// chapter7
	desiging_lockfree_concurrent_data_structure_example();
	// chapter8 
	designing_concurrent_code_example();
#else
	advanced_thread_management_example();
#endif

	return 0;
}
