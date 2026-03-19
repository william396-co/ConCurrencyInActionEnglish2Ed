#pragma once

#include <iostream>
#include <thread>
#include <atomic>
#include <mutex>

namespace desiging_lockfree_concurrent_data_structures {

	extern std::mutex iom;
	// 7.1 Definitions and consequences
	namespace definition_and_consequence {
		// blocking data structures(use by mutex, condition_variable,future)
		// nonblocking (lock free)
		// 7.1.1 Types of nonblocking data structures
		namespace types_of_nonblocking_data_structures {
			// list 7.1.1 Implementation of a spin-lock mutex using std::atomic_flag

			class spinlock_mutex {
			public:
				spinlock_mutex() :flag{} {}
				void lock() {
					while (flag.test_and_set(std::memory_order_acquire));
				}
				void unlock() {
					flag.clear(std::memory_order_release);
				}
			private:
				std::atomic_flag flag;
			};

			/*
			*  Obstruction-Free:
					If all other threads are paused, then any given thread will complete
					its operation in a bounded number of steps

			   Lock-Free:
					If multiple threads are operating on a data structure,
					then after a bounded number of steps one of them will complete its
					operation

			   Wait-Free:
					Every thread operating on a data structure will complete its operation
					in a bounded number of steps. even if other threads are also operating
					on data structure
			*/
		}

		// 7.1.4 The pros and cons of lock-free data structures
	}

	// 7.2 Examples of lock-free data structures
	namespace example_of_lockfree_data_structure {
		// 7.2.1 Writing a thread-safe stack without locks
		namespace writing_a_threadsafe_stack {
			namespace list_7_2 {
				template<typename T>
				class lock_free_stack {
				public:
					void push(T const& data) {
						node* const new_node = new node(data);
						new_node->next = head.load();
						while (!head.compare_exchange_weak(new_node->next, new_node));
					}
					void pop(T& val) {
						node* old_head = head.load();
						while (!head.compare_exchange_weak(old_head, old_head->next));
						val = old_head->data;
					}
				private:
					struct node {
						T data;
						node* next;
						node(T const& data_) :data{ data_ } {}
						~node() {
							std::cout << "~node(" << data << ")\n";
						}
					};
					std::atomic<node*> head;
				};

				inline void test_free_stack(lock_free_stack<int>& stack, int start, int end) {
					for (int i = start;i != end;++i) {
						stack.push(i);
					}
				}
			}

			// 7.2 A lock-free stack that leaks nodes
			namespace list_7_3 {
				template<typename T>
				class lock_free_stack {
				private:
					struct node {
						std::shared_ptr<T> data;
						node* next;
						node(T const& data_) :data(std::make_shared<T>(data_)) {}
						~node() {
							std::cout << "~node(" << data << ")\n";
						}
					};
					std::atomic<node*> head;
				public:
					void push(T const& data) {
						node* const new_node = new node(data);
						new_node->next = head.load();
						while (!head.compare_exchange_weak(new_node->next, new_node));
					}
					std::shared_ptr<T> pop() {
						node* old_head = head.load();
						while (old_head &&
							!head.compare_exchange_weak(old_head, old_head->next));
						return old_head ? old_head->data : std::shared_ptr<T>{};
					}
				};
				inline void test_free_stack(lock_free_stack<int>& stack, int start, int end) {
					for (int i = start;i != end;++i) {
						stack.push(i);
					}
				}
			}
		}

		// 7.2.2 Stoping those pesky leaks: managing memory in lock-free data structure
		namespace managing_memory_in_lockfree_data_structure {
			// 7.4 Reclaiming nodes when no threads are in pop()
			namespace list_7_4 {
				template<typename T>
				class lock_free_stack {
				private:
					struct node {
						std::shared_ptr<T> data;
						node* next;
						node(T const& data_) :data(std::make_shared<T>(data_)) {}
						~node() {
							std::lock_guard lk(iom);
							std::cout <<"["<<std::this_thread::get_id()<< "] ~node()\n";
						}
					};
					std::atomic<node*> head;
				private:
					std::atomic<unsigned> threads_in_pop{};
					std::atomic<node*> to_be_deleted;
					static void delete_nodes(node* nodes) {
						while (nodes) {
							node* next = nodes->next;
							delete nodes;
							nodes = next;
						}
					}
					void try_reclaim(node* old_head) {
						if (1 == threads_in_pop) {
							node* nodes_to_delete = to_be_deleted.exchange(nullptr);
							if (!--threads_in_pop) {
								delete_nodes(nodes_to_delete);
							}
							else if (nodes_to_delete) {
								chain_pending_nodes(nodes_to_delete);
							}
							delete old_head;
						}
						else {
							chain_pending_node(old_head);
							--threads_in_pop;
						}
					}

					void chain_pending_nodes(node* nodes) {
						node* last = nodes;
						while (node* const next = last->next) {
							last = next;
						}
						chain_pending_nodes(nodes, last);
					}
					void chain_pending_nodes(node* first, node* last) {
						last->next = to_be_deleted;
						while (!to_be_deleted.compare_exchange_weak(last->next, first));
					}
					void chain_pending_node(node* n) {
						chain_pending_nodes(n, n);
					}
				public:
					void push(T const& data) {
						node* const new_node = new node(data);
						new_node->next = head.load();
						while (!head.compare_exchange_weak(new_node->next, new_node));
					}
					std::shared_ptr<T> pop() {
						++threads_in_pop;
						node* old_head = head.load();
						while (old_head &&
							!head.compare_exchange_weak(old_head, old_head->next));
						std::shared_ptr<T> res;
						if (old_head) {
							res.swap(old_head->data);
						}
						try_reclaim(old_head);
						return res;
					}
				};
				inline void test_free_stack(lock_free_stack<int>& stack, int start, int end) {
					for (int i = start;i <= end;++i) {
						stack.push(i);
					}
				}
			}
		}
	}

}

void desiging_lockfree_concurrent_data_structure_example();