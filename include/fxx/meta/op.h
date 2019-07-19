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

#include <fxx/meta/detect.h>
// fxx::meta::is_detected

#include <type_traits>
// std::conjunction
#include <utility>
// std::declval

// Helper for definting the has_xxx_op traits for an unary operator.
#define FXX_META_OP_HAS_UNOP(name, arg)\
template<class arg>\
using has_op_ ## name = is_detected<name ## _t, arg>;\
template<class arg>\
static constexpr bool has_op_ ## name ## _v = has_op_ ## name <arg>::value;\
template<class T, class arg>\
using has_op_exact_ ## name = is_detected_exact<T, name ## _t, arg>;\
template<class T, class arg>\
static constexpr bool has_op_exact_ ## name ## _v = has_op_exact_ ## name <T, arg>::value;\
template<class To, class arg>\
using has_op_convertible_ ## name = is_detected_convertible<To, name ## _t, arg>;\
template<class To, class arg>\
static constexpr bool has_op_convertible_ ## name ## _v = has_op_convertible_ ## name <To, arg>::value;\
template<class To, class arg>\
using has_op_nothrow_convertible_ ## name = is_detected_nothrow_convertible<To, name ## _t, arg>;\
template<class To, class arg>\
static constexpr bool has_op_nothrow_convertible_ ## name ## _v = has_op_nothrow_convertible_ ## name <To, arg>::value

// Helper for defining a left-sided unary operator.
#define FXX_META_OP_UNOP_T(name, op)                                                               \
template<class Rhs>                                                                                \
using name ## _t = decltype(op std::declval<Rhs>());                                               \
FXX_META_OP_HAS_UNOP(name, Rhs)

// Helper for defining a right-sided unary operator.
#define FXX_META_OP_UNOP2_T(name, op)                                                              \
template<class Lhs>                                                                                \
using name ## _t = decltype(std::declval<Lhs>() op);                                               \
FXX_META_OP_HAS_UNOP(name, Lhs)

// Helper for definting the has_xxx_op traits for a binary operator.
#define FXX_META_OP_HAS_BINOP(name)\
template<class Lhs, class Rhs = Lhs>\
using has_op_ ## name = is_detected<name ## _t, Lhs, Rhs>;\
template<class Lhs, class Rhs = Lhs>\
static constexpr bool has_op_ ## name ## _v = has_op_ ## name <Lhs, Rhs>::value;\
template<class T, class Lhs, class Rhs = Lhs>\
using has_op_exact_ ## name = is_detected_exact<T, name ## _t, Lhs, Rhs>;\
template<class T, class Lhs, class Rhs = Lhs>\
static constexpr bool has_op_exact_ ## name ## _v = has_op_exact_ ## name <T, Lhs, Rhs>::value;\
template<class To, class Lhs, class Rhs = Lhs>\
using has_op_convertible_ ## name = is_detected_convertible<To, name ## _t, Lhs, Rhs>;\
template<class To, class Lhs, class Rhs = Lhs>\
static constexpr bool has_op_convertible_ ## name ## _v = has_op_convertible_ ## name <To, Lhs, Rhs>::value;\
template<class To, class Lhs, class Rhs = Lhs>\
using has_op_nothrow_convertible_ ## name = is_detected_nothrow_convertible<To, name ## _t, Lhs, Rhs>;\
template<class To, class Lhs, class Rhs = Lhs>\
static constexpr bool has_op_nothrow_convertible_ ## name ## _v = has_op_nothrow_convertible_ ## name <To, Lhs, Rhs>::value

// Helper for defining a binary operator.
#define FXX_META_OP_BINOP_T(name, op)                                                              \
template<class Lhs, class Rhs = Lhs>                                                               \
using name ## _t = decltype(std::declval<Lhs>() op std::declval<Rhs>());                           \
FXX_META_OP_HAS_BINOP(name)

namespace fxx { namespace meta {

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
template<class T, class Sub>
using has_op_subscript = is_detected<subscript_t, T, Sub>;
template<class T, class Sub>
static constexpr bool has_op_subscript_v = has_op_subscript<T, Sub>::value;
template<class To, class T, class Sub>
using has_op_exact_subscript = is_detected_exact<To, subscript_t, T, Sub>;
template<class To, class T, class Sub>
static constexpr bool has_op_exact_subscript_v = has_op_exact_subscript<To, T, Sub>::value;
template<class To, class T, class Sub>
using has_op_convertible_subscript = is_detected_convertible<To, subscript_t, T, Sub>;
template<class To, class T, class Sub>
static constexpr bool has_op_convertible_subscript_v = has_op_convertible_subscript<To, T, Sub>::value;
template<class To, class T, class Sub>
using has_op_nothrow_convertible_subscript = is_detected_nothrow_convertible<To, subscript_t, T, Sub>;
template<class To, class T, class Sub>
static constexpr bool has_op_nothrow_convertible_subscript_v = has_op_nothrow_convertible_subscript<To, T, Sub>::value;

// Call operator.
template<class T, class... Args>
using call_t = decltype(std::declval<T>()(std::declval<Args>()...));
template<class T, class... Args>
using has_op_call = is_detected<call_t, T, Args...>;
template<class T, class... Args>
static constexpr bool has_op_call_v = has_op_call<T, Args...>::value;
template<class To, class T, class... Args>
using has_op_exact_call = is_detected_exact<To, call_t, T, Args...>;
template<class To, class T, class... Args>
static constexpr bool has_op_exact_call_v = has_op_exact_call<To, T, Args...>::value;
template<class To, class T, class... Args>
using has_op_convertible_call = is_detected_convertible<To, call_t, T, Args...>;
template<class To, class T, class... Args>
static constexpr bool has_op_convertible_call_v = has_op_convertible_call<To, T, Args...>::value;
template<class To, class T, class... Args>
using has_op_nothrow_convertible_call = is_detected_nothrow_convertible<To, call_t, T, Args...>;
template<class To, class T, class... Args>
static constexpr bool has_op_nothrow_convertible_call_v = has_op_nothrow_convertible_call<To, T, Args...>::value;

} } // namespace fxx::meta

#endif