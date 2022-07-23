#ifndef _STATIC_REFL_FUNCTION_H_
#define _STATIC_REFL_FUNCTION_H_

#include <string>

#include "TypeTraits.h"

namespace details
{
	// 2022-07-22 20:06:19
	// fix copy cost. (...) => (T&&...)
	template <size_t TIdx, typename... T>
	void for_each_aux(T&&...)
	{ }

	template <typename TValue, typename TFunc>
	void for_each_aux_1(const char* name, TValue&& obj, TFunc&& func, std::false_type)
	{
		func(name, std::forward<TValue>(obj));
	}

	template <typename TValue, typename TFunc>
	void for_each_aux_1(const char* name, TValue&& obj, TFunc&& func, std::true_type)
	{
		for_each(std::forward<TValue>(obj), std::forward<TFunc>(func));
	}

	template <
		size_t TIdx, typename T, typename TFunc,
		typename = typename std::enable_if<(TIdx < std::decay<T>::type::member_count)>::type
	>
	void for_each_aux(T&& obj, TFunc&& func)
	{
		using field_type = details::TField<T, TIdx>;
		field_type field{ obj };

		for_each_aux_1(field.name(), field.value(), std::forward<TFunc>(func), ::TIsStaticReflection<typename field_type::value_type>{});

		// 2022-07-22 20:06:19
		// fix copy cost. obj => std::forward<T>(obj)
		for_each_aux<TIdx + 1>(std::forward<T>(obj), std::forward<TFunc>(func));
	}
}

/*
 * @brief handle static_refl_object each field name and value.
 * @param obj static reflected object.
 * @param func handle static_refl_object field name and value function,
 *		must can call func(const char*, TValue&&) function.
 * @return no return value.
 */
template <typename T, typename TFunc, typename = typename std::enable_if<TIsStaticReflection<T>::value>::type>
void for_each(T&& obj, TFunc&& func)
{
	// 2022-07-22 20:06:19
	// fix copy cost. obj => std::forward<T>(obj)
	details::for_each_aux<0>(std::forward<T>(obj), std::forward<TFunc>(func));
}

namespace details
{
	template <typename TValue, typename TFunc>
	void for_each_aux_1(std::string super_name, const char* name, TValue&& obj, TFunc&& func, std::false_type)
	{
		super_name.append(".").append(name);
		func(super_name.c_str(), std::forward<TValue>(obj));
	}

	template <typename TValue, typename TFunc>
	void for_each_aux_1(std::string super_name, const char* name, TValue&& obj, TFunc&& func, std::true_type)
	{
		super_name.append(".").append(name);
		for_each(super_name, std::forward<TValue>(obj), std::forward<TFunc>(func));
	}

	template <
		size_t TIdx, typename T, typename TFunc,
		typename = typename std::enable_if<(TIdx < std::decay<T>::type::member_count)>::type
	>
	void for_each_aux(const std::string& name, T&& obj, TFunc&& func)
	{
		using field_type = details::TField<T, TIdx>;
		field_type field{ obj };

		for_each_aux_1(name, field.name(), field.value(), std::forward<TFunc>(func), ::TIsStaticReflection<typename field_type::value_type>{});

		for_each_aux<TIdx + 1>(name, std::forward<T>(obj), std::forward<TFunc>(func));
	}
}

/*
 * @brief
 *		handle static_refl_object each field name and value,
 *		this name format is "name.filed_name.sub_field_name.[...].meta_name".
 * @param name variable name
 * @param obj static reflected object.
 * @param func handle static_refl_object field name and value function,
 *		must can call func(const char*, TValue&&) function.
 * @return no return value.
 */
template <typename T, typename TFunc, typename = typename std::enable_if<TIsStaticReflection<T>::value>::type>
void for_each(const std::string& name, T&& obj, TFunc&& func)
{
	details::for_each_aux<0>(name, std::forward<T>(obj), std::forward<TFunc>(func));
}

/*
 * @brief
 *		handle static_refl_object each field name and value,
 *		this function will call func.template operator()<TMetaIdx>(name, value),
 *		so you can call static_refl_object.meta_set/meta_get<TMetaIdx>(args...) function.
 * @param obj static reflected object.
 * @param func handle static_refl_object field name and value function,
 *		must have template<size_t TMetaIdx, typename TValue> operator()(const char*, TValue&&) function.
 * @return no return value.
 */
template <size_t TMetaIdx = 0, typename T, typename TFunc, typename = typename std::enable_if<TIsStaticReflection<T>::value>::type>
void for_each_ex(T&& obj, TFunc&& func);

namespace details
{
	// 2022-07-22 20:06:19
	// fix copy cost. (...) => (T&&...)
	template <size_t TMetaIdx, size_t TIdx, typename... T>
	void for_each_aux_ex(T&&...)
	{ }

	template <size_t TMetaIdx, typename TValue, typename TFunc>
	void for_each_aux_ex_1(const char* name, TValue&& obj, TFunc&& func, std::false_type)
	{
#if defined(_WIN32) || defined(_WIN64)
		func.operator()<TMetaIdx>(name, std::forward<TValue>(obj));
#else
		func.template operator()<TMetaIdx>(name, std::forward<TValue>(obj));
#endif
	}

	template <size_t TMetaIdx, typename TValue, typename TFunc>
	void for_each_aux_ex_1(const char* name, TValue&& obj, TFunc&& func, std::true_type)
	{
		for_each_ex<TMetaIdx>(std::forward<TValue>(obj), std::forward<TFunc>(func));
	}

	template <
		size_t TMetaIdx, size_t TIdx, typename T, typename TFunc,
		typename = typename std::enable_if<(TIdx < std::decay<T>::type::member_count)>::type
	>
	void for_each_aux_ex(T&& obj, TFunc&& func)
	{
		using field_type = details::TField<T, TIdx>;
		field_type field{ obj };

		for_each_aux_ex_1<TMetaIdx>(field.name(), field.value(), std::forward<TFunc>(func), ::TIsStaticReflection<typename field_type::value_type>{});

		// 2022-07-22 20:06:19
		// fix copy cost. obj => std::forward<T>(obj)
		for_each_aux_ex<TMetaIdx + TMetaMemberCount<typename field_type::value_type>::value, TIdx + 1>(std::forward<T>(obj), std::forward<TFunc>(func));
	}
}

template <size_t TMetaIdx, typename T, typename TFunc, typename>
void for_each_ex(T&& obj, TFunc&& func)
{
	// 2022-07-22 20:06:19
	// fix copy cost. obj => std::forward<T>(obj)
	details::for_each_aux_ex<TMetaIdx, 0>(std::forward<T>(obj), std::forward<TFunc>(func));
}

#endif // !_STATIC_REFL_FUNCTION_H_