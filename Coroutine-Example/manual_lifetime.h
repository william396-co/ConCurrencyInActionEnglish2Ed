#pragma once

#include <memory>

template<typename T>
struct manual_lifetime {
public:
	manual_lifetime()noexcept {}
	~manual_lifetime()noexcept {}

	template<typename...Args>
	void construct(Args&&...args) {
		::new(static_cast<void*>(std::addressof(value))) T(std::forward<Args>(args)...);
	}
	void destruct() {
		value.~T();
	}

	T& get()& { return value; }
	T const& get()const& { return value; }
	T&& get()&& { return static_cast<T&&>(value); }
	T const&& get()const&& { return static_cast<T const&&>(value); }

private:
	union { T value; };
};

template<typename T>
struct manual_lifetime<T&&> {
	manual_lifetime()noexcept :ptr{ nullptr } {}
	~manual_lifetime()noexcept {}

	void construct(T&& value)noexcept {
		ptr = std::addressof(value);
	}
	void destruct()noexcept {
		ptr = nullptr;
	}
	T&& get()const noexcept { return *ptr; }
private:
	T* ptr;
};

template<>
struct manual_lifetime<void> {
	void construct()noexcept{}
	void destruct()noexcept {}
	void get() const noexcept {}
};