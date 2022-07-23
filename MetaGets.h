#ifndef _META_GETS_H_
#define _META_GETS_H_

#include <type_traits>
#include <tuple>

#include "TypeTraits.h"

template <typename...T>
struct TTypelist;

template <typename... T>
struct TTypelist<std::tuple<T...>>
{
	template <std::size_t N>
	using type = typename std::tuple_element<N, std::tuple<T...>>::type;
};

template <typename... T>
struct TValueList;

template <typename... T, typename TFunc>																		\
void for_each_ex(const TValueList<T...>& value_list, TFunc&& func);
template <typename... T, typename TFunc>																		\
void for_each_ex(TValueList<T...>&& value_list, TFunc&& func);

template <typename... T>
struct TValueList : std::tuple<T...>
{
protected:
	using super = std::tuple<T...>;
	using type_list = std::tuple<T...>;
	using decay_type_list = std::tuple<typename std::decay<T>::type...>;

public:
	using copy_type_list = TValueList<typename std::decay<T>::type...>;

	TValueList(T&&... args)
		: super(std::forward<T&&>(args)...)
	{ }

	template <size_t N>
	typename TTypelist<type_list>::template type<N> get() noexcept
	{
		return std::get<N>(*this);
	}

	template <size_t N>
	typename TTypelist<type_list>::template type<N> get() const noexcept
	{
		return std::get<N>(*this);
	}

	template <size_t... TIdx>
	TValueList<typename TTypelist<type_list>::template type<TIdx>...> gets() noexcept
	{
		using type = TValueList<typename TTypelist<type_list>::template type<TIdx>...>;
		return type{ std::get<TIdx>(*this)... };
	}

	template <size_t... TIdx>
	TValueList<typename TTypelist<type_list>::template type<TIdx>...> gets() const noexcept
	{
		using type = TValueList<typename TTypelist<type_list>::template type<TIdx>...>;
		return type{ std::get<TIdx>(*this)... };
	}

	struct FCopy
	{
		TValueList& _this;

		FCopy(TValueList& o)
			: _this(o)
		{ }

		template <size_t TIdx, typename TValue>
		void operator()(TValue&& value)
		{
			_this.get<TIdx>() = std::forward<TValue>(value);
		}
	};

	template <
		typename... TOther,
		typename = typename std::enable_if<
			std::is_same<decay_type_list, std::tuple<typename std::decay<TOther>::type...>>::value
		>::type
	>
	TValueList& operator=(const TValueList<TOther...>& other)
	{
		for_each_ex(other, FCopy(*this));
		return *this;
	}

	template <
		typename... TOther,
		typename = typename std::enable_if<
			std::is_same<decay_type_list, std::tuple<typename std::decay<TOther>::type...>>::value
		>::type
	>
	TValueList& operator=(TValueList<TOther...>&& other)
	{
		for_each_ex(std::forward<TValueList<TOther...>>(other), FCopy(*this));
		return *this;
	}
};

template <typename T, typename = void>
struct TIsValueList : std::false_type
{ };

template <typename T>
struct TIsValueList<T, typename TVoid<decltype(std::declval<typename std::decay<T>::type::copy_type_list>())>::type> : std::true_type
{ };

template <typename T, size_t... TIdxs>
struct TMetaValueListType
{
#if defined(_WIN32) || defined(_WIN64)
	using type = TValueList<decltype(std::declval<T>().meta_get<TIdxs>())...>;
#else
	using type = TValueList<decltype(std::declval<T>().template meta_get<TIdxs>())...>;
#endif
};

template <
	size_t... TIdxs, typename T, 
	typename = typename std::enable_if<TIsStaticReflection<T>::value>::type
>
typename TMetaValueListType<T, TIdxs...>::type meta_gets(T&& obj) noexcept
{
	using type = typename TMetaValueListType<T, TIdxs...>::type;
#if defined(_WIN32) || defined(_WIN64)
	return type{ obj.meta_get<TIdxs>()... };
#else
	return type{ obj.template meta_get<TIdxs>()... };
#endif
}

template <typename T, size_t... TIdxs>
struct TFieldValueListType
{
#if defined(_WIN32) || defined(_WIN64)
	using type = TValueList<decltype(std::declval<T>().get<TIdxs>())...>;
#else
	using type = TValueList<decltype(std::declval<T>().template get<TIdxs>())...>;
#endif
};

#if defined(_WIN32) || defined(_WIN64)
template <size_t... TIdxs, typename T>
#else
template <
	size_t... TIdxs, typename T,
	typename = typename std::enable_if<TIsStaticReflection<T>::value || TIsValueList<T>::value>::type
>
#endif
typename TFieldValueListType<T, TIdxs...>::type gets(T&& obj) noexcept
{
	using type = typename TFieldValueListType<T, TIdxs...>::type;
#if defined(_WIN32) || defined(_WIN64)
	return type{ obj.get<TIdxs>()... };
#else
	return type{ obj.template get<TIdxs>()... };
#endif
}

namespace details
{
	template <size_t TIdx, size_t TMax, typename TValue, typename TFunc>
	void for_each_aux_1(TValue&& value, TFunc&& func, std::false_type)
	{ }

	template <size_t TIdx, size_t TMax, typename TValue, typename TFunc>
	void for_each_aux_1(TValue&& value, TFunc&& func, std::true_type)
	{
#if defined(_WIN32) || defined(_WIN64)
		func(value.get<TIdx>());
#else
		func(value.template get<TIdx>());
#endif
		for_each_aux_1<TIdx + 1, TMax>(std::forward<TValue>(value), std::forward<TFunc>(func), std::integral_constant<bool, (TIdx + 1) != TMax>{ });
	}

	template <size_t TIdx, size_t TMax, typename T, typename TFunc>
	void for_each_aux(T&& values, TFunc&& func)
	{
		for_each_aux_1<TIdx, TMax>(std::forward<T>(values), std::forward<TFunc>(func), std::integral_constant<bool, TIdx != TMax>{ });
	}

	template <size_t TIdx, size_t TMax, typename TValue, typename TFunc>
	void for_each_ex_aux_1(TValue&& value, TFunc&& func, std::false_type)
	{ }

	template <size_t TIdx, size_t TMax, typename TValue, typename TFunc>
	void for_each_ex_aux_1(TValue&& value, TFunc&& func, std::true_type)
	{
#if defined(_WIN32) || defined(_WIN64)
		func.operator()<TIdx>(value.get<TIdx>());
#else
		func.template operator()<TIdx>(value.template get<TIdx>());
#endif
		for_each_ex_aux_1<TIdx + 1, TMax>(std::forward<TValue>(value), std::forward<TFunc>(func), std::integral_constant<bool, (TIdx + 1) != TMax>{ });
	}

	template <size_t TIdx, size_t TMax, typename T, typename TFunc>
	void for_each_ex_aux(T&& values, TFunc&& func)
	{
		for_each_ex_aux_1<TIdx, TMax>(std::forward<T>(values), std::forward<TFunc>(func), std::integral_constant<bool, TIdx != TMax>{ });
	}
}

#define META_VALUE_LIST_FOR_EACH(Qualifier)											\
template <typename... T, typename TFunc>											\
void for_each(TValueList<T...> Qualifier value_list, TFunc&& func)					\
{																					\
	details::for_each_aux<0, sizeof...(T)>(value_list, std::forward<TFunc>(func));	\
}																					\
// ...

META_VALUE_LIST_FOR_EACH(&);
META_VALUE_LIST_FOR_EACH(&&);
META_VALUE_LIST_FOR_EACH(const&);
META_VALUE_LIST_FOR_EACH(const&&);
META_VALUE_LIST_FOR_EACH(volatile&);
META_VALUE_LIST_FOR_EACH(volatile&&);
META_VALUE_LIST_FOR_EACH(const volatile&);
META_VALUE_LIST_FOR_EACH(const volatile&&);

#undef META_VALUE_LIST_FOR_EACH

#define META_VALUE_LIST_FOR_EACH_EX(Qualifier)											\
template <typename... T, typename TFunc>												\
void for_each_ex(TValueList<T...> Qualifier value_list, TFunc&& func)					\
{																						\
	details::for_each_ex_aux<0, sizeof...(T)>(value_list, std::forward<TFunc>(func));	\
}																						\
// ...

META_VALUE_LIST_FOR_EACH_EX(&);
META_VALUE_LIST_FOR_EACH_EX(&&);
META_VALUE_LIST_FOR_EACH_EX(const&);
META_VALUE_LIST_FOR_EACH_EX(const&&);
META_VALUE_LIST_FOR_EACH_EX(volatile&);
META_VALUE_LIST_FOR_EACH_EX(volatile&&);
META_VALUE_LIST_FOR_EACH_EX(const volatile&);
META_VALUE_LIST_FOR_EACH_EX(const volatile&&);

#undef META_VALUE_LIST_FOR_EACH_EX

#endif // !_META_GETS_H_
