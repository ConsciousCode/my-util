#ifndef PRETTY_PRINT_HPP
#define PRETTY_PRINT_HPP

#include <utility>
#include <typeinfo>

#include "meta.hpp"

template<typename S, typename T>
struct PrettyPrinter;

namespace impl {
	/**
	 * Implement pretty printing for something with a Java-like toString method.
	**/
	template<typename S, typename T>
	struct JavalikePrinter {
		static S print(const T& v) {
			return (S)v.toString();
		}
	};
	NEW_TEST_METHOD(toString)
	
	/**
	 * Implement pretty printing for something with a C-like str method, such
	 *  as stringstream.
	**/
	template<typename S, typename T>
	struct ClikePrinter {
		static S print(const T& v) {
			return (S)v.str();
		}
	};
	NEW_TEST_METHOD(str)
	
	/**
	 * Prints T if it can be converted to the string type S
	**/
	template<typename S, typename T>
	struct ConversionPrinter {
		static S print(const T& v) {
			return (S)v;
		}
	};
	
	/**
	 * Implement pretty printing for types which implement C++'s static
	 *  iteration interface, e.g. for(auto v : seq)
	**/
	template<typename S, typename T>
	struct IterablePrinter {
		static S print(const T& t) {
			S res = PrettyPrinter<S, T>::type() + "{";
			bool first = true;
			for(auto v : t) {
				if(first) {
					res += PrettyPrinter<S, decltype(v)>::print(v);
					first = false;
				}
				else {
					res += ", " + PrettyPrinter<S, decltype(v)>::print(v);
				}
			}

			return res + "}";
		}
	};
	
	/**
	 * None of our printer methods worked, so we default to something which
	 *  is guaranteed to work.
	**/
	template<typename S, typename T>
	struct UglyPrinter {
		static S print(const T& v) {
			return (S)("<" + PrettyPrinter<S, T>::type() + " @ " +
				PrettyPrinter<S, size_t>::print(&v, 16) +
			">");
		}
	};
	
	template<typename S, typename T, typename P>
	struct DefaultPrettyPrinterSelector {
		static constexpr bool value = false;
	};
	template<typename S, typename T>
	struct DefaultPrettyPrinterSelector<S, T, JavalikePrinter<S, T>> {
		static constexpr bool value = __has_toString<T, S>::value;
	};
	template<typename S, typename T>
	struct DefaultPrettyPrinterSelector<S, T, ClikePrinter<S, T>> {
		static constexpr bool value = __has_str<T, S>::value;
	};
	template<typename S, typename T>
	struct DefaultPrettyPrinterSelector<S, T, IterablePrinter<S, T>> {
		static constexpr bool value = is_iterable<T>();
	};
	template<typename S, typename T>
	struct DefaultPrettyPrinterSelector<S, T, ConversionPrinter<S, T>> {
		static constexpr bool value = std::is_convertible<T, S>();
	};
	template<typename S, typename T>
	struct DefaultPrettyPrinterSelector<S, T, UglyPrinter<S, T>> {
		static constexpr bool value = true;
	};
	
	/**
	 * This is needed because Default... needs S and T, but Fallback
	 *  operates on a template that takes one type.
	**/
	template<typename S, typename T>
	struct ForwardPP_ST {
		template<typename P>
		struct Selector : public DefaultPrettyPrinterSelector<S, T, P> {};
		
		using type = fallback<
			Selector,
	
			JavalikePrinter<S, T>,
			ClikePrinter<S, T>,
			ConversionPrinter<S, T>,
			IterablePrinter<S, T>,
			UglyPrinter<S, T>
		>;
	};
}

/**
 * This defines the default pretty printer for anything not given a
 *  template specialization.
 *
 * We inherit from alternative because there's a
 *  number of common pretty printing interfaces we could use, but no guarantee
 *  that they're mutually exclusive, so we want to choose the one we want most
 *  to prevent any errors about ambiguous calls.
**/
template<typename S, typename T>
struct DefaultPrettyPrinter : public impl::ForwardPP_ST<S, T>::type {
	/**
	 * type_info is the only exposed interface for getting a name for T.
	**/
	static S type() {
		return (S)typeid(T).name();
	}
};

//Use this if all else fails
template<typename S, typename T>
struct PrettyPrinter : public DefaultPrettyPrinter<S, T> {};

template<typename S>
struct PrettyPrinter<S, const char*> {
	static S type() {
		return (S)"const char*";
	}
	
	static S print(const char* s) {
		return (S)s;
	}
};

template<typename S>
struct PrettyPrinter<S, bool> {
	static S type() {
		return (S)"bool";
	}
	
	static S print(bool v) {
		return (S)(v? "true" : "false");
	}
};

template<typename S>
struct PrettyPrinter<S, char> {
	static S type() {
		return (S)"char";
	}
	
	static S print(char v) {
		switch(v) {
			case '\\': return (S)"'\\'";
			case '\'': return (S)"'\''";
			case '\0': return (S)"'\\0'";
			case '\a': return (S)"'\\a'";
			case '\b': return (S)"'\\b'";
			case '\t': return (S)"'\\t'";
			case '\n': return (S)"'\\n'";
			case '\v': return (S)"'\\v'";
			case '\f': return (S)"'\\f'";
			case '\r': return (S)"'\\r'";
			
			default:
				if(v < ' ') {
					const char* HEXDIGIT = "0123456789abcdef";
					char buf[] = {
						'\'', 'x',
							HEXDIGIT[(v>>4)&0x0f], HEXDIGIT[v&0x0f],
						'\'', 0
					};
					
					return (S)buf;
				}
				else {
					char buf[] = {'\'', v, '\'', 0};
					return (S)buf;
				}
		}
	}
};

template<typename S, typename T>
struct IntPrettyPrinter {
	static S print(T i, unsigned base) {
		const char* DIGITS;
		
		switch(base) {
			case 12:
				DIGITS = "0123456789TE";
				break;
			
			case 58:
				DIGITS = "123456789"
					"ABCDEFGHJKLMNPQRSTUVWXYZ"
					"abcdefghijkmnopqrstuvwxyz";
				break;
			
			case 64:
				DIGITS =
					"ABCDEFGHIJKLMNOPQRSTUVWXYZ"
					"abcdefghijklmnopqrstuvwxyz"
					"0123456789+/";
				break;
			
			case 85:
				DIGITS = "0123465789"
					"ABCDEFGHIJKLMNOPQRSTUVWXYZ"
					"abcdefghijklmnopqrstuvwxyz"
					"!#$%&()*+-;<=>?@^_`{|}~";
				break;
			
			default:
				DIGITS = "0123456789"
					"ABCDEFGHIJKLMNOPQRSTUVWXYZ"
					"abcdefghijklmnopqrstuvwxyz";
				break;
		}
		
		S s = "";
		
		if(i < 0) {
			s += '-';
			i = -i;
		}
		
		while(i > 1) {
			s += DIGITS[i%base];
			i /= base;
		}
		
		return s;
	}
};

#define IPP(x) \
	template<typename S> \
	struct PrettyPrinter<S, x> : public IntPrettyPrinter<S, x> { \
		static S type() { \
			return (S)#x; \
		} \
	};

IPP(signed char) IPP(unsigned char)
IPP(short) IPP(unsigned short)
IPP(int) IPP(unsigned int)
IPP(long) IPP(unsigned long)
IPP(long long) IPP(unsigned long long)

#undef IPP

template<typename S>
struct PrettyPrinter<S, float> {
	static S type() {
		return (S)"float";
	}
	
	static S print(float f) {
		char buf[64];
		snprintf(buf, 64, "%.6g", (double)f);
		
		return (S)buf;
	}
};

template<typename S>
struct PrettyPrinter<S, double> {
	static S type() {
		return (S)"double";
	}
	
	static S print(double f) {
		char buf[64];
		snprintf(buf, 64, "%.g", f);
		
		return (S)buf;
	}
};

template<typename S>
struct PrettyPrinter<S, long double> {
	static S type() {
		return (S)"long double";
	}
	
	static S print(long double f) {
		char buf[64];
		snprintf(buf, 64, "%.Lg", f);
		
		return (S)buf;
	}
};

#define MOD(pre, post) \
	template<typename S, typename T> \
	struct PrettyPrinter<S, pre T post> \
	 : public DefaultPrettyPrinter<S, pre T post> { \
		static S type() { \
			return #pre " " + PrettyPrinter<S, pre T post>::type() + #post; \
		} \
	};

MOD(const,)
MOD(,&)
MOD(,&&)
MOD(,[])

#undef MOD

template<typename S, typename T>
struct PrettyPrinter<S, T*> {
	static S type() {
		return PrettyPrinter<S, T>::type() + "*";
	}
	
	static S print(T* v) {
		return "<" + type() + " @ " +
			(v?PrettyPrinter<S, size_t>::print(v) : "null") + ">";
	}
};

template<typename S, typename T>
struct PrettyPrinter<S, T const*> {
	static S type() {
		return PrettyPrinter<S, T>::type() + " const*";
	}
	
	static S print(T* v) {
		return "<" + type() + " @ " +
			(v?PrettyPrinter<S, size_t>::print(v) : "null") + ">";
	}
};

namespace {
	template<typename S, typename T, typename... R>
	S print_typelist() {
		return PrettyPrinter<S, T>::type() + ", " + print_typelist<S, R...>();
	}
	
	template<typename S, typename T>
	S print_typelist() {
		return PrettyPrinter<S, T>::type();
	}
}

template<typename S, typename R, typename... A>
struct PrettyPrinter<S, R(*)(A...)>
 : public DefaultPrettyPrinter<S, R(*)(A...)> {
	static S type() {
		return PrettyPrinter<S, R>::type() + "(*)(" +
			print_typelist<S, A...>() + ")";
	}
};

template<typename S, typename T, size_t N>
struct PrettyPrinter<S, T(&)[N]> {
	static S type() {
		return PrettyPrinter<S, T>::type() + "[" +
			PrettyPrinter<S, size_t>::print(N) + "]";
	}
	
	static S print(T(&a)[N]) {
		S s = type() + "{";
		bool first = true;
		for(int i = 0; i < N; ++i) {
			if(first) {
				s += PrettyPrinter<S, T>::print(a[i]);
				first = false;
			}
			else {
				s += ", " + PrettyPrinter<S, T>::print(a[i]);
			}
		}
		
		return s + "}";
	}
};

template<typename S, typename L, typename R>
struct PrettyPrinter<S, std::pair<L, R>> {
	static S type() {
		return "std::pair<" +
			PrettyPrinter<S, L>::type() +
			PrettyPrinter<S, R>::type() + ">";
	}
	
	static S print(const std::pair<L, R>& p) {
		return "[" + PrettyPrinter<S, L>::print(p.first) + " : " +
			PrettyPrinter<S, R>::print(p.second) + "]";
	}
};

#define STD(name) \
	namespace std { \
		template<typename T, typename Alloc> class name; \
	} \
	template<typename S, typename T, typename A> \
	struct PrettyPrinter<S, std::name<T, A>> \
	 : public DefaultPrettyPrinter<S, std::name<T, A>> { \
		static S type() { \
			return "std::" #name "<" + PrettyPrinter<S, T>::type() + ">"; \
		} };

namespace std {
	template<typename T, size_t N> class array;
	
	template<typename K, typename T, typename C, typename A> class map;
}
template<typename S, typename T, size_t N>
struct PrettyPrinter<S, std::array<T, N>>
 : public DefaultPrettyPrinter<S, std::array<T, N>> {
	static S type() {
		return "std::array<" + PrettyPrinter<S, T>::type() + ">";
	}
};

template<typename S, typename K, typename T, typename C, typename A>
struct PrettyPrinter<S, std::map<K, T, C, A>>
 : public DefaultPrettyPrinter<S, std::map<K, T, C, A>> {
	static S type() {
		return "std::map<" +
			PrettyPrinter<S, K>::type() + ", " +
			PrettyPrinter<S, T>::type() + ">";
	}
};

STD(deque)
STD(forward_list)
STD(list)
STD(queue)
STD(set)
STD(stack)
STD(unordered_map)
STD(unordered_set)
STD(vector)

#undef STD

template<typename S, typename T>
S pprint(T v) {
	return PrettyPrinter<S, T>::print(v);
}

//Extended pprint for chaining pprints
template<typename S, typename T, typename... REST>
S pprint(T v, REST... r) {
	return pprint<S>(v) + pprint<S>(r...);
}

#endif

