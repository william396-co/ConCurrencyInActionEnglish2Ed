#include "BasicThreadManagement.h"

#include <cassert>

namespace basic_thread_management
{
	namespace launching_a_thread
	{
		void launching_a_thread_example() {
			std::thread my_thread(
				[]() {
					std::cout << "launching a thread\n";
				}
			);
			my_thread.join();
		}

		void run_background_task()
		{
			//background_task t;
			std::jthread f(background_task{});
			std::jthread f2{ background_task() };
			std::jthread f3((background_task()));
		}
	}
	namespace running_threads_in_background {
		void back_ground_test() {
			std::thread t([] {
				std::cout << "running background\n";
				});
			t.detach();
			assert(!t.joinable());
		}
	}
}
namespace passing_arg_to_thread_func 
{
	void f(int i, std::string const& s) {
		std::cout << "f(" << i << "," << s << ")\n";
	}
	void oops(int some_param) {
		char buffer[1024];
		sprintf(buffer, "%i", some_param);
#if 0
		std::thread t(f, 3, buffer);
#else
		std::thread t(f, 3, std::string(buffer));// using std::string avoid dangling pointer
#endif
		t.detach();
	}

	void update_data_for_widget(uint32_t w_id, widget& data)
	{
		std::cout << "update_data_for_widget(" << w_id << ")\n";
	}

	void oops_again(uint32_t w_id)
	{
		widget data;
#if 0
		std::thread t(update_data_for_widget, w_id, data);//error
#else
		std::thread t(update_data_for_widget, w_id, std::ref(data));
#endif
		t.join();
	}

	void X::do_lengthy_work(int x,std::string const&s)
	{
		std::cout << "do_lengthy_work(" << x << "," << s << ")\n";
	}
	void passing_this_with_memfn()
	{
		X my_x;
		char buf[] = "passing";
		std::thread t(&X::do_lengthy_work, &my_x, 57, std::string(buf));
		t.join();
	}

	void process_big_object(std::unique_ptr<Big_Obj> pw) {
		std::cout << "process_big_object: " << pw->getName() << "\n";
	}
	void passing_arg_by_move() {
#if 0
		auto pw = std::make_unique<Big_Obj>("Named Obj");
		std::thread t(process_big_object, std::move(pw));
#else
		std::thread t(process_big_object, std::make_unique<Big_Obj>("Uname Obj"));
#endif
		t.join();
	}
}

namespace transferring_ownership_of_a_thread {

	void some_func() { std::cout << "some func\n"; }
	void some_other_func() { std::cout << "some other func\n"; }
	void move_thread_example()
	{
#if 0
		std::thread t1(some_func);
		std::thread t2 = std::move(t1);		
		t1 = std::thread(some_other_func);
		std::thread t3;		
		t3 = std::move(t2);
		t1 = std::move(t3);
		t1.join();
		t2.join();
		t3.join();
#endif
	}
}



void managing_threads_example() {

#if 0
	// 2.1 basic thread management
	{
		using namespace basic_thread_management;
		// 2.1.1 launching a thread
		{
			using namespace launching_a_thread;
			launching_a_thread_example();
			run_background_task();
			{
				using namespace list_2_1;
				oops();
			}
		}
		// 2.1.3
		{
			using namespace waiting_in_exceptional_circumstances;
			{
				using namespace list_2_2;
				f();
			}
		}
		// 2.1.4
		{
			using namespace running_threads_in_background;
			back_ground_test();
		}
	}
#else
	// 2.2 passing argument to a thread function
	{
		using namespace passing_arg_to_thread_func;
		oops(42);
		oops_again(32);
		passing_this_with_memfn();
		passing_arg_by_move();
	}
	// 2.3 transferring ownership of a thread
	{
		using namespace transferring_ownership_of_a_thread;
		move_thread_example();
		// list 2.5
		{
			using namespace list_2_5;
#if 0
			auto t = f();
			t.join();

			auto t2 = g();
			t2.join();
#else
			g();

#endif
		}
		// list 2.6
		{
			using namespace list_2_6;

			f();
		}
		// list 2.7
		{
			using namespace list_2_7;
			f();
		}
		// list 2.8
		{
			using namespace list_2_8;
			f();
		}
	}
#endif
	// 2.4 Choosing the number of threads at runtime
	{
		using namespace chapter_2_4;
		// list 2.9
		{
			using namespace list_2_9;
			parallel_accumulate_example();
		}
	}
	// 2.5 Identifying threads
	{
		using namespace identifying_threads;
		auto id = std::this_thread::get_id();
		std::cout << "this thread id: " << id << "\n";
	}
}
