#ifndef PARAMS_HPP
#define PARAMS_HPP

#include <functional>

template<class T>
struct __in : public std::reference_wrapper<T const> {
	explicit __in(T const& t):std::reference_wrapper<T const>(t) {}
	__in(__in<T>&& o):std::reference_wrapper<T const>(std::move(o)) {}
	
	void operator=(__in<T> const& o) = delete;
};

template<class T>
struct __io : public std::reference_wrapper<T> {
	explicit __io(T& t):std::reference_wrapper<T>(t) {}
	__io(__io<T>&& o):std::reference_wrapper<T>(std::move(o)) {}
};

template<typename T>
struct __out {
	private:
		T&
	public:
		__out(T& v):t(v) {}
		__out(__out<T>&& o):t(o.t) {}
		
		void operator=(__out<T> const& o) = delete;
		void operator=(__out<T>&& o) = delete;
		void operator=(__out<T>& o) = delete;
		
		template<class U>
		__out<T>& operator=(U&& u) {
			t = std::forward<U>(u);
			return *this;
		}
};

#endif

