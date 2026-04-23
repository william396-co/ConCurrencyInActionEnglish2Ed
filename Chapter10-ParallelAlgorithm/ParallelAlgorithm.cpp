#include "ParallelAlgorithm.h"

namespace parallel_algorithm {
	namespace counting_visits {
		namespace list_10_3 {
			log_info parse_log_line(std::string const& line) {
				return {};
			}
		}
	}
}

void parallel_algorithm_example() {
	using namespace parallel_algorithm;

#if 0
	std::vector<int> my_data;
	std::sort(std::execution::par, my_data.begin(),my_data.end());	


	std::vector<int> vi = { 1,2,3,4,5,6 };
	std::for_each(std::execution::par, vi.begin(), vi.end(), [](auto& x) { ++x;});
	for (auto const& i : vi) {
		std::cout << i << "\t";
	}
	std::cout << "\n";

	int count = vi.size();
	std::for_each(std::execution::par, vi.begin(), vi.end(), [&count](auto& x) {x = ++count;});
	for (auto const& i : vi) {
		std::cout << i << "\t";
	}
	std::cout << "\n";

	std::cout << std::format("the answer is :{}\n", 42);
#endif

	using namespace example_parallel_algorithm;
	{
		{
			using namespace list_10_1;
			std::vector<X> vx(10);
			//increment_all(vx);
			//std::for_each(vx.begin(), vx.end(), [](X const& x) {std::cout << x.get_value() << "\n";});

		}
		{
			using namespace list_10_2;
			std::vector<Y> vy(10);
			ProtectedY data(vy);
			increment_all(data);			
		}
		{
			using namespace counting_visits;
			{
				using namespace list_10_3;

			}
		}

	}
}