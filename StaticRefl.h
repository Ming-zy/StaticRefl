#ifndef _STATIC_REFLECTION_H_
#define _STATIC_REFLECTION_H_

#include <type_traits>
#include <cstdint>

#include "TypeTraits.h"

namespace details
{
    template <typename T, std::size_t TIdx>
    struct TField;
}

#define MEMBER_COUNT_INTERNAL(                                                      \
    _1,  _2,  _3,  _4,  _5,  _6,  _7,  _8,  _9,  _10, _11, _12, _13, _14, _15, _16, \
    _17, _18, _19, _20, _21, _22, _23, _24, _25, _26, _27, _28, _29, _30, _31, _32, \
    _33, _34, _35, _36, _37, _38, _39, _40, _41, _42, _43, _44, _45, _46, _47, _48, \
    _49, _50, _51, _52, _53, _54, _55, _56, _57, _58, _59, _60, _61, _62, _63, _64, \
    n, ...)                                                                         \
    n                                                                               \
// ...

// MSVC++ variadic macro expansion 
//   [https://stackoverflow.com/questions/9183993/msvc-variadic-macro-expansion]
// MSVC++ preprocessor bug 
//   [https://developercommunity.visualstudio.com/t/-va-args-seems-to-be-trated-as-a-single-parameter/460154]
#if defined(_WIN32) || defined(_WIN64)

#define MEMBER_COUNT(...)	                                            \
	INTERNAL_EXPAND_ARGS_PRIVATE(INTERNAL_ARGS_AUGMENTER(__VA_ARGS__))	\
// ...

#define INTERNAL_ARGS_AUGMENTER(...) unused, __VA_ARGS__

#define INTERNAL_EXPAND(x) x

#define INTERNAL_EXPAND_ARGS_PRIVATE(...)								 \
	INTERNAL_EXPAND(MEMBER_COUNT_INTERNAL(__VA_ARGS__				     \
		64, 63, 62, 61, 60, 59, 58, 57, 56, 55, 54, 53, 52, 51, 50, 49,  \
		48, 47, 46, 45, 44, 43, 42, 41, 40, 39, 38, 37, 36, 35, 34, 33,	 \
		32, 31, 30, 29, 28, 27, 26, 25, 24, 23, 22, 21, 20, 19, 18, 17,	 \
		16, 15, 14, 13, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1,			 \
		0)																)\
// ...

#else
#define MEMBER_COUNT(...)                                               \
    MEMBER_COUNT_INTERNAL(__VA_ARGS__,                                  \
        64, 63, 62, 61, 60, 59, 58, 57, 56, 55, 54, 53, 52, 51, 50, 49, \
        48, 47, 46, 45, 44, 43, 42, 41, 40, 39, 38, 37, 36, 35, 34, 33, \
        32, 31, 30, 29, 28, 27, 26, 25, 24, 23, 22, 21, 20, 19, 18, 17, \
        16, 15, 14, 13, 12, 11, 10,  9,  8,  7,  6,  5,  4,  3,  2,  1, \
        0) \
// ...
#endif 

#define PARE(...) __VA_ARGS__
// PARE (double) x => doube x
#define PAIR(x) PARE x
// EAT (double) x => x
#define EAT(x) 
// x => "x"
#define STR(x) #x

#define STRIP(x) EAT x
#define STRING(x) STR(x)

#define TO_STRING(x) STRING(STRIP(x))

#define CONCAT(x, y) x ## y
#define NAME_CONCAT(x, y) x "." y
#define PASTE(x, y) CONCAT(x, y)

#define FIELD_EACH(type, i, arg)                    \
    FIELD_EACH_IMPL(type, i, arg)                   \
    FIELD_EACH_IMPL(type&, i, arg)                  \
    FIELD_EACH_IMPL(type&&, i, arg)                 \
    FIELD_EACH_IMPL(const type, i, arg)             \
    FIELD_EACH_IMPL(const type&, i, arg)            \
    FIELD_EACH_IMPL(const type&&, i, arg)           \
    FIELD_EACH_IMPL(volatile type, i, arg)          \
    FIELD_EACH_IMPL(volatile type&, i, arg)         \
    FIELD_EACH_IMPL(volatile type&&, i, arg)        \
    FIELD_EACH_IMPL(const volatile type, i, arg)    \
    FIELD_EACH_IMPL(const volatile type&, i, arg)   \
    FIELD_EACH_IMPL(const volatile type&&, i, arg)  \
// ...

// MSVC++ variadic macro expansion 
//   [https://stackoverflow.com/questions/9183993/msvc-variadic-macro-expansion]
// MSVC++ preprocessor bug 
//   [https://developercommunity.visualstudio.com/t/-va-args-seems-to-be-trated-as-a-single-parameter/460154]
#if defined(_WIN32) || defined(_WIN64)

// perfect forwarding MSCV __VA_ARGS__
#define ARGS_FORWARD(func, args) func args
// ...
// perfect forwarding MSCV __VA_ARGS__
#define EXPAND_ARGS_BY_FUNC(func, args) func args 
// ...
#define TO_STRING_WIN(x) EXPAND_ARGS_BY_FUNC(STRING, (STRIP(x)))
// ...

#define FIELD_EACH_IMPL(st_type, i, arg)                                                    \
namespace details {                                                                         \
    template <>                                                                             \
    struct TField<st_type, i>                                                               \
    {                                                                                       \
        using field_type = TField<st_type, i>;                                              \
        using base_type = st_type;                                                          \
        using value_type = typename std::add_lvalue_reference<typename TCopyQualifiers<base_type, decltype(std::decay<st_type>::type::STRIP(arg))>::type>::type;  \
        using reference_type = typename std::add_lvalue_reference<base_type>::type;         \
        reference_type obj;                                                                 \
        TField(reference_type o) : obj(o) { }                                               \
        value_type value() noexcept                                                         \
	    { return obj.STRIP(arg); }                                                          \
        constexpr static const char* name() noexcept                                        \
        { return TO_STRING_WIN(arg) + 1; }                                                  \
        constexpr static const char* super_name() noexcept                                  \
        { return STR(st_type); }                                                            \
        constexpr static const char* full_name() noexcept                                   \
        { return NAME_CONCAT(STR(st_type), TO_STRING_WIN(arg)); }                           \
    };                                                                                      \
}                                                                                           \
// ...

#define ARG_0()
#define ARG_1(arg)		 PAIR(arg)
#define ARG_2(arg, ...)  PAIR(arg); ARGS_FORWARD(ARG_1,  (__VA_ARGS__))
#define ARG_3(arg, ...)  PAIR(arg); ARGS_FORWARD(ARG_2,  (__VA_ARGS__))
#define ARG_4(arg, ...)  PAIR(arg); ARGS_FORWARD(ARG_3,  (__VA_ARGS__))
#define ARG_5(arg, ...)  PAIR(arg); ARGS_FORWARD(ARG_4,  (__VA_ARGS__))
#define ARG_6(arg, ...)  PAIR(arg); ARGS_FORWARD(ARG_5,  (__VA_ARGS__))
#define ARG_7(arg, ...)  PAIR(arg); ARGS_FORWARD(ARG_6,  (__VA_ARGS__))
#define ARG_8(arg, ...)  PAIR(arg); ARGS_FORWARD(ARG_7,  (__VA_ARGS__))
#define ARG_9(arg, ...)  PAIR(arg); ARGS_FORWARD(ARG_8,  (__VA_ARGS__))
#define ARG_10(arg, ...) PAIR(arg); ARGS_FORWARD(ARG_9,  (__VA_ARGS__))
#define ARG_11(arg, ...) PAIR(arg); ARGS_FORWARD(ARG_10, (__VA_ARGS__))
#define ARG_12(arg, ...) PAIR(arg); ARGS_FORWARD(ARG_11, (__VA_ARGS__))
#define ARG_13(arg, ...) PAIR(arg); ARGS_FORWARD(ARG_12, (__VA_ARGS__))
#define ARG_14(arg, ...) PAIR(arg); ARGS_FORWARD(ARG_13, (__VA_ARGS__))
#define ARG_15(arg, ...) PAIR(arg); ARGS_FORWARD(ARG_14, (__VA_ARGS__))
#define ARG_16(arg, ...) PAIR(arg); ARGS_FORWARD(ARG_15, (__VA_ARGS__))
#define ARG_17(arg, ...) PAIR(arg); ARGS_FORWARD(ARG_16, (__VA_ARGS__))
#define ARG_18(arg, ...) PAIR(arg); ARGS_FORWARD(ARG_17, (__VA_ARGS__))
#define ARG_19(arg, ...) PAIR(arg); ARGS_FORWARD(ARG_18, (__VA_ARGS__))
#define ARG_20(arg, ...) PAIR(arg); ARGS_FORWARD(ARG_19, (__VA_ARGS__))
#define ARG_21(arg, ...) PAIR(arg); ARGS_FORWARD(ARG_20, (__VA_ARGS__))
#define ARG_22(arg, ...) PAIR(arg); ARGS_FORWARD(ARG_21, (__VA_ARGS__))
#define ARG_23(arg, ...) PAIR(arg); ARGS_FORWARD(ARG_22, (__VA_ARGS__))
#define ARG_24(arg, ...) PAIR(arg); ARGS_FORWARD(ARG_23, (__VA_ARGS__))
#define ARG_25(arg, ...) PAIR(arg); ARGS_FORWARD(ARG_24, (__VA_ARGS__))
#define ARG_26(arg, ...) PAIR(arg); ARGS_FORWARD(ARG_25, (__VA_ARGS__))
#define ARG_27(arg, ...) PAIR(arg); ARGS_FORWARD(ARG_26, (__VA_ARGS__))
#define ARG_28(arg, ...) PAIR(arg); ARGS_FORWARD(ARG_27, (__VA_ARGS__))
#define ARG_29(arg, ...) PAIR(arg); ARGS_FORWARD(ARG_28, (__VA_ARGS__))
#define ARG_30(arg, ...) PAIR(arg); ARGS_FORWARD(ARG_29, (__VA_ARGS__))
#define ARG_31(arg, ...) PAIR(arg); ARGS_FORWARD(ARG_30, (__VA_ARGS__))
#define ARG_32(arg, ...) PAIR(arg); ARGS_FORWARD(ARG_31, (__VA_ARGS__))
#define ARG_33(arg, ...) PAIR(arg); ARGS_FORWARD(ARG_32, (__VA_ARGS__))
#define ARG_34(arg, ...) PAIR(arg); ARGS_FORWARD(ARG_33, (__VA_ARGS__))
#define ARG_35(arg, ...) PAIR(arg); ARGS_FORWARD(ARG_34, (__VA_ARGS__))
#define ARG_36(arg, ...) PAIR(arg); ARGS_FORWARD(ARG_35, (__VA_ARGS__))
#define ARG_37(arg, ...) PAIR(arg); ARGS_FORWARD(ARG_36, (__VA_ARGS__))
#define ARG_38(arg, ...) PAIR(arg); ARGS_FORWARD(ARG_37, (__VA_ARGS__))
#define ARG_39(arg, ...) PAIR(arg); ARGS_FORWARD(ARG_38, (__VA_ARGS__))
#define ARG_40(arg, ...) PAIR(arg); ARGS_FORWARD(ARG_39, (__VA_ARGS__))
#define ARG_41(arg, ...) PAIR(arg); ARGS_FORWARD(ARG_40, (__VA_ARGS__))
#define ARG_42(arg, ...) PAIR(arg); ARGS_FORWARD(ARG_41, (__VA_ARGS__))
#define ARG_43(arg, ...) PAIR(arg); ARGS_FORWARD(ARG_42, (__VA_ARGS__))
#define ARG_44(arg, ...) PAIR(arg); ARGS_FORWARD(ARG_43, (__VA_ARGS__))
#define ARG_45(arg, ...) PAIR(arg); ARGS_FORWARD(ARG_44, (__VA_ARGS__))
#define ARG_46(arg, ...) PAIR(arg); ARGS_FORWARD(ARG_45, (__VA_ARGS__))
#define ARG_47(arg, ...) PAIR(arg); ARGS_FORWARD(ARG_46, (__VA_ARGS__))
#define ARG_48(arg, ...) PAIR(arg); ARGS_FORWARD(ARG_47, (__VA_ARGS__))
#define ARG_49(arg, ...) PAIR(arg); ARGS_FORWARD(ARG_48, (__VA_ARGS__))
#define ARG_50(arg, ...) PAIR(arg); ARGS_FORWARD(ARG_49, (__VA_ARGS__))
#define ARG_51(arg, ...) PAIR(arg); ARGS_FORWARD(ARG_50, (__VA_ARGS__))
#define ARG_52(arg, ...) PAIR(arg); ARGS_FORWARD(ARG_51, (__VA_ARGS__))
#define ARG_53(arg, ...) PAIR(arg); ARGS_FORWARD(ARG_52, (__VA_ARGS__))
#define ARG_54(arg, ...) PAIR(arg); ARGS_FORWARD(ARG_53, (__VA_ARGS__))
#define ARG_55(arg, ...) PAIR(arg); ARGS_FORWARD(ARG_54, (__VA_ARGS__))
#define ARG_56(arg, ...) PAIR(arg); ARGS_FORWARD(ARG_55, (__VA_ARGS__))
#define ARG_57(arg, ...) PAIR(arg); ARGS_FORWARD(ARG_56, (__VA_ARGS__))
#define ARG_58(arg, ...) PAIR(arg); ARGS_FORWARD(ARG_57, (__VA_ARGS__))
#define ARG_59(arg, ...) PAIR(arg); ARGS_FORWARD(ARG_58, (__VA_ARGS__))
#define ARG_60(arg, ...) PAIR(arg); ARGS_FORWARD(ARG_59, (__VA_ARGS__))
#define ARG_61(arg, ...) PAIR(arg); ARGS_FORWARD(ARG_60, (__VA_ARGS__))
#define ARG_62(arg, ...) PAIR(arg); ARGS_FORWARD(ARG_61, (__VA_ARGS__))
#define ARG_63(arg, ...) PAIR(arg); ARGS_FORWARD(ARG_62, (__VA_ARGS__))
#define ARG_64(arg, ...) PAIR(arg); ARGS_FORWARD(ARG_63, (__VA_ARGS__))

#define FOR_EACH_0(func, type, i, arg)
#define FOR_EACH_1(func, type, i, arg)         func(type, i, arg)
#define FOR_EACH_2(func, type, i, arg, ...)    func(type, i, arg); ARGS_FORWARD(FOR_EACH_1,  (func, type, i + 1, __VA_ARGS__)) 
#define FOR_EACH_3(func, type, i, arg, ...)    func(type, i, arg); ARGS_FORWARD(FOR_EACH_2,  (func, type, i + 1, __VA_ARGS__))
#define FOR_EACH_4(func, type, i, arg, ...)    func(type, i, arg); ARGS_FORWARD(FOR_EACH_3,  (func, type, i + 1, __VA_ARGS__))
#define FOR_EACH_5(func, type, i, arg, ...)    func(type, i, arg); ARGS_FORWARD(FOR_EACH_4,  (func, type, i + 1, __VA_ARGS__))
#define FOR_EACH_6(func, type, i, arg, ...)    func(type, i, arg); ARGS_FORWARD(FOR_EACH_5,  (func, type, i + 1, __VA_ARGS__))
#define FOR_EACH_7(func, type, i, arg, ...)    func(type, i, arg); ARGS_FORWARD(FOR_EACH_6,  (func, type, i + 1, __VA_ARGS__))
#define FOR_EACH_8(func, type, i, arg, ...)    func(type, i, arg); ARGS_FORWARD(FOR_EACH_7,  (func, type, i + 1, __VA_ARGS__))
#define FOR_EACH_9(func, type, i, arg, ...)    func(type, i, arg); ARGS_FORWARD(FOR_EACH_8,  (func, type, i + 1, __VA_ARGS__))
#define FOR_EACH_10(func, type, i, arg, ...)   func(type, i, arg); ARGS_FORWARD(FOR_EACH_9,  (func, type, i + 1, __VA_ARGS__))
#define FOR_EACH_11(func, type, i, arg, ...)   func(type, i, arg); ARGS_FORWARD(FOR_EACH_10, (func, type, i + 1, __VA_ARGS__))
#define FOR_EACH_12(func, type, i, arg, ...)   func(type, i, arg); ARGS_FORWARD(FOR_EACH_11, (func, type, i + 1, __VA_ARGS__))
#define FOR_EACH_13(func, type, i, arg, ...)   func(type, i, arg); ARGS_FORWARD(FOR_EACH_12, (func, type, i + 1, __VA_ARGS__))
#define FOR_EACH_14(func, type, i, arg, ...)   func(type, i, arg); ARGS_FORWARD(FOR_EACH_13, (func, type, i + 1, __VA_ARGS__))
#define FOR_EACH_15(func, type, i, arg, ...)   func(type, i, arg); ARGS_FORWARD(FOR_EACH_14, (func, type, i + 1, __VA_ARGS__))
#define FOR_EACH_16(func, type, i, arg, ...)   func(type, i, arg); ARGS_FORWARD(FOR_EACH_15, (func, type, i + 1, __VA_ARGS__))
#define FOR_EACH_17(func, type, i, arg, ...)   func(type, i, arg); ARGS_FORWARD(FOR_EACH_16, (func, type, i + 1, __VA_ARGS__))
#define FOR_EACH_18(func, type, i, arg, ...)   func(type, i, arg); ARGS_FORWARD(FOR_EACH_17, (func, type, i + 1, __VA_ARGS__))
#define FOR_EACH_19(func, type, i, arg, ...)   func(type, i, arg); ARGS_FORWARD(FOR_EACH_18, (func, type, i + 1, __VA_ARGS__))
#define FOR_EACH_20(func, type, i, arg, ...)   func(type, i, arg); ARGS_FORWARD(FOR_EACH_19, (func, type, i + 1, __VA_ARGS__))
#define FOR_EACH_21(func, type, i, arg, ...)   func(type, i, arg); ARGS_FORWARD(FOR_EACH_20, (func, type, i + 1, __VA_ARGS__))
#define FOR_EACH_22(func, type, i, arg, ...)   func(type, i, arg); ARGS_FORWARD(FOR_EACH_21, (func, type, i + 1, __VA_ARGS__))
#define FOR_EACH_23(func, type, i, arg, ...)   func(type, i, arg); ARGS_FORWARD(FOR_EACH_22, (func, type, i + 1, __VA_ARGS__))
#define FOR_EACH_24(func, type, i, arg, ...)   func(type, i, arg); ARGS_FORWARD(FOR_EACH_23, (func, type, i + 1, __VA_ARGS__))
#define FOR_EACH_25(func, type, i, arg, ...)   func(type, i, arg); ARGS_FORWARD(FOR_EACH_24, (func, type, i + 1, __VA_ARGS__))
#define FOR_EACH_26(func, type, i, arg, ...)   func(type, i, arg); ARGS_FORWARD(FOR_EACH_25, (func, type, i + 1, __VA_ARGS__))
#define FOR_EACH_27(func, type, i, arg, ...)   func(type, i, arg); ARGS_FORWARD(FOR_EACH_26, (func, type, i + 1, __VA_ARGS__))
#define FOR_EACH_28(func, type, i, arg, ...)   func(type, i, arg); ARGS_FORWARD(FOR_EACH_27, (func, type, i + 1, __VA_ARGS__))
#define FOR_EACH_29(func, type, i, arg, ...)   func(type, i, arg); ARGS_FORWARD(FOR_EACH_28, (func, type, i + 1, __VA_ARGS__))
#define FOR_EACH_30(func, type, i, arg, ...)   func(type, i, arg); ARGS_FORWARD(FOR_EACH_29, (func, type, i + 1, __VA_ARGS__))
#define FOR_EACH_31(func, type, i, arg, ...)   func(type, i, arg); ARGS_FORWARD(FOR_EACH_30, (func, type, i + 1, __VA_ARGS__))
#define FOR_EACH_32(func, type, i, arg, ...)   func(type, i, arg); ARGS_FORWARD(FOR_EACH_31, (func, type, i + 1, __VA_ARGS__))
#define FOR_EACH_33(func, type, i, arg, ...)   func(type, i, arg); ARGS_FORWARD(FOR_EACH_32, (func, type, i + 1, __VA_ARGS__))
#define FOR_EACH_34(func, type, i, arg, ...)   func(type, i, arg); ARGS_FORWARD(FOR_EACH_33, (func, type, i + 1, __VA_ARGS__))
#define FOR_EACH_35(func, type, i, arg, ...)   func(type, i, arg); ARGS_FORWARD(FOR_EACH_34, (func, type, i + 1, __VA_ARGS__))
#define FOR_EACH_36(func, type, i, arg, ...)   func(type, i, arg); ARGS_FORWARD(FOR_EACH_35, (func, type, i + 1, __VA_ARGS__))
#define FOR_EACH_37(func, type, i, arg, ...)   func(type, i, arg); ARGS_FORWARD(FOR_EACH_36, (func, type, i + 1, __VA_ARGS__))
#define FOR_EACH_38(func, type, i, arg, ...)   func(type, i, arg); ARGS_FORWARD(FOR_EACH_37, (func, type, i + 1, __VA_ARGS__))
#define FOR_EACH_39(func, type, i, arg, ...)   func(type, i, arg); ARGS_FORWARD(FOR_EACH_38, (func, type, i + 1, __VA_ARGS__))
#define FOR_EACH_40(func, type, i, arg, ...)   func(type, i, arg); ARGS_FORWARD(FOR_EACH_39, (func, type, i + 1, __VA_ARGS__))
#define FOR_EACH_41(func, type, i, arg, ...)   func(type, i, arg); ARGS_FORWARD(FOR_EACH_40, (func, type, i + 1, __VA_ARGS__))
#define FOR_EACH_42(func, type, i, arg, ...)   func(type, i, arg); ARGS_FORWARD(FOR_EACH_41, (func, type, i + 1, __VA_ARGS__))
#define FOR_EACH_43(func, type, i, arg, ...)   func(type, i, arg); ARGS_FORWARD(FOR_EACH_42, (func, type, i + 1, __VA_ARGS__))
#define FOR_EACH_44(func, type, i, arg, ...)   func(type, i, arg); ARGS_FORWARD(FOR_EACH_43, (func, type, i + 1, __VA_ARGS__))
#define FOR_EACH_45(func, type, i, arg, ...)   func(type, i, arg); ARGS_FORWARD(FOR_EACH_44, (func, type, i + 1, __VA_ARGS__))
#define FOR_EACH_46(func, type, i, arg, ...)   func(type, i, arg); ARGS_FORWARD(FOR_EACH_45, (func, type, i + 1, __VA_ARGS__))
#define FOR_EACH_47(func, type, i, arg, ...)   func(type, i, arg); ARGS_FORWARD(FOR_EACH_46, (func, type, i + 1, __VA_ARGS__))
#define FOR_EACH_48(func, type, i, arg, ...)   func(type, i, arg); ARGS_FORWARD(FOR_EACH_47, (func, type, i + 1, __VA_ARGS__))
#define FOR_EACH_49(func, type, i, arg, ...)   func(type, i, arg); ARGS_FORWARD(FOR_EACH_48, (func, type, i + 1, __VA_ARGS__))
#define FOR_EACH_50(func, type, i, arg, ...)   func(type, i, arg); ARGS_FORWARD(FOR_EACH_49, (func, type, i + 1, __VA_ARGS__))
#define FOR_EACH_51(func, type, i, arg, ...)   func(type, i, arg); ARGS_FORWARD(FOR_EACH_50, (func, type, i + 1, __VA_ARGS__))
#define FOR_EACH_52(func, type, i, arg, ...)   func(type, i, arg); ARGS_FORWARD(FOR_EACH_51, (func, type, i + 1, __VA_ARGS__))
#define FOR_EACH_53(func, type, i, arg, ...)   func(type, i, arg); ARGS_FORWARD(FOR_EACH_52, (func, type, i + 1, __VA_ARGS__))
#define FOR_EACH_54(func, type, i, arg, ...)   func(type, i, arg); ARGS_FORWARD(FOR_EACH_53, (func, type, i + 1, __VA_ARGS__))
#define FOR_EACH_55(func, type, i, arg, ...)   func(type, i, arg); ARGS_FORWARD(FOR_EACH_54, (func, type, i + 1, __VA_ARGS__))
#define FOR_EACH_56(func, type, i, arg, ...)   func(type, i, arg); ARGS_FORWARD(FOR_EACH_55, (func, type, i + 1, __VA_ARGS__))
#define FOR_EACH_57(func, type, i, arg, ...)   func(type, i, arg); ARGS_FORWARD(FOR_EACH_56, (func, type, i + 1, __VA_ARGS__))
#define FOR_EACH_58(func, type, i, arg, ...)   func(type, i, arg); ARGS_FORWARD(FOR_EACH_57, (func, type, i + 1, __VA_ARGS__))
#define FOR_EACH_59(func, type, i, arg, ...)   func(type, i, arg); ARGS_FORWARD(FOR_EACH_58, (func, type, i + 1, __VA_ARGS__))
#define FOR_EACH_60(func, type, i, arg, ...)   func(type, i, arg); ARGS_FORWARD(FOR_EACH_59, (func, type, i + 1, __VA_ARGS__))
#define FOR_EACH_61(func, type, i, arg, ...)   func(type, i, arg); ARGS_FORWARD(FOR_EACH_60, (func, type, i + 1, __VA_ARGS__))
#define FOR_EACH_62(func, type, i, arg, ...)   func(type, i, arg); ARGS_FORWARD(FOR_EACH_61, (func, type, i + 1, __VA_ARGS__))
#define FOR_EACH_63(func, type, i, arg, ...)   func(type, i, arg); ARGS_FORWARD(FOR_EACH_62, (func, type, i + 1, __VA_ARGS__))
#define FOR_EACH_64(func, type, i, arg, ...)   func(type, i, arg); ARGS_FORWARD(FOR_EACH_63, (func, type, i + 1, __VA_ARGS__))

#define DEFINE_STRUCT(type_name, ...)                                                                                                           \
struct type_name                                                                                                                                \
{                                                                                                                                               \
    static constexpr size_t member_count = MEMBER_COUNT(__VA_ARGS__);                                                                           \
    EXPAND_ARGS_BY_FUNC(PASTE(ARG_, MEMBER_COUNT(__VA_ARGS__)), (__VA_ARGS__));                                                                 \
    template <size_t TIdx>                                                                                                                      \
    typename TValueType<type_name, TIdx>::type get() noexcept                                                                                   \
    { return FFieldGet::template get<TIdx>(*this); }                                                                                            \
    template <size_t TIdx>                                                                                                                      \
    typename TValueType<const type_name, TIdx>::type get() const noexcept                                                                       \
    { return FFieldGet::template get<TIdx>(*this); }                                                                                            \
    template <size_t TMetaIdx>                                                                                                                  \
    typename TMetaIndex<type_name, TMetaIdx>::value_type meta_get() noexcept                                                                    \
    { return FMetaGet::template meta_get<TMetaIdx>(*this); }                                                                                    \
    template <size_t TMetaIdx>                                                                                                                  \
    typename TMetaIndex<const type_name, TMetaIdx>::value_type meta_get() const noexcept                                                        \
    { return FMetaGet::template meta_get<TMetaIdx>(*this); }                                                                                    \
    template <size_t TIdx, typename T,                                                                                                          \
        typename = typename std::enable_if<                                                                                                     \
            std::is_constructible<typename std::remove_reference<typename TValueType<type_name, TIdx>::type>::type, T>::value                   \
        >::type>                                                                                                                                \
    void set(T&& value)                                                                                                                         \
    { get<TIdx>() = std::forward<T>(value); }                                                                                                   \
    template <size_t TIdx, typename... TArgs,                                                                                                   \
        typename = typename std::enable_if<                                                                                                     \
            std::is_constructible<typename std::remove_reference<typename TValueType<type_name, TIdx>::type>::type, TArgs...>::value            \
        >::type>                                                                                                                                \
    void set(TArgs&&... args)                                                                                                                   \
    {                                                                                                                                           \
        using type = typename std::decay<typename TValueType<type_name, TIdx>::type>::type;                                                     \
        get<TIdx>() = type(std::forward<TArgs>(args)...);                                                                                       \
    }                                                                                                                                           \
    template <size_t TMetaIdx, typename T,                                                                                                      \
        typename = typename std::enable_if<                                                                                                     \
            std::is_constructible<typename std::remove_reference<typename TMetaIndex<type_name, TMetaIdx>::value_type>::type, T>::value         \
        >::type>                                                                                                                                \
    void meta_set(T&& value)                                                                                                                    \
    { meta_get<TMetaIdx>() = std::forward<T>(value); }                                                                                          \
    template <size_t TMetaIdx, typename... TArgs,                                                                                               \
        typename = typename std::enable_if<                                                                                                     \
            std::is_constructible<typename std::remove_reference<typename TMetaIndex<type_name, TMetaIdx>::value_type>::type, TArgs...>::value  \
        >::type>                                                                                                                                \
    void meta_set(TArgs&&... args)                                                                                                              \
    {                                                                                                                                           \
        using type = typename std::decay<typename TMetaIndex<type_name, TMetaIdx>::value_type>::type;                                           \
        meta_get<TMetaIdx>() = type(std::forward<TArgs>(args)...);                                                                              \
    }                                                                                                                                           \
};                                                                                                                                              \
EXPAND_ARGS_BY_FUNC(PASTE(FOR_EACH_, MEMBER_COUNT(__VA_ARGS__)), (FIELD_EACH, type_name, 0, __VA_ARGS__));                                      \
// ...

#else

#define ARG_EACH(type, i, arg)  \
    PAIR(arg)                   \
// ...

#define FIELD_EACH_IMPL(st_type, i, arg)                                                    \
namespace details {                                                                         \
    template <>                                                                             \
    struct TField<st_type, i>                                                               \
    {                                                                                       \
        using field_type = TField<st_type, i>;                                              \
        using base_type = st_type;                                                          \
        using value_type = typename std::add_lvalue_reference<typename TCopyQualifiers<base_type, decltype(std::decay<st_type>::type::STRIP(arg))>::type>::type;  \
        using reference_type = typename std::add_lvalue_reference<base_type>::type;         \
        reference_type obj;                                                                 \
        TField(reference_type o) : obj(o) { }                                               \
        value_type value() noexcept                                                         \
	    { return obj.STRIP(arg); }                                                          \
        constexpr static const char* name() noexcept                                        \
        { return TO_STRING(arg); }                                                      \
        constexpr static const char* super_name() noexcept                                  \
        { return STR(st_type); }                                                            \
        constexpr static const char* full_name() noexcept                                   \
        { return NAME_CONCAT(STR(st_type), TO_STRING(arg)); }                               \
    };                                                                                      \
}                                                                                           \
// ...

#define FOR_EACH_0(func, type, i, arg)
#define FOR_EACH_1(func, type, i, arg)         func(type, i, arg)
#define FOR_EACH_2(func, type, i, arg, ...)    func(type, i, arg); FOR_EACH_1(func,  type, i + 1, __VA_ARGS__) 
#define FOR_EACH_3(func, type, i, arg, ...)    func(type, i, arg); FOR_EACH_2(func,  type, i + 1, __VA_ARGS__)
#define FOR_EACH_4(func, type, i, arg, ...)    func(type, i, arg); FOR_EACH_3(func,  type, i + 1, __VA_ARGS__)
#define FOR_EACH_5(func, type, i, arg, ...)    func(type, i, arg); FOR_EACH_4(func,  type, i + 1, __VA_ARGS__)
#define FOR_EACH_6(func, type, i, arg, ...)    func(type, i, arg); FOR_EACH_5(func,  type, i + 1, __VA_ARGS__)
#define FOR_EACH_7(func, type, i, arg, ...)    func(type, i, arg); FOR_EACH_6(func,  type, i + 1, __VA_ARGS__)
#define FOR_EACH_8(func, type, i, arg, ...)    func(type, i, arg); FOR_EACH_7(func,  type, i + 1, __VA_ARGS__)
#define FOR_EACH_9(func, type, i, arg, ...)    func(type, i, arg); FOR_EACH_8(func,  type, i + 1, __VA_ARGS__)
#define FOR_EACH_10(func, type, i, arg, ...)   func(type, i, arg); FOR_EACH_9(func,  type, i + 1, __VA_ARGS__)
#define FOR_EACH_11(func, type, i, arg, ...)   func(type, i, arg); FOR_EACH_10(func, type, i + 1, __VA_ARGS__)
#define FOR_EACH_12(func, type, i, arg, ...)   func(type, i, arg); FOR_EACH_11(func, type, i + 1, __VA_ARGS__)
#define FOR_EACH_13(func, type, i, arg, ...)   func(type, i, arg); FOR_EACH_12(func, type, i + 1, __VA_ARGS__)
#define FOR_EACH_14(func, type, i, arg, ...)   func(type, i, arg); FOR_EACH_13(func, type, i + 1, __VA_ARGS__)
#define FOR_EACH_15(func, type, i, arg, ...)   func(type, i, arg); FOR_EACH_14(func, type, i + 1, __VA_ARGS__)
#define FOR_EACH_16(func, type, i, arg, ...)   func(type, i, arg); FOR_EACH_15(func, type, i + 1, __VA_ARGS__)
#define FOR_EACH_17(func, type, i, arg, ...)   func(type, i, arg); FOR_EACH_16(func, type, i + 1, __VA_ARGS__)
#define FOR_EACH_18(func, type, i, arg, ...)   func(type, i, arg); FOR_EACH_17(func, type, i + 1, __VA_ARGS__)
#define FOR_EACH_19(func, type, i, arg, ...)   func(type, i, arg); FOR_EACH_18(func, type, i + 1, __VA_ARGS__)
#define FOR_EACH_20(func, type, i, arg, ...)   func(type, i, arg); FOR_EACH_19(func, type, i + 1, __VA_ARGS__)
#define FOR_EACH_21(func, type, i, arg, ...)   func(type, i, arg); FOR_EACH_20(func, type, i + 1, __VA_ARGS__)
#define FOR_EACH_22(func, type, i, arg, ...)   func(type, i, arg); FOR_EACH_21(func, type, i + 1, __VA_ARGS__)
#define FOR_EACH_23(func, type, i, arg, ...)   func(type, i, arg); FOR_EACH_22(func, type, i + 1, __VA_ARGS__)
#define FOR_EACH_24(func, type, i, arg, ...)   func(type, i, arg); FOR_EACH_23(func, type, i + 1, __VA_ARGS__)
#define FOR_EACH_25(func, type, i, arg, ...)   func(type, i, arg); FOR_EACH_24(func, type, i + 1, __VA_ARGS__)
#define FOR_EACH_26(func, type, i, arg, ...)   func(type, i, arg); FOR_EACH_25(func, type, i + 1, __VA_ARGS__)
#define FOR_EACH_27(func, type, i, arg, ...)   func(type, i, arg); FOR_EACH_26(func, type, i + 1, __VA_ARGS__)
#define FOR_EACH_28(func, type, i, arg, ...)   func(type, i, arg); FOR_EACH_27(func, type, i + 1, __VA_ARGS__)
#define FOR_EACH_29(func, type, i, arg, ...)   func(type, i, arg); FOR_EACH_28(func, type, i + 1, __VA_ARGS__)
#define FOR_EACH_30(func, type, i, arg, ...)   func(type, i, arg); FOR_EACH_29(func, type, i + 1, __VA_ARGS__)
#define FOR_EACH_31(func, type, i, arg, ...)   func(type, i, arg); FOR_EACH_30(func, type, i + 1, __VA_ARGS__)
#define FOR_EACH_32(func, type, i, arg, ...)   func(type, i, arg); FOR_EACH_31(func, type, i + 1, __VA_ARGS__)
#define FOR_EACH_33(func, type, i, arg, ...)   func(type, i, arg); FOR_EACH_32(func, type, i + 1, __VA_ARGS__)
#define FOR_EACH_34(func, type, i, arg, ...)   func(type, i, arg); FOR_EACH_33(func, type, i + 1, __VA_ARGS__)
#define FOR_EACH_35(func, type, i, arg, ...)   func(type, i, arg); FOR_EACH_34(func, type, i + 1, __VA_ARGS__)
#define FOR_EACH_36(func, type, i, arg, ...)   func(type, i, arg); FOR_EACH_35(func, type, i + 1, __VA_ARGS__)
#define FOR_EACH_37(func, type, i, arg, ...)   func(type, i, arg); FOR_EACH_36(func, type, i + 1, __VA_ARGS__)
#define FOR_EACH_38(func, type, i, arg, ...)   func(type, i, arg); FOR_EACH_37(func, type, i + 1, __VA_ARGS__)
#define FOR_EACH_39(func, type, i, arg, ...)   func(type, i, arg); FOR_EACH_38(func, type, i + 1, __VA_ARGS__)
#define FOR_EACH_40(func, type, i, arg, ...)   func(type, i, arg); FOR_EACH_39(func, type, i + 1, __VA_ARGS__)
#define FOR_EACH_41(func, type, i, arg, ...)   func(type, i, arg); FOR_EACH_40(func, type, i + 1, __VA_ARGS__)
#define FOR_EACH_42(func, type, i, arg, ...)   func(type, i, arg); FOR_EACH_41(func, type, i + 1, __VA_ARGS__)
#define FOR_EACH_43(func, type, i, arg, ...)   func(type, i, arg); FOR_EACH_42(func, type, i + 1, __VA_ARGS__)
#define FOR_EACH_44(func, type, i, arg, ...)   func(type, i, arg); FOR_EACH_43(func, type, i + 1, __VA_ARGS__)
#define FOR_EACH_45(func, type, i, arg, ...)   func(type, i, arg); FOR_EACH_44(func, type, i + 1, __VA_ARGS__)
#define FOR_EACH_46(func, type, i, arg, ...)   func(type, i, arg); FOR_EACH_45(func, type, i + 1, __VA_ARGS__)
#define FOR_EACH_47(func, type, i, arg, ...)   func(type, i, arg); FOR_EACH_46(func, type, i + 1, __VA_ARGS__)
#define FOR_EACH_48(func, type, i, arg, ...)   func(type, i, arg); FOR_EACH_47(func, type, i + 1, __VA_ARGS__)
#define FOR_EACH_49(func, type, i, arg, ...)   func(type, i, arg); FOR_EACH_48(func, type, i + 1, __VA_ARGS__)
#define FOR_EACH_50(func, type, i, arg, ...)   func(type, i, arg); FOR_EACH_49(func, type, i + 1, __VA_ARGS__)
#define FOR_EACH_51(func, type, i, arg, ...)   func(type, i, arg); FOR_EACH_50(func, type, i + 1, __VA_ARGS__)
#define FOR_EACH_52(func, type, i, arg, ...)   func(type, i, arg); FOR_EACH_51(func, type, i + 1, __VA_ARGS__)
#define FOR_EACH_53(func, type, i, arg, ...)   func(type, i, arg); FOR_EACH_52(func, type, i + 1, __VA_ARGS__)
#define FOR_EACH_54(func, type, i, arg, ...)   func(type, i, arg); FOR_EACH_53(func, type, i + 1, __VA_ARGS__)
#define FOR_EACH_55(func, type, i, arg, ...)   func(type, i, arg); FOR_EACH_54(func, type, i + 1, __VA_ARGS__)
#define FOR_EACH_56(func, type, i, arg, ...)   func(type, i, arg); FOR_EACH_55(func, type, i + 1, __VA_ARGS__)
#define FOR_EACH_57(func, type, i, arg, ...)   func(type, i, arg); FOR_EACH_56(func, type, i + 1, __VA_ARGS__)
#define FOR_EACH_58(func, type, i, arg, ...)   func(type, i, arg); FOR_EACH_57(func, type, i + 1, __VA_ARGS__)
#define FOR_EACH_59(func, type, i, arg, ...)   func(type, i, arg); FOR_EACH_58(func, type, i + 1, __VA_ARGS__)
#define FOR_EACH_60(func, type, i, arg, ...)   func(type, i, arg); FOR_EACH_59(func, type, i + 1, __VA_ARGS__)
#define FOR_EACH_61(func, type, i, arg, ...)   func(type, i, arg); FOR_EACH_60(func, type, i + 1, __VA_ARGS__)
#define FOR_EACH_62(func, type, i, arg, ...)   func(type, i, arg); FOR_EACH_61(func, type, i + 1, __VA_ARGS__)
#define FOR_EACH_63(func, type, i, arg, ...)   func(type, i, arg); FOR_EACH_62(func, type, i + 1, __VA_ARGS__)
#define FOR_EACH_64(func, type, i, arg, ...)   func(type, i, arg); FOR_EACH_63(func, type, i + 1, __VA_ARGS__)

#define DEFINE_STRUCT(type_name, ...)                                                                                                           \
struct type_name                                                                                                                                \
{                                                                                                                                               \
    static constexpr size_t member_count = MEMBER_COUNT(__VA_ARGS__);                                                                           \
    PASTE(FOR_EACH_, MEMBER_COUNT(__VA_ARGS__))(ARG_EACH, type_name, 0, __VA_ARGS__);                                                           \
    template <size_t TIdx>                                                                                                                      \
    typename TValueType<type_name, TIdx>::type get() noexcept                                                                                   \
    { return FFieldGet::template get<TIdx>(*this); }                                                                                            \
    template <size_t TIdx>                                                                                                                      \
    typename TValueType<const type_name, TIdx>::type get() const noexcept                                                                       \
    { return FFieldGet::template get<TIdx>(*this); }                                                                                            \
    template <size_t TMetaIdx>                                                                                                                  \
    typename TMetaIndex<type_name, TMetaIdx>::value_type meta_get() noexcept                                                                    \
    { return FMetaGet::template meta_get<TMetaIdx>(*this); }                                                                                    \
    template <size_t TMetaIdx>                                                                                                                  \
    typename TMetaIndex<const type_name, TMetaIdx>::value_type meta_get() const noexcept                                                        \
    { return FMetaGet::template meta_get<TMetaIdx>(*this); }                                                                                    \
    template <size_t TIdx, typename T,                                                                                                          \
        typename = typename std::enable_if<                                                                                                     \
            std::is_constructible<typename std::remove_reference<typename TValueType<type_name, TIdx>::type>::type, T>::value                   \
        >::type>                                                                                                                                \
    void set(T&& value)                                                                                                                         \
    { get<TIdx>() = std::forward<T>(value); }                                                                                                   \
    template <size_t TIdx, typename... TArgs,                                                                                                   \
        typename = typename std::enable_if<                                                                                                     \
            std::is_constructible<typename std::remove_reference<typename TValueType<type_name, TIdx>::type>::type, TArgs...>::value            \
        >::type>                                                                                                                                \
    void set(TArgs&&... args)                                                                                                                   \
    {                                                                                                                                           \
        using type = typename std::decay<typename TValueType<type_name, TIdx>::type>::type;                                                     \
        get<TIdx>() = type(std::forward<TArgs>(args)...);                                                                                       \
    }                                                                                                                                           \
    template <size_t TMetaIdx, typename T,                                                                                                      \
        typename = typename std::enable_if<                                                                                                     \
            std::is_constructible<typename std::remove_reference<typename TMetaIndex<const type_name, TMetaIdx>::value_type>::type, T>::value   \
        >::type>                                                                                                                                \
    void meta_set(T&& value)                                                                                                                    \
    { meta_get<TMetaIdx>() = std::forward<T>(value); }                                                                                          \
    template <size_t TMetaIdx, typename... TArgs,                                                                                               \
        typename = typename std::enable_if<                                                                                                     \
            std::is_constructible<typename std::remove_reference<typename TMetaIndex<type_name, TMetaIdx>::value_type>::type, TArgs...>::value  \
        >::type>                                                                                                                                \
    void meta_set(TArgs&&... args)                                                                                                              \
    {                                                                                                                                           \
        using type = typename std::decay<typename TMetaIndex<type_name, TMetaIdx>::value_type>::type;                                           \
        meta_get<TMetaIdx>() = type(std::forward<TArgs>(args)...);                                                                              \
    }                                                                                                                                           \
};                                                                                                                                              \
PASTE(FOR_EACH_, MEMBER_COUNT(__VA_ARGS__))(FIELD_EACH, type_name, 0, __VA_ARGS__)                                                              \
// ...
#endif

#endif // !_STATIC_REFLECTION_H_
