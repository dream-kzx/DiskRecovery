#pragma once
#include<thread>
#include<mutex>
#include<condition_variable>


class Semaphore {
private:
	int count;
	std::mutex mtk;
	std::condition_variable cv;

public:
	Semaphore(int value=1):count(value){}

	void wait() {
		std::unique_lock<std::mutex> lck(mtk);
		if (--count < 0) {
			cv.wait(lck);
		}
	}

	void signal() {
		std::unique_lock<std::mutex> lck(mtk);
		if (++count <= 0) {
			cv.notify_one();
		}
	}
};