#ifndef UTIL_MACRO_HPP
#define UTIL_MACRO_HPP

#define EVAL(...)  EVAL1(EVAL1(EVAL1(__VA_ARGS__)))
#define EVAL1(...) EVAL2(EVAL2(EVAL2(__VA_ARGS__)))
#define EVAL2(...) EVAL3(EVAL3(EVAL3(__VA_ARGS__)))
#define EVAL3(...) EVAL4(EVAL4(EVAL4(__VA_ARGS__)))
#define EVAL4(...) EVAL5(EVAL5(EVAL5(__VA_ARGS__)))
#define EVAL5(...) __VA_ARGS__

#define CHECK2(x, n, ...) n

#define LPAREN (
#define RPAREN )
#define COMMA ,

#define NIL(...)
#define IDENT(...) __VA_ARGS__
#define DEFER(...) IDENT(__VA_ARGS__)NIL()
#define EXPAND(...) IDENT(__VA_ARGS__)DEFER(NIL)()

#define BIND(fn, args) fn LPAREN IDENT args, BIND_EAT
#define BIND_EAT(...) __VA_ARGS__)

#define APPLY(fn, args) DEFER(DEFER(fn) args)

#define INDIRECT(fn) IDENT(fn)NIL

#define CAT(a, ...) PRIMITIVE_CAT(a, __VA_ARGS__)
#define PRIMITIVE_CAT(a, ...) a ## __VA_ARGS__

#define IFEMPTY_(th) 0, th
#define IFEMPTY(x, th, el) DEFER(DEFER(CHECK2)(CAT(IFEMPTY_, x)(th), el, 0))

#define FOREACH(fn, x, ...) EVAL((fn(x)FOREACH_IMPL(fn, __VA_ARGS__,,)))
#define FOREACH_IMPL(fn, x, y, ...) \
	, fn(x)EXPAND(IFEMPTY(y,INDIRECT(NIL),FOREACH_IMPL_))()(fn, y, __VA_ARGS__)
#define FOREACH_IMPL_() FOREACH_IMPL

#endif
