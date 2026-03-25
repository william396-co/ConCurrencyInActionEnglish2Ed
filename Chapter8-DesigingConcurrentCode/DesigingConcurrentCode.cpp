#include "DesigingConcurrentCode.h"

#include <iostream>



void designing_concurrent_code_example() {
	using namespace designing_concurrent_code;

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

		}
	}
}