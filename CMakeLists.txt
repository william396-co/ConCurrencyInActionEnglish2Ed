cmake_minimum_required(VERSION 3.20)

project(ConcurrencyInAction LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

find_package(Threads REQUIRED)

set(CONCURRENCY_IN_ACTION_SOURCES
    ConcurrencyInAction/main.cpp
    ATM_example/atm.cpp
    ATM_example/messaging.cpp
    Chapter1-HelloWorldOfConcurrencyInCpp/HelloConcurrencyWorld.cpp
    Chapter2-ManagingThreads/BasicThreadManagement.cpp
    Chapter3-SharingDataBetweenThreads/SharingDataBetweenThreads.cpp
    Chapter4-SynchronizingConcurrentOperation/SychronizingConCurrentOperation.cpp
    Chapter5-TheCppMemoryModelAndOperationOnAtomicTypes/AtomicOperationsAndTypesInCpp.cpp
    Chapter5-TheCppMemoryModelAndOperationOnAtomicTypes/MemoryModelBasics.cpp
    Chapter6-DesigningLockbasedConcurrentDataStructures/DesigningLockbasedConcurrentDataStructures.cpp
    Chapter7-DesigningLockFreeConcurrentDataStructures/DesigningLockFreeConcurrentDataStructures.cpp
    Chapter8-DesigingConcurrentCode/DesigingConcurrentCode.cpp
    Chapter9-AdvancedThreadManagement/AdvancedThreadManagement.cpp
    Chapter10-ParallelAlgorithm/ParallelAlgorithm.cpp
    Chapter11-TechinqueForLocatingConcurrencyrelatedBugs/TestingAndDebuggingMultithreadedApllication.cpp
    Coroutine-Example/example_test.cpp
    Coroutine-Example/hello-coroutine.cpp
    Coroutine-Example/trace_coroutine.cpp
)

set(CONCURRENCY_IN_ACTION_HEADERS
    ATM_example/atm.h
    ATM_example/atm_message.h
    ATM_example/bank.h
    ATM_example/interface_machine.h
    ATM_example/messaging.h
    Chapter1-HelloWorldOfConcurrencyInCpp/HelloConcurrencyWorld.h
    Chapter2-ManagingThreads/BasicThreadManagement.h
    Chapter3-SharingDataBetweenThreads/SharingDataBetweenThreads.h
    Chapter4-SynchronizingConcurrentOperation/SychronizingConCurrentOperation.h
    Chapter5-TheCppMemoryModelAndOperationOnAtomicTypes/AtomicOperationsAndTypesInCpp.h
    Chapter5-TheCppMemoryModelAndOperationOnAtomicTypes/MemoryModelBasics.h
    Chapter6-DesigningLockbasedConcurrentDataStructures/DesigningLockbasedConcurrentDataStructures.h
    Chapter7-DesigningLockFreeConcurrentDataStructures/DesigningLockFreeConcurrentDataStructures.h
    Chapter8-DesigingConcurrentCode/DesigingConcurrentCode.h
    Chapter9-AdvancedThreadManagement/AdvancedThreadManagement.h
    Chapter10-ParallelAlgorithm/ParallelAlgorithm.h
    Chapter11-TechinqueForLocatingConcurrencyrelatedBugs/TestingAndDebuggingMultithreadedApllication.h
    Common/join_thread.h
    Common/threadsafe_queue.h
    Coroutine-Example/generator.h
    Coroutine-Example/hello-coroutine.h
    Coroutine-Example/manual_lifetime.h
    Coroutine-Example/new_thread_executor.h
    Coroutine-Example/sync_wait.h
    Coroutine-Example/task.h
    Coroutine-Example/trace_coroutine.h
)

add_executable(ConcurrencyInAction
    ${CONCURRENCY_IN_ACTION_SOURCES}
    ${CONCURRENCY_IN_ACTION_HEADERS}
)

target_link_libraries(ConcurrencyInAction PRIVATE Threads::Threads)

target_include_directories(ConcurrencyInAction PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/ATM_example
    ${CMAKE_CURRENT_SOURCE_DIR}/Common
    ${CMAKE_CURRENT_SOURCE_DIR}/Coroutine-Example
)

if(MSVC)
    target_compile_options(ConcurrencyInAction PRIVATE /W3 /permissive- /EHsc /utf-8)
    target_compile_definitions(ConcurrencyInAction PRIVATE
        _CRT_SECURE_NO_WARNINGS
        __PRETTY_FUNCTION__=__FUNCSIG__
    )
endif()

source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES
    ${CONCURRENCY_IN_ACTION_SOURCES}
    ${CONCURRENCY_IN_ACTION_HEADERS}
)

set_property(DIRECTORY PROPERTY VS_STARTUP_PROJECT ConcurrencyInAction)
