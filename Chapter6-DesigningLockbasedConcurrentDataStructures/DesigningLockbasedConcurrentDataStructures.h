#pragma once

#include <thread>
#include <mutex>
#include <exception>
#include <stack>
#include <memory>
#include <condition_variable>
#include <queue>
#include <iostream>

namespace designing_lock_based_data_structure {

	extern std::mutex iom;

	// 6.1 What does it mean to design for concurrency
	namespace what_does_it_mean_to_desgin_for_concurrency{

		// 6.1.1 Guidelines for designing data structures for concurrency

	}

	// 6.2 Lock-based concurrent data structures
	namespace lock_based_concurrent_data_structures {

		// 6.2.1 A thread-safe stack using locks

		// list 6.1 A class definition for a thread-safe stack
		namespace list_6_1 {
			struct empty_stack : std::exception {
				const char* what()const throw();
			};

			template<typename T>
			class threadsafe_stack {
			public:
				threadsafe_stack() = default;
				threadsafe_stack(threadsafe_stack const& other)
				{
					std::lock_guard lk(other.mtx);
					data = other.data;
				}
				threadsafe_stack& operator=(threadsafe_stack const&) = delete;
				void push(T new_val) {
					std::lock_guard lk(mtx);
					data.push(new_val);
				}
				std::shared_ptr<T> pop() {
					std::lock_guard lk(mtx);
					if (data.empty()) throw empty_stack{};					
					std::shared_ptr<T> const res(std::make_shared<T>(data.top()));
					data.pop();
					return res;
				}
				void pop(T& value) {
					std::lock_guard lk(mtx);
					if (data.empty()) throw empty_stack{};
					value = std::move(data.top());
					data.pop();
				}
				bool empty()const {
					std::lock_guard lk(mtx);
					return data.empty();
				}
			private:
				std::stack<T> data;
				mutable std::mutex mtx;
			};

			template<typename T>
			void process_stack(threadsafe_stack<T>& stack) {
				for (;;) {
					auto rnd = rand() % 100;
					stack.push(rnd);
					std::lock_guard lk(iom);
					std::cout << "process stack[" << rnd << "]\n";
					std::this_thread::sleep_for(std::chrono::milliseconds{ 3 });
				}
			}
			template<typename T>
			void consume_stack(threadsafe_stack<T>& stack) {
				try {
					for (;;) {
						if (!stack.empty()) {
							auto ret = stack.pop();
							std::lock_guard lk(iom);
							std::cout << "consume stack[" << *ret << "]\n";
						}
					}
				}
				catch (empty_stack const& e) {
					std::lock_guard lk(iom);
					std::cout << e.what() << "\n";
				}
			}
		}

		// 6.2.2 A thread-safe queue using locks and condition variable
		namespace thread_safe_queue_example {

			// list 6.2 The full class definition for a thread-safe queue using condition variables
			namespace list_6_2 {
				template<typename T>
				class threadsafe_queue {
				public:
					threadsafe_queue() = default;
					void push(T new_val) {
						std::lock_guard lk(mtx);
						data.push(std::move(new_val));
						cond.notify_one();
					}
					void wait_and_pop(T& val) {
						std::unique_lock lk(mtx);
						cond.wait(lk, [this]() {return !data.empty();});
						val = std::move(data.front());
						data.pop();
					}
					std::shared_ptr<T> wait_and_pop() {
						std::unique_lock lk(mtx);
						cond.wait(lk, [this] {return !data.empty();});
						auto res = std::make_shared<T>(std::move(data.front()));
						data.pop();
						return res;
					}
					bool try_pop(T& val) {
						std::lock_guard lk(mtx);
						if (data.empty())return false;
						val = std::move(data.front());
						data.pop();
						return true;
					}
					std::shared_ptr<T> try_pop() {
						std::lock_guard lk(mtx);
						if (data.empty())return {};
						auto res = std::make_shared<T>(std::move(data.front()));
						data.pop();
						return res;
					}
					bool empty()const {
						std::lock_guard lk(mtx);
						return data.empty();
					}
				private:
					mutable std::mutex mtx;
					std::queue<T> data;
					std::condition_variable cond;
				};
				template<typename T>
				void process_queue(threadsafe_queue<T>& queue) {
					for (;;) {
						auto rnd = rand() % 100;
						queue.push(rnd);
						std::lock_guard lk(iom);
						std::cout << "process queue[" << rnd << "]\n";
						std::this_thread::sleep_for(std::chrono::milliseconds{ 3 });
					}
				}
				template<typename T>
				void consume_queue(threadsafe_queue<T>& queue) {
					for (;;) {
						auto ret = queue.wait_and_pop();
						std::lock_guard lk(iom);
						std::cout << "consume queue[" << *ret << "]\n";
					}
				}
			}
			// list 6.3 A thread-safe queue holding std::shared_ptr<> Instances
			namespace list_6_3 {

				template<typename T>
				class threadsafe_queue {
				public:
					threadsafe_queue() = default;
					void wait_and_pop(T&val) {
						std::unique_lock lk(mtx);
						cond.wait(lk, [this] {return !data.empty();});
						val  = std::move(*data.front());
					}
					bool try_pop(T& val) {
						std::lock_guard lk(mtx);
						if (data.empty())return false;
						val = std::move(*data.front());
						val = *res;
						return true;
					}
					std::shared_ptr<T> wait_and_pop() {
						std::unique_lock lk(mtx);
						cond.wait(lk, [this] {return !data.empty();});
						auto res = data.front();
						data.pop();
						return res;
					}
					std::shared_ptr<T> try_pop() {
						std::lock_guard lk(mtx);
						if (data.empty())return {};
						auto res = data.front();
						data.pop();
						return res;
					}
					void push(T val) {
						auto res = std::make_shared<T>(val);
						std::lock_guard lk(mtx);
						data.push(res);
						cond.notify_all();
					}
					bool empty()const {
						std::lock_guard lk(mtx);
						return data.empty();
					}
				private:
					mutable std::mutex mtx;
					std::condition_variable cond;
					std::queue<std::shared_ptr<T>> data;
				};
				template<typename T>
				void process_queue(threadsafe_queue<T>& queue) {
					for (;;) {
						auto rnd = rand() % 100;
						queue.push(rnd);
						std::lock_guard lk(iom);
						std::cout << "process queue[" << rnd << "]\n";
						std::this_thread::sleep_for(std::chrono::milliseconds{ 3 });
					}
				}
				template<typename T>
				void consume_queue(threadsafe_queue<T>& queue) {
					for (;;) {
						auto ret = queue.wait_and_pop();
						std::lock_guard lk(iom);
						std::cout << "consume queue[" << *ret << "]\n";
					}
				}
			}

			// list 6.4 A simple single-thread queue implementation
			namespace list_6_4 {
				template<typename T>
				class queue {
				public:
					queue() :tail{} 
					{
					}
					queue(queue const&) = delete;
					queue& operator=(queue const&) = delete;

					std::shared_ptr<T> try_pop() {
						if (!head) {
							return {};
						}
						std::shared_ptr<T> const res(std::make_shared<T>(std::move(head->data));
						std::unique_ptr<node> const old_head = std::move(head);
						head = std::move(old_head->next);
						if (!head)
							tail = nullptr;
						return res;
					}
					void push(T new_val) {
						std::unique_ptr<node> p(new node(std::move(new_val)));
						node* const new_tail = p.get();
						if (tail) {
							tail->next = std::move(p);
						}
						else {
							head = std::move(p);
						}
						tail = new_tail;
					}
				private:
					struct node {
						T data;
						std::unique_ptr<node> next;
						node(T data_) :data(std::move(data_)) 
						{}
					};
					std::unique_ptr<node> head;
					node* tail{};
				};
			}
		}
	}
}

void desiging_lockbased_concurrent_data_structure_example();