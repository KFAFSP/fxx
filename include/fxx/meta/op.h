/** Traits for all C++ operators.
 *
 * Using the detection idiom, provides traits for all C++ operator types.
 *
 * @file        meta/op.h
 * @author      Karl F. A. Friebel (karl.friebel@friebelnet.de)
 *
 * @version     0.1
 * @date        2019-07-18
 *
 * @copyright   Copyright (c) 2019
 */

#ifndef FXX_META_OP_H
#define FXX_META_OP_H
#pragma once

#include <fxx/cxx/is_nothrow_convertible.h>
// std::is_nothrow_convertible
#include <fxx/meta/tuple.h>
// fxx::meta::(apply_t, tuple_skip_t)

#include <tuple>
// std::(tuple_element_t, tuple_size_v)
#include <type_traits>
// std::conjunction
#include <utility>
// std::declval

// Helper for defining a left-sided unary operator.
#define FXX_META_OP_UNOP_T(name, op)                                                               \
template<class Rhs>                                                                                \
using name ## _t = decltype(op std::declval<Rhs>());                                               \
template<class Rhs, class Enable = void>                                                           \
struct op_ ## name : operator_base<Rhs> {};                                                        \
template<class Rhs>                                                                                \
struct op_ ## name <Rhs, std::void_t<name ## _t<Rhs>>>                                             \
: operator_info<name ## _t<Rhs>, Rhs> {                                                            \
    static constexpr bool is_nothrow = noexcept(op std::declval<Rhs>());                           \
    constexpr auto operator()(Rhs&& rhs) noexcept(is_nothrow)                                      \
    { return op std::forward<Rhs>(rhs); }                                                          \
}

// Helper for defining a right-sided unary operator.
#define FXX_META_OP_UNOP2_T(name, op)                                                              \
template<class Lhs>                                                                                \
using name ## _t = decltype(std::declval<Lhs>() op);                                               \
template<class Lhs, class Enable = void>                                                           \
struct op_ ## name : operator_base<Lhs> {};                                                        \
template<class Lhs>                                                                                \
struct op_ ## name <Lhs, std::void_t<name ## _t<Lhs>>>                                             \
: operator_info<name ## _t<Lhs>, Lhs> {                                                            \
    static constexpr bool is_nothrow = noexcept(std::declval<Lhs>() op);                           \
    constexpr auto operator()(Lhs&& lhs) noexcept(is_nothrow)                                      \
    { return std::forward<Lhs>(lhs) op; }                                                          \
}

// Helper for defining a binary operator.
#define FXX_META_OP_BINOP_T(name, op)                                                              \
template<class Lhs, class Rhs = Lhs>                                                               \
using name ## _t = decltype(std::declval<Lhs>() op std::declval<Rhs>());                           \
template<class Lhs, class Rhs = Lhs, class Enable = void>                                          \
struct op_ ## name : operator_base<Lhs, Rhs> {};                                                   \
template<class Lhs, class Rhs>                                                                     \
struct op_ ## name <Lhs, Rhs, std::void_t<name ## _t<Lhs, Rhs>>>                                   \
: operator_info<name ## _t<Lhs, Rhs>, Lhs, Rhs> {                                                  \
    static constexpr bool is_nothrow = noexcept(std::declval<Lhs>() op std::declval<Rhs>());       \
    constexpr auto operator()(Lhs&& lhs, Rhs&& rhs) noexcept(is_nothrow)                           \
    { return std::forward<Lhs>(lhs) op std::forward<Rhs>(rhs); }                                   \
}

namespace fxx { namespace meta {

template<class... Operands>
struct operator_base {
    static constexpr std::size_t arity = sizeof...(Operands);
    using operand_types = std::tuple<Operands...>;

    static constexpr bool is_detected = false;
    static constexpr bool is_nothrow = false;
    template<class>
    static constexpr bool is_convertible = false;
    template<class>
    static constexpr bool is_nothrow_convertible = false;
};

template<class Result, class... Operands>
struct operator_info : operator_base<Operands...> {
    using result_type = Result;

    static constexpr bool is_detected = true;
    template<class To>
    static constexpr bool is_convertible = std::is_convertible_v<To, result_type>;
    template<class To>
    static constexpr bool is_nothrow_convertible = std::is_nothrow_convertible_v<To, result_type>;
};

// Arithmetic operators.

FXX_META_OP_BINOP_T(plus, +);
FXX_META_OP_UNOP_T(inc, ++);
FXX_META_OP_UNOP2_T(postinc, ++);
FXX_META_OP_BINOP_T(minus, -);
FXX_META_OP_UNOP_T(dec, --);
FXX_META_OP_UNOP2_T(postdec, --);
FXX_META_OP_BINOP_T(multiplies, *);
FXX_META_OP_BINOP_T(divides, /);
FXX_META_OP_BINOP_T(modulus, %);
FXX_META_OP_UNOP_T(negate, -);
FXX_META_OP_UNOP_T(promote, +);

// Comparisons.

FXX_META_OP_BINOP_T(equal_to, ==);
FXX_META_OP_BINOP_T(not_equal_to, !=);
FXX_META_OP_BINOP_T(greater, >);
FXX_META_OP_BINOP_T(less, <);
FXX_META_OP_BINOP_T(greater_equal, >=);
FXX_META_OP_BINOP_T(less_equal, <=);

// Logical operators.

FXX_META_OP_BINOP_T(logical_and, &&);
FXX_META_OP_BINOP_T(logical_or, ||);
FXX_META_OP_UNOP_T(logical_not, !);

// Bitwise operators.

FXX_META_OP_BINOP_T(bit_and, &);
FXX_META_OP_BINOP_T(bit_or, |);
FXX_META_OP_BINOP_T(bit_xor, ^);
FXX_META_OP_UNOP_T(bit_not, ~);
FXX_META_OP_BINOP_T(bit_left_shift, <<);
FXX_META_OP_BINOP_T(bit_right_shift, >>);

// Miscellaneous operators.

FXX_META_OP_UNOP_T(deref, *);
FXX_META_OP_UNOP_T(addr_of, &);

// Subscript operator.
template<class T, class Sub>
using subscript_t = decltype(std::declval<T>()[std::declval<Sub>()]);

template<class T, class Sub, class Enable = void>
struct op_subscript : operator_base<T, Sub> {
    using target_type = T;
    using index_type = Sub;
};
template<class T, class Sub>
struct op_subscript <T, Sub, std::void_t<subscript_t<T, Sub>>>
: operator_info<subscript_t<T, Sub>, T, Sub> {
    using target_type = T;
    using index_type = Sub;

    static constexpr bool is_nothrow = noexcept(std::declval<T>()[std::declval<Sub>()]);
    constexpr auto operator()(T&& t, Sub&& sub) noexcept(is_nothrow)
    { return std::forward<T>(t)[std::forward<Sub>(sub)]; }
};

// Call operator.
template<class T, class... Args>
using call_t = decltype(std::declval<T>()(std::declval<Args>()...));

namespace detail {

template<class Ops, class Enable = void>
struct op_call_impl : apply_t<operator_base, Ops> {
    using target_type = std::tuple_element_t<0, Ops>;
    using argument_types = tuple_skip_t<1, Ops>;
    static constexpr std::size_t argument_count = std::tuple_size_v<argument_types>;
};

template<class Target, class... Args>
struct op_call_impl<std::tuple<Target, Args...>, call_t<Target, Args...>>
: operator_info<op_call_impl<Target, Args...>, Target, Args...> {
    using target_type = Target;
    using argument_types = std::tuple<Args...>;
    static constexpr std::size_t argument_count = sizeof...(Args);

    static constexpr bool is_nothrow = noexcept(std::declval<Target>()(std::declval<Args>()...));
    constexpr auto operator()(Target&& target, Args&&... args) noexcept(is_nothrow)
    { return std::forward<Target>(target)(std::forward<Args>(args)...); }
};

} // namespace detail

template<class Target, class... Args>
using op_call = detail::op_call_impl<std::tuple<Target, Args...>, void>;

} } // namespace fxx::meta

#endif