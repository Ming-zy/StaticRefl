#ifndef _STATIC_REFL_TYPE_TRAITS_H_
#define _STATIC_REFL_TYPE_TRAITS_H_

#include <cstdint>
#include <type_traits>

namespace details
{
    template <typename T, std::size_t TIdx> 
    struct TField;
	
    template <typename T, typename Enabler>
	struct TIsStaticReflectionAux;

	template <typename T, size_t TIdx>
	struct TMetaMemberCountAux;

	template <typename T, size_t TFIdx, size_t TCurMetaIdx, size_t TMetaIdx>
	struct TMetaIndexAux;
}

template <typename...>
struct TVoid
{
    using type = void;
};

template <typename TFrom, typename TTo>
struct TCopyQualifiers
{ 
    using type = TTo;
};

template <typename TFrom, typename TTo>
struct TCopyQualifiers<const TFrom, TTo>
{
    using type = typename std::add_const<TTo>::type;
};

template <typename TFrom, typename TTo>
struct TCopyQualifiers<volatile TFrom, TTo>
{
    using type = typename std::add_volatile<TTo>::type;
};

template <typename TFrom, typename TTo>
struct TCopyQualifiers<const volatile TFrom, TTo>
{
    using type = typename std::add_cv<TTo>::type;
};

template <typename TFrom, typename TTo>
struct TCopyQualifiers<TFrom&, TTo> : TCopyQualifiers<TFrom, TTo>
{ };

template <typename TFrom, typename TTo>
struct TCopyQualifiers<TFrom&&, TTo> : TCopyQualifiers<TFrom, TTo>
{ };

template <typename T>
struct TIsStaticReflection : details::TIsStaticReflectionAux<typename std::decay<T>::type, void>
{ };

template <typename T, size_t TIdx, typename TEnabler = void>
struct TValueType : std::enable_if<!TIsStaticReflection<T>::value, T>
{ };

template <typename T, size_t TIdx>
struct TValueType<T, TIdx, typename TVoid<decltype(std::declval<typename details::TField<T, TIdx>::value_type>())>::type>
{ 
	using type = typename details::TField<T, TIdx>::value_type;
};

template <typename T, typename = void>
struct TMemberCount
{
	static constexpr size_t value = 1;
};

template <typename T>
struct TMemberCount<T, typename std::enable_if<TIsStaticReflection<T>::value>::type>
{
	static constexpr size_t value = std::decay<T>::type::member_count;
};

template <typename T, typename = void>
struct TMetaMemberCount
{
	static constexpr size_t value = 1;
};

template <typename T>
struct TMetaMemberCount<T, typename std::enable_if<TIsStaticReflection<T>::value>::type> : details::TMetaMemberCountAux<T, std::decay<T>::type::member_count - 1>
{ };

struct FFieldGet
{
	template <
		size_t TIdx, typename T, 		
		typename = typename std::enable_if<(TIdx < std::decay<T>::type::member_count)>::type>
	static typename TValueType<T, TIdx>::type get_aux(T&& obj, std::true_type) noexcept
	{
		return details::TField<T, TIdx>(std::forward<T>(obj)).value();
	}

	template <size_t TIdx, typename T>
	static typename TValueType<T, TIdx>::type get_aux(T&& obj, std::false_type) noexcept
	{
		return std::forward<T>(obj);
	}

	template <size_t TIdx, typename T>
	static typename TValueType<T, TIdx>::type get(T&& obj) noexcept
	{
		return get_aux<TIdx>(std::forward<T>(obj), TIsStaticReflection<T>{ });
	} 
};

template <typename T, size_t TMetaIdx, typename = void>
struct TMetaIndex
{
	static constexpr size_t field_index = 0;
	static constexpr size_t meta_index  = 0;
	using value_type = T;
};

template <typename T, size_t TMetaIdx>
struct TMetaIndex<T, TMetaIdx, typename std::enable_if<(TIsStaticReflection<T>::value)>::type> : details::TMetaIndexAux<T, 0, 0, (TMetaIdx % TMetaMemberCount<T>::value)>
{ };

struct FMetaGet
{
	template <size_t TMetaIdx, typename T>
	static typename TMetaIndex<T, TMetaIdx>::value_type meta_get_aux(T&& obj, std::true_type) noexcept
	{
		using meta_index = TMetaIndex<T, TMetaIdx>;
		using field_type = details::TField<T, meta_index::field_index>;
		return meta_get<meta_index::meta_index>(field_type(obj).value());
	}

	template <size_t TMetaIdx, typename T>
	static typename TMetaIndex<T, TMetaIdx>::value_type meta_get_aux(T&& obj, std::false_type) noexcept
	{
		return std::forward<T>(obj);
	}

	template <size_t TMetaIdx, typename T>
	static typename TMetaIndex<T, TMetaIdx>::value_type meta_get(T&& obj) noexcept
	{
		return meta_get_aux<TMetaIdx>(std::forward<T>(obj), TIsStaticReflection<T>{ });
	}
};

struct NoReflectedTypeTag;

template <typename T>
struct TStaticReflectionTrait
{
	/* template <size_t TIdx>
	using field_type = typename std::conditional<
		TIsStaticReflection<T>::value,
		details::TField<T, TIdx>,
		NoReflectedTypeTag
	>::type; */

	template <size_t TIdx>
	using value_type = typename TValueType<T, TIdx>::type;

	template <size_t TMetaIdx>
	using meta_value_type = typename TMetaIndex<T, TMetaIdx>::value_type;

	static constexpr size_t meta_member_count = TMetaMemberCount<T>::value;
};

namespace details
{
    template <typename T, typename Enabler>
    struct TIsStaticReflectionAux : std::false_type
    { };

    template <typename T>
    struct TIsStaticReflectionAux<T, typename TVoid<decltype(T::member_count)>::type> : std::true_type
    { };

	template <typename T, size_t TIdx>
	struct TMetaMemberCountAux
	{
		using value_type = typename TValueType<T, TIdx>::type;
		static constexpr size_t value = TMetaMemberCount<value_type>::value + TMetaMemberCountAux<T, TIdx - 1>::value;
	};

	template <typename T>
	struct TMetaMemberCountAux<T, 0>
	{
		using value_type = typename TValueType<T, 0>::type;
		static constexpr size_t value = TMetaMemberCount<value_type>::value;
	};

#if defined(_WIN32) || defined(_WIN64)
	template <typename T, size_t TFIdx, size_t TCurMetaIdx, size_t TMetaIdx>
	struct TMetaIndexAux
	{
		static constexpr size_t field_index =
			((TCurMetaIdx + TMetaMemberCount<typename TValueType<T, TFIdx>::type>::value) > TMetaIdx) ?
			TFIdx :
			TMetaIndexAux<
				T, TFIdx + 1, 
				((TCurMetaIdx + TMetaMemberCount<typename TValueType<T, TFIdx>::type>::value) > TMetaIdx) ? 
					TMetaIdx : (TCurMetaIdx + TMetaMemberCount<typename TValueType<T, TFIdx>::type>::value), 
				TMetaIdx>::field_index;

		static constexpr size_t meta_index =
			((TCurMetaIdx + TMetaMemberCount<typename TValueType<T, TFIdx>::type>::value) > TMetaIdx) ?
			TMetaIdx - TCurMetaIdx :
			TMetaIndexAux<
				T, TFIdx + 1, 
				((TCurMetaIdx + TMetaMemberCount<typename TValueType<T, TFIdx>::type>::value) > TMetaIdx) ?
					TMetaIdx : (TCurMetaIdx + TMetaMemberCount<typename TValueType<T, TFIdx>::type>::value),
				TMetaIdx>::meta_index;

		// using field_type = details::TField<T, field_index>;
		using value_type = typename TMetaIndex<typename details::TField<T, field_index>::value_type, meta_index>::value_type;
	};

	template <typename T, size_t TFIdx>
	struct TMetaIndexAux<T, TFIdx, 0, 0>
	{
		static constexpr size_t field_index = TFIdx;
		static constexpr size_t meta_index = 0;
		// using field_type = details::TField<T, field_index>;
		using value_type = typename TMetaIndex<typename details::TField<T, field_index>::value_type, meta_index>::value_type;
	};

	template <typename T, size_t TFIdx, size_t TMetaIdx>
	struct TMetaIndexAux<T, TFIdx, TMetaIdx, TMetaIdx>
	{
		static constexpr size_t field_index = TFIdx;
		static constexpr size_t meta_index = 0;
	};
#else
	template <typename T, size_t TFIdx, size_t TCurMetaIdx, size_t TMetaIdx>
	struct TMetaIndexAux
	{
		static constexpr size_t field_index = 
			((TCurMetaIdx + TMetaMemberCount<typename TValueType<T, TFIdx>::type>::value) > TMetaIdx) ? 
			TFIdx : 
			TMetaIndexAux<T, TFIdx + 1, TCurMetaIdx + TMetaMemberCount<typename TValueType<T, TFIdx>::type>::value, TMetaIdx>::field_index;
		
		static constexpr size_t meta_index = 
			((TCurMetaIdx + TMetaMemberCount<typename TValueType<T, TFIdx>::type>::value) > TMetaIdx) ? 
			TMetaIdx -  TCurMetaIdx: 
			TMetaIndexAux<T, TFIdx + 1, TCurMetaIdx + TMetaMemberCount<typename TValueType<T, TFIdx>::type>::value, TMetaIdx>::meta_index;

		// using field_type = details::TField<T, field_index>;
		using value_type = typename TMetaIndex<typename details::TField<T, field_index>::value_type, meta_index>::value_type;
	};

	template <typename T, size_t TCurMetaIdx, size_t TMetaIdx>
	struct TMetaIndexAux<T, std::decay<T>::type::member_count, TCurMetaIdx, TMetaIdx>
	{ 
		static constexpr size_t field_index = std::decay<T>::type::member_count;
		static constexpr size_t meta_index = 0;
	};
#endif 
}

#endif // !_STATIC_REFL_TYPE_TRAITS_H_