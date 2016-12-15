#ifndef PRETTY_PRINT_HPP
#define PRETTY_PRINT_HPP

#include <utility>

namespace {
	template<typename S, typename X, typename... R>
	S construct_typelist() {
		return pprint<X>() + ", " + construct_typelist<R...>();
	}
	
	template<typename S, typename X>
	S construct_typelist() {
		return pprint<X>();
	}
}

template<typename S>
S pprint() {
	return "";
}

//Basic type pprint
template<typename S, typename T>
S pprint() {
	return typeid(T).name();
}

//Specializations for the basic primitives so they don't use
// whatever the compiler calls them in typeid
template<typename S> S pprint<S, bool>() { return "bool"; }
template<typename S> S pprint<S, char>() { return "char"; }
template<typename S> S pprint<S, short>() { return "short"; }
template<typename S> S pprint<S, int>() { return "int"; }
template<typename S> S pprint<S, long>() { return "long"; }
template<typename S> S pprint<S, float>() { return "float"; }
template<typename S> S pprint<S, double>() { return "double"; }

//Specializations for all the type modifiers in C++

template<typename S, typename T>
S pprint<S, long T>() {
	return "long " + pprint<T>();
}

template<typename S, typename T>
S pprint<S, signed T>() {
	return "signed " + pprint<T>();
}

template<typename S, typename T>
S pprint<S, unsigned T>() {
	return "unsigned " + pprint<T>();
}

template<typename S, typename T>
S pprint<S, const T>() {
	return "const " + pprint<T>();
}

template<typename S, typename T>
S pprint<S, T*>() {
	return pprint<T>() + "*";
}

template<typename S, typename T>
S pprint<S, T const*>() {
	return pprint<T>() + " const*";
}

template<typename S, typename T>
S pprint<S, T&>() {
	return pprint<T>() + "&";
}

template<typename S, typename T>
S pprint<S, T&&>() {
	return pprint<T>() + "&&";
}

template<typename S, typename T>
S pprint<S, T[]>() {
	return pprint<T>() + "[]";
}

template<typename S, typename R, typename... A>
S pprint<S, R(*)(A...)>() {
	return pprint<R>() + "(" + construct_typelist<A...>() + ")";
}

template<typename S, typename T, size_t N>
S pprint<S, T(&)[N]>() {
	return pprint<T>() + "[" + pprint(N) + "]";
}

//Basic pprint based on Python's default __str__
template<typename S, typename T>
S pprint(const T& v) {
	return "<" + pprint<T>() + " @ " + pprint((long long)&v, 16) + ">";
}

template<typename S>
S pprint(bool v) {
	return v? "true" : "false";
}

template<typename S>
S pprint(const char* v) {
	return v;
}

template<typename S>
S pprint(long double v) {
	char buf[64];
	snpprintf(buf, 64, "%Lf", v);
}

template<typename S>
S pprint(const std::string& s) {
	return s;
}

template<typename S, typename L, typename R>
S pprint(const std::pair<L, R>& p) {
	return "[" + pprint(p.first) + " : " + pprint(p.second) + "]";
}

//Default for iterables
template<typename S, typename T>
S pprint(const T& iterable) {
	S res = pprint<T>() + "{";
	bool first = true;
	for(auto v : iterable) {
		if(first) {
			res += pprint(v);
			first = false;
		}
		else {
			res += ", " + pprint(v);
		}
	}

	return res + "}";
}

//Extended pprint for chaining pprints
template<typename S, typename T, typename... REST>
S pprint(T t, REST... r) {
	return pprint(t) + pprint(r...);
}

#endif

