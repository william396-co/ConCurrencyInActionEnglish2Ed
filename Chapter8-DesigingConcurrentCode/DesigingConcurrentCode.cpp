#include "DesigingConcurrentCode.h"

#include <iostream>



void designing_concurrent_code_example() {
	using namespace designing_concurrent_code;
	std::mutex iom;
	{
		// 8.1 Techniques for dividing work between threads
		using namespace dividing_work_between_threads;

		{
			using namespace dividing_data_recursively;

			{
#if 0
				using namespace list_8_1;

				std::list<int> li = { 2,4,3,56,8,100,200 };
				auto res = parallel_quick_sort(li);
				for (auto const& r : res) {
					std::cout << r << "\t";
				}
				std::cout << "\n";
#endif
			}
		}
		

		// 8.2 Factors affecting the performance of concurrent code
		{
			using namespace factors_affecting_the_performance_of_concurrent_code;
		}

		// 8.3 Designing data structure for multithreaded performance
		{
			using namespace designing_data_structure_for_multithreaded_performance;
			{
				using namespace dividing_array_elements_for_complex_operations;

			}
		}


		// 8.4 Additional conisderations when designing for concurrency
		{
			using namespace additional_consideration_when_designing_for_concurrency;
			{
				using namespace exception_safety_in_parallel_algorithm;

				std::vector<size_t> vi;
				constexpr auto max_size = 100000000;
				vi.reserve(max_size);
				for (unsigned long i = 0; i != max_size;++i) {
					vi.emplace_back(i * i);
				}
				{
					using namespace list_8_2;
					std::cout << parallel_accumulate(vi.begin(), vi.end(), (size_t)0) << "\n";
				}
				{
					using namespace list_8_3;
					std::cout << parallel_accumulate(vi.begin(), vi.end(), (size_t)0) << "\n";
				}
				{
					using namespace list_8_4;
					std::cout << parallel_accumulate(vi.begin(), vi.end(), (size_t)0) << "\n";
				}
				{
					using namespace list_8_5;
					std::cout << parallel_accumulate(vi.begin(), vi.end(), (size_t)0) << "\n";
				}
			}
		}

		// 8.5 Designing concurrent code in practice
		{			
			using namespace designing_concurrent_code_in_practice;

			std::vector<size_t> vi;
			constexpr auto max_size = 100000000;
			vi.reserve(max_size);
			for (unsigned long i = 0; i != max_size;++i) {
				vi.emplace_back(i * i);
			}
			// 8.5.1 A parallel implementation of std::for_each
			{
				using namespace parallel_impl_of_for_each;
				{
					using namespace list_8_7;
#if 0
					parallel_for_each(vi.begin(), vi.end(),
						[&iom](auto item) {
							std::lock_guard lk(iom);
							std::cout << "[" << std::this_thread::get_id() << "] " << item << "\n";
						});
#endif
				}
				{
					using namespace list_8_8;

#if 0
					parallel_for_each(vi.begin(), vi.end(),
						[&iom](auto item) {
							std::lock_guard lk(iom);
							std::cout << "[" << std::this_thread::get_id() << "] " << item << "\n";
						});
#endif
				}
			}

			// 8.5.2 A parallel implementation of std::find
			{
				using namespace parallel_impl_of_find;

				{
					using namespace list_8_9;
					auto it = parallel_find(vi.begin(), vi.end(), 1024 * 1024);
					if (it != vi.end()) {
						std::cout << "Found :" << *it << "\n";
					}
					else {
						std::cout << "not found\n";
					}
				}
				{
					using namespace list_8_10;
					auto it = parallel_find(vi.begin(), vi.end(), 4096 * 4096);
					if (it != vi.end()) {
						std::cout << "Found :" << *it << "\n";
					}
					else {
						std::cout << "not found\n";
					}
				}
			}

			// 8.5.3 A parallel implementation of std::partial_sum
			{
				using namespace parallel_impl_of_partial_sum;
				{
					using namespace list_8_11;
#if 0
					parallel_partial_sum(vi.begin(), vi.end());
					for (auto const& i : vi) {
						std::cout << i << "\t";
					}
					std::cout << "\n";
#endif
				}
				{
					using namespace list_8_13;
					std::vector<size_t> vi = { 1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17};
					parallel_partial_sum(vi.begin(), vi.end());
					for (auto const& i : vi) {
						std::cout << i << "\t";
					}
					std::cout << "\n";
				}
			}
		}
	}
}