#pragma once
#include <iostream>
#include <algorithm>
#include <numeric>
#include <thread>
#include <future>
#include <condition_variable>
#include <mutex>
#include <vector>
#include <execution>
#include <map>
#include <format>
#include <unordered_map>
#include <string>

namespace parallel_algorithm {

	// 10.3.1 Examples of using parallel algorithms
	namespace example_parallel_algorithm {

		class X {
			mutable std::mutex mtx;
			int data;
		public:
			X() :data{} {}
			int get_value()const {
				std::lock_guard lk(mtx);
				return data;
			}
			void increment() {
				std::lock_guard lk(mtx);
				++data;
			}
		};

		class Y {
			int data;
		public:
			Y() :data{} {}
			int get_value()const {
				return data;
			}
			void increment() {				
				++data;
			}

		};
		class ProtectedY {
			std::mutex mtx;
			std::vector<Y> v;
		public:
			explicit ProtectedY(std::vector<Y> v)
				:v{ std::move(v) } 
			{
			}
			void lock() {
				mtx.lock();
			}
			void unlock() {
				mtx.lock();
			}
			std::vector<Y>& get_vec() {
				return v;
			}
		};
		namespace list_10_1 {			
			inline void increment_all(std::vector<X>& vx) 
			{
				std::for_each(std::execution::par, vx.begin(),
					vx.end(), [](X& x) { x.increment(); });
			}
		}
		namespace list_10_2 {
			inline void increment_all(ProtectedY& data) {
				std::lock_guard lk(data);
				auto& v = data.get_vec();
				std::for_each(std::execution::par_unseq, v.begin(), v.end(),
					[](Y& y) {y.increment(); });
			}
		}
	}
	// 10.3.2 Counting visits
	namespace counting_visits {
		// Using transform_reduce to count visits to pages of a website
		namespace list_10_3 {
			struct log_info {
				std::string page;
				time_t visit_time;
				std::string brower;
				// any other fields
			};
			extern log_info parse_log_line(std::string const& line);
			using visit_map_type = std::unordered_map<std::string, unsigned long long>;

			inline visit_map_type 
				count_visit_per_page(std::vector<std::string> const& log_lines) {
				struct combine_visits {
					visit_map_type operator()(visit_map_type  lhs, visit_map_type rhs)const {
						if (lhs.size() < rhs.size()) {
							std::swap(lhs, rhs);
						}
						for (auto const& entry : rhs) {
							lhs[entry.first] = entry.second;
						}
						return lhs;
					}

					visit_map_type operator()(log_info log, visit_map_type map)const {
						++map[log.page];
						return map;
					}
					visit_map_type operator()(visit_map_type map, log_info log)const {
						++map[log.page];
						return map;
					}
					visit_map_type operator()(log_info log1, log_info log2)const {
						visit_map_type map;
						++map[log1.page];
						++map[log2.page];
						return map;
					}
				};
				return std::transform_reduce(
					std::execution::par, log_lines.begin(), log_lines.end(),
					visit_map_type(), combine_visits(), parse_log_line); 
			}
		}
	}
}

void parallel_algorithm_example();