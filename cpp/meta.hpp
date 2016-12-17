#ifndef META_UTILS_HPP
#define META_UTILS_HPP

/**
 * Empty structure
**/
struct Empty {};

/**
 * ::type is T if b is true, else F
**/
template<bool b, typename T, typename F>
struct Choose {
	typedef T type;
};
template<typename T, typename F>
struct Choose<false, T, F> {
	typedef F type;
};

/**
 * Pick the first type in the given type list for which C<T>::value is true
**/
template<template<typename> class C, typename T, typename... REST>
struct Fallback {
	typedef typename Choose<
		C<T>::value, T, typename Fallback<C, REST...>::type
	>::type type;
};
template<template<typename> class C, typename T, typename R>
struct Fallback<C, T, R> {
	typedef typename Choose<C<T>::value, T, R>::type type;
};

template<template<typename T> class C, typename... T>
using fallback = typename Fallback<C, T...>::type;

/**
 * Call this to generate a SFINAE class that checks if the given type has
 *  a method which also has the given signature, setting ::value
**/
#define NEW_TEST_METHOD(method) \
	template<typename C, typename... Args> \
	struct __has_ ## method { \
		template<typename T> \
		static constexpr auto check(T*) -> \
			decltype( std::declval<T>().method(std::declval<Args>()...)); \
		template<typename> static constexpr std::false_type check(...); \
		typedef decltype(check<C>(0)) type; \
		static constexpr bool value = type::value; };

//Iterability test, courtesy of this SO answer:
//http://stackoverflow.com/questions/13830158/#answer-29634934

namespace detail {
	//To allow ADL with custom begin/end
	using std::begin;
	using std::end;
	
	template<typename T>
	auto is_iterable_impl(int)
	-> decltype (
		//Test for begin/end and operator !=
		begin(std::declval<T&>()) != end(std::declval<T&>()),
		void(), // Handle evil operator ,
		++std::declval<decltype(begin(std::declval<T&>()))&>(), // operator ++
		void(*begin(std::declval<T&>())), // operator*
		std::true_type{});

	template <typename T>
	std::false_type is_iterable_impl(...);

}

template <typename T>
using is_iterable = decltype(detail::is_iterable_impl<T>(0));

#endif

