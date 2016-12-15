#ifndef I_ARITHMETIC_HPP
#define I_ARITHMETIC_HPP

#define opeq(op) A& operator op ## = (const A& a) { \
	*this = *this op a; \
	return *this; }

template<class A>
struct I_Arithmetic {
	opeq(+)
	opeq(-)
	opeq(*)
	opeq(/)
	opeq(%)
	
	A& operator++() {
		*this = *this + 1;
		return *this;
	}
	
	A& operator++(int) {
		*this = *this + 1;
		return *this;
	}
	
	A& operator--() {
		*this = *this - 1;
		return *this;
	}
	
	A& operator--(int) {
		*this = *this - 1;
		return *this;
	}
	
	operator bool() const {
		return *this != 0;
	}
	
	bool operator!=(const A& a) {
		return !(*this == a);
	}
	
	bool operator>=(const A& a) {
		return (*this > a) || (*this == a);
	}
	
	bool operator<=(const A& a) {
		return (*this < a) || (*this == a);
	}
};

#undef opeq

#endif

