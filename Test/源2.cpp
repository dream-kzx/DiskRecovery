
#include <iostream>
#include "ThreadPool.h"

void func()
{
	std::this_thread::sleep_for(std::chrono::milliseconds(100));
	std::cout << "worker thread ID:" << std::this_thread::get_id() << std::endl;
}

//int main()
//{
//	ThreadPool pool(4);
//	while (1)
//	{
//		pool.enqueue(func);
//	}
//
//}