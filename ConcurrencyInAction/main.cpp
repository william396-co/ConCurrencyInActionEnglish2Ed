#include <thread>
#include <iostream>


int main() {

	std::jthread t([]() {
		std::cout << "Concurrency In Action\n";
		});


	return 0;
}