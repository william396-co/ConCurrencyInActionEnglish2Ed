#pragma once

#include <queue>
#include <condition_variable>
#include <mutex>
#include <memory>

template<typename T>
class threadsafe_queue {
public:
	threadsafe_queue() = default;
	void push(T new_val) {
		std::lock_guard lk(mtx);
		data.push(std::move(new_val));
		cond.notify_all();
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
	T pop() {// blocking pop
		std::unique_lock lk(mtx);
		cond.wait(lk, [this] {return !data.empty();});
		auto res = data.front();
		data.pop();
		return res;		
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