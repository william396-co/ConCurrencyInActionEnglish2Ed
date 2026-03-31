#pragma once

#include <thread>
#include <mutex>
#include <exception>
#include <stack>
#include <memory>
#include <condition_variable>
#include <queue>
#include <iostream>
#include <list>
#include <functional>
#include <shared_mutex>
#include <map>
#include <algorithm>

namespace designing_lock_based_data_structure {

	extern std::mutex iom;

	// 6.1 What does it mean to design for concurrency
	namespace what_does_it_mean_to_desgin_for_concurrency {

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
						std::cout << "[" << std::this_thread::get_id() << "] process queue[" << rnd << "]\n";
						std::this_thread::sleep_for(std::chrono::milliseconds{ 3 });
					}
				}
				template<typename T>
				void consume_queue(threadsafe_queue<T>& queue) {
					for (;;) {
						auto ret = queue.wait_and_pop();
						std::lock_guard lk(iom);
						std::cout << "[" << std::this_thread::get_id() << "] consume queue[" << *ret << "]\n";
					}
				}
			}
			// list 6.3 A thread-safe queue holding std::shared_ptr<> Instances
			namespace list_6_3 {

				template<typename T>
				class threadsafe_queue {
				public:
					threadsafe_queue() = default;
					void wait_and_pop(T& val) {
						std::unique_lock lk(mtx);
						cond.wait(lk, [this] {return !data.empty();});
						val = std::move(*data.front());
					}
					bool try_pop(T& val) {
						std::lock_guard lk(mtx);
						if (data.empty())return false;
						val = std::move(*data.front());
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
						std::cout << "[" << std::this_thread::get_id() << "] process queue[" << rnd << "]\n";
						std::this_thread::sleep_for(std::chrono::milliseconds{ 3 });
					}
				}
				template<typename T>
				void consume_queue(threadsafe_queue<T>& queue) {
					for (;;) {
						auto ret = queue.wait_and_pop();
						std::lock_guard lk(iom);
						std::cout << "[" << std::this_thread::get_id() << "] consume queue[" << *ret << "]\n";
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
						std::shared_ptr<T> const res(std::make_shared<T>(std::move(head->data)));
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
						{
						}
					};
					std::unique_ptr<node> head;
					node* tail{};
				};
			}

			// list 6.5 A simple queue with a dummy node
			namespace list_6_5 {
				template<typename T>
				class queue {
				public:
					queue() :head(new node), tail(head.get()) {}
					queue(queue const&) = delete;
					queue& operator=(queue const&) = delete;
					std::shared_ptr<T> try_pop() {
						if (head.get() == tail) {// same point the dummy node
							return {};
						}
						std::shared_ptr<T> const res(head->data);
						auto old_head = std::move(head);
						head = std::move(old_head->next);
						return res;
					}
					void push(T val) {
						std::shared_ptr<T> new_data(std::make_shared<T>(std::move(val)));
						std::unique_ptr<node> p(new node);// dummy node
						tail->data = new_data;// set data to old dummy node
						node* const new_tail = p.get();
						tail->next = std::move(p);// insert dummy node to tail
						tail = new_tail;// update tail
					}
				private:
					struct node {
						std::shared_ptr<T> data;
						std::unique_ptr<node> next;
					};
					std::unique_ptr<node> head;
					node* tail;
				};
			}

			// list 6.6 A thread-safe queue with fine-grained locking
			namespace list_6_6 {
				template<typename T>
				class threadsafe_queue {
				public:
					threadsafe_queue() :head(new node), tail{ head.get() } {}

					threadsafe_queue(threadsafe_queue const&) = delete;
					threadsafe_queue& operator=(threadsafe_queue const&) = delete;

					std::shared_ptr<T> try_pop() {
						std::unique_ptr<node> old_head = pop_head();
						return old_head ? old_head->data : std::shared_ptr<T>{};
					}
					void push(T val) {
						auto new_data = std::make_shared<T>(std::move(val));
						std::unique_ptr<node> p(new node);
						node* const new_tail = p.get();

						std::lock_guard lk(tail_mtx);
						tail->data = new_data;
						tail->next = std::move(p);
						tail = new_tail;
					}
					bool empty()const {
						std::lock_guard lk(head_mtx);
						return head.get() == get_tail();
					}
				private:
					struct node;
					node* get_tail()const {
						std::lock_guard lk(tail_mtx);
						return tail;
					}
					std::unique_ptr<node> pop_head() {
						std::lock_guard lk(head_mtx);
						if (head.get() == get_tail()) {
							return {};
						}
						std::unique_ptr<node> old_head = std::move(head);
						head = std::move(old_head->next);
						return old_head;
					}
				private:
					struct node {
						std::shared_ptr<T> data;
						std::unique_ptr<node> next;
					};
					mutable std::mutex head_mtx;
					std::unique_ptr<node> head;
					mutable std::mutex tail_mtx;
					node* tail;
				};
				template<typename T>
				void process_queue(threadsafe_queue<T>& queue) {
					for (;;) {
						auto rnd = rand() % 100;
						queue.push(rnd);
						std::lock_guard lk(iom);
						std::cout << "[" << std::this_thread::get_id() << "] process queue[" << rnd << "]\n";
						std::this_thread::sleep_for(std::chrono::milliseconds{ 3 });
					}
				}
				template<typename T>
				void consume_queue(threadsafe_queue<T>& queue) {
					for (;;) {
						if (!queue.empty()) {
							auto ret = queue.try_pop();
							std::lock_guard lk(iom);
							std::cout << "[" << std::this_thread::get_id() << "] consume queue[" << *ret << "]\n";
						}
					}
				}
			}

			// list 6.7 A thread-safe queue with locking and waiting: interals and interface
			namespace list_6_7 {
				template<typename T>
				class threadsafe_queue {
					struct node;
				public:
					threadsafe_queue() :head(new node), tail(head.get()) {}

					threadsafe_queue(threadsafe_queue const&) = delete;
					threadsafe_queue& operator=(threadsafe_queue const&) = delete;

					std::shared_ptr<T> try_pop() {
						std::unique_ptr<node> old_head = try_pop_head();
						return old_head ? old_head->data : std::shared_ptr<T>{};
					}
					bool try_pop(T& val) {
						std::unique_ptr<node> old_head = try_pop_head(val);
						return old_head;
					}
					// wait and pop
					std::shared_ptr<T> wait_and_pop() {
						std::unique_ptr<node> const old_head = wait_pop_head();
						return old_head->data;
					}
					void wait_and_pop(T& val) {
						std::unique_ptr<node> const old_head = wait_pop_head(val);
					}
					// push new values
					void push(T val) {
						std::shared_ptr<T> new_data = std::make_shared<T>(std::move(val));
						std::unique_ptr<node> new_dummy(new node);
						{
							std::lock_guard lk(tail_mtx);
							tail->data = new_data;
							node* const new_tail = new_dummy.get();
							tail->next = std::move(new_dummy);
							tail = new_tail;
						}
						cond.notify_one();
					}
					bool empty()const {
						std::lock_guard lk(head_mtx);
						return head.get() == get_tail();
					}
				private:
					std::unique_ptr<node> try_pop_head() {
						std::lock_guard head_lock(head_mtx);
						if (head.get() == get_tail()) {
							return std::unique_ptr<node>{};
						}
						return pop_head();
					}
					std::unique_ptr<node> try_pop_head(T& val) {
						std::lock_guard head_lock(head_mtx);
						if (head.get() == get_tail()) {
							return std::unique_ptr<node>{};
						}
						val = std::move(*head->data);
						return pop_head();
					}
					std::unique_ptr<node> pop_head() {
						auto old_head = std::move(head);
						head = std::move(old_head->next);
						return old_head;
					}
					const node* get_tail()const {
						std::lock_guard lk(tail_mtx);
						return tail;
					}
					std::unique_lock<std::mutex> wait_for_data() {
						std::unique_lock<std::mutex> head_lock(head_mtx);
						cond.wait(head_lock, [&] { return head.get() != get_tail();});
						return head_lock;
					}
					std::unique_ptr<node> wait_pop_head(T& val) {
						std::unique_lock<std::mutex> head_lock(wait_for_data());
						val = std::move(*head->data);
						return pop_head();
					}
				private:
					struct node {
						std::shared_ptr<T> data;
						std::unique_ptr<node> next;
					};
					std::unique_ptr<node> head;
					node* tail;
					mutable std::mutex head_mtx;
					mutable std::mutex tail_mtx;
					std::condition_variable cond;
				};

				template<typename T>
				void process_queue(threadsafe_queue<T>& queue) {
					for (;;) {
						auto rnd = rand() % 100;
						queue.push(rnd);
						std::lock_guard lk(iom);
						std::cout << "[" << std::this_thread::get_id() << "] process queue[" << rnd << "]\n";
						std::this_thread::sleep_for(std::chrono::milliseconds{ 3 });
					}
				}
				template<typename T>
				void consume_queue(threadsafe_queue<T>& queue) {
					for (;;) {
						if (!queue.empty()) {
							auto ret = queue.try_pop();
							std::lock_guard lk(iom);
							std::cout << "[" << std::this_thread::get_id() << "] consume queue[" << *ret << "]\n";
						}
					}
				}
			}
		}
	}

	// 6.3 Desiging more complex lock-based data structures
	namespace desiging_more_comple_lock_based_data_structures {

		// 6.3.1 Writing a thread-safe lookup table using locks
		// list 6.11 A thread-safe lookup table
		namespace list_6_11 {

			template<typename Key,typename Value, typename Hash=std::hash<Key>>
			class threadsafe_lookup_table {
			private:
				class bucket_type {
				private:
					using bucket_value = std::pair<Key, Value>;
					using bucket_data = std::list<bucket_value>;
					using bucket_iterator = bucket_data::iterator;

					bucket_data data;
					mutable std::shared_mutex mutex;
				public:
					bucket_iterator find_entry_for(Key const& key)const {
						return std::find_if(data.begin(), data.end(),
							[&](bucket_value const& item) {return item.first == key;});
					}
					Value value_for(Key const& key, Value const& default_value)const {
						std::shared_lock lock(mutex);
						bucket_iterator const found_entry = find_entry_for(key);
						return (found_entry == data.end()) ? default_value : found_entry->second;
					}
					void add_or_update_mapping(Key const& key, Value const& value) {
						std::unique_lock lock(mutex);
						bucket_iterator const found_entry = find_entry_for(key);
						if (found_entry == data.end()) {
							data.emplace_back(bucket_value{ key,value });
						}
						else {
							found_entry->second = value;
						}
					}
					void remove_mapping(Key const& key) {
						std::unique_lock lock(mutex);
						bucket_iterator const found_entry = find_entry_for(key);
						if (found_entry != data.end()) {
							data.erase(found_entry);
						}
					}
				};

				std::vector<std::unique_ptr<bucket_type>> buckets;
				Hash hasher;
				bucket_type& get_bucket(Key const& key)const {
					std::size_t const bucket_idx = hasher(key) % buckets.size();
					return *buckets[bucket_idx];
				}
			public:
				using key_type = Key;
				using mapped_type = Value;
				using hash_type = Hash;

				threadsafe_lookup_table(unsigned num_bucket = 19, Hash const& hasher_ = Hash{})
					: buckets(num_bucket), hasher{ hasher_ }
				{
					for (unsigned i = 0; i != num_bucket;++i) {
						buckets[i].reset(new bucket_type);
					}
				}
				threadsafe_lookup_table(threadsafe_lookup_table const&) = delete;
				threadsafe_lookup_table& operator=(threadsafe_lookup_table const&) = delete;
				Value value_for(Key const& key, Value const& default_value = Value{})const
				{
					return get_bucket(key).value_for(key, default_value);
				}
				void add_or_update_mapping(Key const& key, Value const& value) {
					get_bucket(key).add_or_update_mapping(value);
				}
				void remove_mapping(Key const& key) {
					get_bucket(key).remove_mapping(key);
				}
				std::map<Key, Value> get_map()const {
					std::vector<std::unique_lock<std::shared_mutex>> locks;
					for (unsigned i = 0;i != buckets.size();++i) {
						locks.push_back(std::unique_lock<std::shared_mutex>(buckets[i].mutex));
					}
					std::map<Key, Value> res;
					for (unsigned i = 0; i != buckets.size();++i) {
						for (auto it = buckets[i].data.begin(); it != buckets[i].data.end();++it) {
							res.emplace(*it);
						}
					}
					return res;
				}
			};
		}

		// 6.3.2 Writing a thread-safe list using locks
		// list 6.12 A threadsafe list with iterator support
		namespace list_6_12 {

			template<typename T>
			class threadsafe_list {
			private:
				struct node {
					std::mutex mtx;
					std::shared_ptr<T> data;
					std::unique_ptr<node> next;
					node():/* data{std::make_shared<T>()},*/ next() {
						//std::cout << "ctor(" << *data << ")\n";
					}
					explicit node(T const& value)
						:data(std::make_shared<T>(value))
					{
						//std::cout << "ctor(" << value << ")\n";
					}
					~node() {
						if (data) {
						//	std::cout << "dtor(" << *data << ")\n";
						}
					}
				};
				node head;
			public:
				threadsafe_list() = default;
				~threadsafe_list() {
					remove_if([](T const&) {return true;});
				}
				threadsafe_list(threadsafe_list const&) = delete;
				threadsafe_list& operator=(threadsafe_list const&) = delete;

				void push_front(T const& value) {
					std::unique_ptr<node> new_node(new node(value));
					std::lock_guard lk(head.mtx);
					new_node->next = std::move(head.next);
					head.next = std::move(new_node);
				}
				template<typename Function>
				void for_each(Function fn) {
					node* current = &head;
					std::unique_lock lk(head.mtx);
					while (node* const next = current->next.get()) {// start from first node(after dummy head)
						std::unique_lock next_lk(next->mtx);
						lk.unlock();						
						fn(*next->data);
						current = next;
						lk = std::move(next_lk);
					}
				}
				template<typename Pred>
				std::shared_ptr<T> find_first_if(Pred pred) {
					node* current = &head;
					std::unique_lock lk(head.mtx);
					while (node* const next = current->next->get()) {// start from first node(after dummy head)
						std::unique_lock next_lk(next->mtx);
						lk.unlock();
						if (pred(*next->data)) {
							return next->data;
						}
						current = next;
						lk = std::move(next_lk);
					}
					return std::shared_ptr<T>{};
				}

				template<typename Pred>
				void remove_if(Pred pred) {
					node* current = &head;
					std::unique_lock lk(head.mtx);
					while (node* const next = current->next.get()) {// start from first node(after dummy head)
						std::unique_lock next_lk(next->mtx);
						if (pred(*next->data)) {
							std::unique_ptr<node> old_next = std::move(current->next);// take ownership by unique_ptr,then release out of scope
							current->next = std::move(next->next);
							next_lk.unlock();								
						}
						else {
							lk.unlock();
							current = next;
							lk = std::move(next_lk);
						}
					}
				}
			};
		}
	}
}

void desiging_lockbased_concurrent_data_structure_example();