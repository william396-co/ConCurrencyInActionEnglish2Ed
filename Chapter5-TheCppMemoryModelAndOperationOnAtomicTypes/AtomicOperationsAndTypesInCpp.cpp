#include "AtomicOperationsAndTypesInCpp.h"

#include <thread>
#include <string>
#include <vector>
#include <mutex>
namespace atomic_operation_and_types_in_cpp 
{
	std::shared_ptr<Widget> p;
	void process_global_data()
	{
		std::shared_ptr<Widget> local = std::atomic_load(&p);
		//process_data(local);
	}

	void update_global_data()
	{
		std::shared_ptr<Widget> local(new Widget);
		std::atomic_store(&p, local);
	}

	namespace sychronizing_op_enforcing_ordering 
	{
		std::atomic<bool> data_ready{};
		std::vector<int> data;
		namespace memory_ordering_for_atomic_op {		

			namespace seq_cst
			{
				std::atomic<bool> x{};
				std::atomic<bool> y{};
				std::atomic<int> z{};
			}
			namespace relaxed {
				namespace list_5_5 {
					std::atomic<bool> x{};
					std::atomic<bool> y{};
					std::atomic<int> z{};
				}
				namespace list_5_6 {
					std::atomic<bool> go{};
					std::atomic<int> x{};
					std::atomic<int> y{};
					std::atomic<int> z{};
					read_values vals1[loop_count];
					read_values vals2[loop_count];
					read_values vals3[loop_count];
					read_values vals4[loop_count];
					read_values vals5[loop_count];
				}
			}
			namespace acquire_release {
				namespace list_5_7 {
					std::atomic<bool> x{};
					std::atomic<bool> y{};
					std::atomic<int> z{};
				}
				namespace list_5_8 {

					std::atomic<bool> x{};
					std::atomic<bool> y{};
					std::atomic<int> z{};
				}
				namespace list_5_9 {
					std::atomic<int> data[5];
#if 0
					std::atomic<bool> sync1{};
					std::atomic<bool> sync2{};
#else
					std::atomic<int> sync{ 0 };
#endif
				}
			}
		}
		namespace release_sequencen_and_synchronize_with
		{
			namespace list_5_11 {
				std::vector<int> queue_data;
				std::atomic<int> count{};
				std::mutex cout_mtx;
			}
		}
	}
}

void atomic_op_types_in_cpp_example()
{
	using namespace atomic_operation_and_types_in_cpp;

	std::cout << __PRETTY_FUNCTION__ << "\n";

	print_lock_free<int>();
	print_lock_free<bool>();
	print_lock_free<void*>();
	print_lock_free<char>();
	print_lock_free<unsigned char>();
	print_lock_free<Widget>();// true
	print_lock_free<BigData>();// false,

	{
		std::atomic<int> x = 2;
		auto rx = x.load();
	}

	{
		// 5.2.2 Operations on std::atomic_flag

		std::atomic_flag f = ATOMIC_FLAG_INIT;
		f.clear(std::memory_order_relaxed);
		auto x = f.test_and_set(std::memory_order_relaxed);
		std::cout << std::boolalpha << x << " f=" << f.test() << "\n";

		SpinLock mtx;
		std::vector<std::jthread> threadVec;
		for (size_t i = 0;i != 2;++i) {
			threadVec.emplace_back([&mtx](auto idx) {
				for (int i = 0;i != 10;++i) {
					std::lock_guard<SpinLock> lock(mtx);
					std::cout << idx << "-[" << std::this_thread::get_id() << "] is running\n";
					std::this_thread::sleep_for(std::chrono::milliseconds{ 42 });					
				}
			},i);
		}
	}


	{// 5.2.3 Operations on std::atomic<bool>
		std::atomic<bool> b{ true };
		b = false;
		b.store(false, std::memory_order_release);
		auto c = b.load(std::memory_order_acquire);

		SpinLockEx mtx;
		std::vector<std::jthread> threadVec;
		for (size_t i = 0;i != 2;++i) {
			threadVec.emplace_back([&mtx](auto idx) {
				for (int i = 0;i != 10;++i) {
					std::lock_guard<SpinLockEx> lock(mtx);
					std::cout << idx << "-[" << std::this_thread::get_id() << "] is running\n";
					std::this_thread::sleep_for(std::chrono::milliseconds{ 42 });
				}
				}, i);
		}
	}

	{// 5.2.4 Operations on std::atomic<T>::pointer arithmetic

		class Foo {};
		Foo some_array[5];
		std::atomic<Foo*> p(some_array);
		auto x = p.fetch_add(2);// add 2 to p return old value
		assert(x == some_array);
		assert(p.load() == &some_array[2]);

		x = (p -= 1);// sub 1 from p and return new value assign to x
		assert(x == &some_array[1]);
		assert(p.load() == &some_array[1]);

		p.fetch_add(3, std::memory_order_release);
	}
	{
		// 5.2.7 Free functions for atomic operations

		print_atomic_is_lock_free(std::atomic<int>{});
		print_atomic_is_lock_free(std::atomic_int{});
		print_atomic_is_lock_free(std::atomic_bool{});
	}

	{
		// 5.3 Synchronzing operations and enforcing ordering
		using namespace sychronizing_op_enforcing_ordering;

		std::vector<std::jthread> ts;
		ts.emplace_back(read_thread);
		ts.emplace_back(write_thread);

		// 5.3.1 The sychronizes-with relationship
		// 5.3.2 The happens-before relationship		

		// 5.3.3 Memory ordering for atomic operations
		{
			using namespace memory_ordering_for_atomic_op;
			/*
			* . sequentially consistent:
			*      		 memory_order_seq_cst
			* . acquire-release:
			*            memory_order_consume, memory_order_acquire,memory_order_release
			* . relaxed:
			*  		     memory_order_relaxed
			*/

			{// SEQUENTAILLY CONSISTENT ORDERING
				using namespace seq_cst;
				std::vector<std::thread> ts;
				ts.emplace_back(read_y_then_x);
				ts.emplace_back(read_x_then_y);
				ts.emplace_back(write_x);
				ts.emplace_back(write_y);
				for (auto& t : ts) {
					t.join();
				}
				assert(z.load() != 0);// z can never fire
				std::cout << "z=" << z << "\n";
			}

			{// RELAXED ORDERING
				using namespace relaxed;
				{
					using namespace list_5_5;
					std::vector<std::thread> ts;
					ts.emplace_back(write_x_then_y);
					ts.emplace_back(read_y_then_x);
					for (auto& t : ts) {
						t.join();
					}
					assert(z.load() != 0);// can fire(z might be 0)
					std::cout << "z=" << z << "\n";
				}
				{
					using namespace list_5_6;
					std::vector<std::thread> ts;
					ts.emplace_back(increment, &x, vals1);
					ts.emplace_back(increment, &y, vals2);
					ts.emplace_back(increment, &z, vals3);
					ts.emplace_back(read_vals, vals4);
					ts.emplace_back(read_vals, vals5);
					go = true;
					for (auto& t : ts) {
						t.join();
					}
					print(vals1);
					print(vals2);
					print(vals3);
					print(vals4);
					print(vals5);
				}
			}


			{// ACQUIRE-RELEASE ORDERING
				using namespace acquire_release;
				{
					using namespace list_5_7;
					std::vector<std::thread> ts;
					ts.emplace_back(read_y_then_x);
					ts.emplace_back(read_x_then_y);
					ts.emplace_back(write_x);
					ts.emplace_back(write_y);
					for (auto& t : ts) {
						t.join();
					}
					assert(z.load() != 0);// z can fire
					std::cout << "z=" << z << "\n";
				}

				{
					using namespace list_5_8;
					std::vector<std::thread> ts;
					ts.emplace_back(write_x_then_y);
					ts.emplace_back(read_y_then_x);
					for (auto& t : ts) {
						t.join();
					}
					assert(z.load() != 0);// can never fire(z might be 0)
					std::cout << "z=" << z << "\n";
				}
				{
					using namespace list_5_9;
					std::vector<std::thread> ts;
					ts.emplace_back(thread1);
					ts.emplace_back(thread2);
					ts.emplace_back(thread3);
					for (auto& t : ts) {
						t.join();
					}
				}
			}
		}

		// 5.3.4 Release sequence and synchroniz-with
		{
			using namespace release_sequencen_and_synchronize_with;
			{
				using namespace list_5_11;
				std::vector<std::jthread> ts;
				ts.emplace_back(populate_queue);
				ts.emplace_back(consume_queue_items);
				ts.emplace_back(consume_queue_items);
				ts.emplace_back(consume_queue_items);
				ts.emplace_back(consume_queue_items);
				ts.emplace_back(consume_queue_items);
			}
		}
	}
}