#pragma once
#include <tuple>
template<size_t N>
struct vistek_function_exec
{
	template<typename F, typename T, typename... A>
	static inline auto exec(F && f, T && t, A &&... a)
		-> decltype(vistek_function_exec<N - 1>::exec(
		::std::forward<F>(f), ::std::forward<T>(t),
		::std::get<N - 1>(::std::forward<T>(t)),
		::std::forward<A>(a)...
		))
	{
		return vistek_function_exec<N - 1>::exec(::std::forward<F>(f),
			::std::forward<T>(t),
			::std::get<N - 1>(::std::forward<T>(t)),
			::std::forward<A>(a)...
			);
	}
};

template<>
struct vistek_function_exec < 0 >
{
	template<typename F, typename T, typename... A>
	static inline auto exec(F && f, T &&, A &&... a)
		-> decltype(::std::forward<F>(f)
		(::std::forward<A>(a)...))
	{
		return ::std::forward<F>(f)(::std::forward < A >
			(a)...);
	}
};
template<typename F, typename T>
inline auto vistek_exec(F && f, T && t)
-> decltype(vistek_function_exec< ::std::tuple_size<
typename ::std::decay<T>::type
>::value>::exec(::std::forward<F>(f),
::std::forward<T>(t)))
{
	return vistek_function_exec< ::std::tuple_size<
		typename ::std::decay<T>::type
	>::value>::exec(::std::forward<F>(f),
	::std::forward<T>(t));
}