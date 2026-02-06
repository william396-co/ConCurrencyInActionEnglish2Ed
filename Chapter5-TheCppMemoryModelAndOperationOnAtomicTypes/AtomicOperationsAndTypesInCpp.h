#pragma once

#include <atomic>
#include <thread>
#include <iostream>
#include <cassert>
#include <memory>
#include <chrono>
#include <algorithm>
#include <functional>
#include <vector>

namespace atomic_operation_and_types_in_cpp {

	struct Widget {
		int x;
		char y;
		double d;
	};
	struct BigData {
		double d;
		double dx;
		int x;
	};

	template<typename T>
	void print_lock_free() {
		std::atomic<T> x{};

		std::cout << "sizeof(" << typeid(T).name() << ")=" << sizeof(T) << "\n";
		if (x.is_lock_free()) {
			std::cout << typeid(T).name() << " is lock free\n";
		}
		else {
			std::cout << typeid(T).name() << " is Not lock free\n";
		}
	}

	template<typename T>
	void print_atomic_is_lock_free(std::atomic<T> const& x) {

		//std::cout << "sizeof(" << typeid(T).name() << ")=" << sizeof(T) << "\n";
		if (std::atomic_is_lock_free(&x)) {
			std::cout << typeid(T).name() << " is lock free\n";
		}
		else {
			std::cout << typeid(T).name() << " is Not lock free\n";
		}
	}

	class SpinLock {
	public:
		SpinLock()
			:f{} {
		}
		void lock() {
			while (f.test_and_set(std::memory_order_acquire))// busy-wait
				;
		}
		void unlock() {
			f.clear(std::memory_order_release);
		}
	private:
		std::atomic_flag f{};
	};

	class SpinLockEx {
	public:
		SpinLockEx()
			:f{} {
		}
		void lock() {		
			while (!f.compare_exchange_weak(expected, true) && !expected) {
				expected = false;
			}
		}
		void unlock() {
			f.store(false, std::memory_order_release);
		}
	private:
		std::atomic<bool> f{};
		bool expected{};
	};

	void process_global_data();
	void update_global_data();

	// 5.3 Synchronzing operations and enforcing ordering
	namespace sychronizing_op_enforcing_ordering
	{
		extern std::atomic<bool> data_ready;
		extern std::vector<int> data;
		inline void read_thread() {
			while (!data_ready) {
				std::this_thread::sleep_for(std::chrono::milliseconds{ 1 });
			}
			std::cout << "The answer=" << data[0] << "\n";
		}
		inline void write_thread() {
			data.emplace_back(42);
			data_ready = true;// data happens-before data_ready(seq_cst, can not be reordered)
		}

		// 5.3.3 Memory ordering for atomic operations
		namespace memory_ordering_for_atomic_op {
			// SEQUENTIALLY CONSISTENT ORDERING
			namespace seq_cst
			{
				extern std::atomic<bool> x;
				extern std::atomic<bool> y;
				extern std::atomic<int> z;

				inline void write_x() {
					x.store(true, std::memory_order_seq_cst);
					std::cout << "write_x\n";
				}
				inline void write_y() {
					y.store(true, std::memory_order_seq_cst);
					std::cout << "write_y\n";
				}
				inline void read_x_then_y() {
					while (!x.load(std::memory_order_seq_cst));
					if (y.load(std::memory_order_seq_cst))
						++z;
				}
				inline void read_y_then_x() {
					while (!y.load(std::memory_order_seq_cst));
					if (x.load(std::memory_order_seq_cst))
						++z;
				}
			}

			// RELAXED ORDERING
			namespace relaxed
			{
				namespace list_5_5
				{
					extern std::atomic<bool> x;
					extern std::atomic<bool> y;
					extern std::atomic<int> z;

					inline void write_x_then_y() {
						x.store(true, std::memory_order_relaxed);//x not happens-before y, 
						y.store(true, std::memory_order_relaxed);//because different varaible can be reordered
					}
					inline void read_y_then_x() {
						while (!y.load(std::memory_order_relaxed));
						if (x.load(std::memory_order_relaxed))
							++z;
					}
				}

				namespace list_5_6 {
					extern std::atomic<bool> go;
					extern std::atomic<int> x;
					extern std::atomic<int> y;
					extern std::atomic<int> z;
					constexpr auto loop_count = 10;
					struct read_values
					{
						int x{};
						int y{};
						int z{};
					};
					extern read_values vals1[];
					extern read_values vals2[];
					extern read_values vals3[];
					extern read_values vals4[];
					extern read_values vals5[];

					inline void increment(std::atomic<int>* var_to_inc, read_values* values) {
						while (!go)
							std::this_thread::yield;
						for (int i = 0; i != loop_count;++i) {
							values[i].x = x.load(std::memory_order_relaxed);
							values[i].y = y.load(std::memory_order_relaxed);
							values[i].z = z.load(std::memory_order_relaxed);
							var_to_inc->store(i + 1, std::memory_order_relaxed);
							std::this_thread::yield;
						}
					}
					inline void read_vals(read_values* values) {
						while (!go)
							std::this_thread::yield;
						for (int i = 0;i != loop_count;++i) {

							values[i].x = x.load(std::memory_order_relaxed);
							values[i].y = y.load(std::memory_order_relaxed);
							values[i].z = z.load(std::memory_order_relaxed);
							std::this_thread::yield;
						}
					}
					inline void print(read_values* v) {
						for (int i = 0;i != loop_count;++i) {
							if (i)
								std::cout << ",";
							std::cout << "(" << v[i].x << "," << v[i].y << "," << v[i].z << ")";
						}
						std::cout << "\n";
					}
				}

			}

			// ACQUIRE-RELEASE ORDERING
			namespace acquire_release {

				namespace list_5_7 {

					extern std::atomic<bool> x;
					extern std::atomic<bool> y;
					extern std::atomic<int> z;

					inline void write_x() {
						x.store(true, std::memory_order_release);
						std::cout << "write_x\n";
					}
					inline void write_y() {
						y.store(true, std::memory_order_release);
						std::cout << "write_y\n";
					}
					inline void read_x_then_y() {
						while (!x.load(std::memory_order_acquire));
						if (y.load(std::memory_order_acquire))
							++z;
					}
					inline void read_y_then_x() {
						while (!y.load(std::memory_order_acquire));
						if (x.load(std::memory_order_acquire))
							++z;
					}
				}
				namespace list_5_8 {

					extern std::atomic<bool> x;
					extern std::atomic<bool> y;
					extern std::atomic<int> z;

					inline void write_x_then_y() {
						x.store(true, std::memory_order_relaxed);
						y.store(true, std::memory_order_release);// x happens-before y, becuase y is release(can not reordered)
					}
					inline void read_y_then_x() {
						while (!y.load(std::memory_order_acquire));
						if (x.load(std::memory_order_relaxed))
							++z;
					}
				}
				namespace list_5_9 {
					// Transitive synchronization using acuqire and release ordering
					extern std::atomic<int> data[];
#if 0
					extern std::atomic<bool> sync1;
					extern std::atomic<bool> sync2;
#else
					extern std::atomic<int> sync;
#endif
					inline void thread1() {
						data[0].store(42, std::memory_order_relaxed);
						data[1].store(27, std::memory_order_relaxed);
						data[2].store(-42, std::memory_order_relaxed);
						data[3].store(1042, std::memory_order_relaxed);
						data[4].store(2004, std::memory_order_relaxed);
#if 0
						sync1.store(true, std::memory_order_release);// data happens-before sync1(set sync1)
#else
						sync.store(1, std::memory_order_release);
#endif
					}
					inline void thread2() {
#if 0
						while (!sync1.load(std::memory_order_acquire));
						sync2.store(true, std::memory_order_release);// sync1 happens-before sync2(set sync2)
#else
						int expected = 1;
						while (!sync.compare_exchange_strong(expected, 2, std::memory_order_acq_rel)) {
							expected = 1;
						}
#endif
					}
					inline void thread3() {
#if 0
						while (!sync2.load(std::memory_order_acquire));//loop until sync2 is set
#else
						while (sync.load(std::memory_order_acquire) < 2);
#endif
						assert(data[0].load(std::memory_order_relaxed) == 42);
						assert(data[1].load(std::memory_order_relaxed) == 27);
						assert(data[2].load(std::memory_order_relaxed) == -42);
						assert(data[3].load(std::memory_order_relaxed) == 1042);
						assert(data[4].load(std::memory_order_relaxed) == 2004);
					}
				}
			}
		}
		// 5.3.4 Release sequences and synchronizes-with
		namespace release_sequencen_and_synchronize_with {
			namespace list_5_11 {

				extern std::vector<int> queue_data;
				extern std::atomic<int> count;
				inline void process(int n) {
					std::cout << "process(" << n << ")\n";
				}
				inline void populate_queue() {
					constexpr auto number_of_items = 20;
					queue_data.clear();
					for (unsigned i = 0; i != number_of_items;++i) {
						queue_data.emplace_back(i);
					}
					count.store(number_of_items, std::memory_order_release);

				}
				inline void consume_queue_items() {
					while (true) {
						int item_idx;
						if ((item_idx = count.fetch_sub(1, std::memory_order_acquire)) <= 0) {
							//wait_for_more_items();
							continue;
						}
						process(queue_data[item_idx - 1]);						
					}
				}
			}
		}
	}
}

void atomic_op_types_in_cpp_example();