/** Backport of the C++20 feature std::is_nothrow_convertible.
 *
 * @file        cxx/is_nothrow_convertible.h
 * @author      Karl F. A. Friebel (karl.friebel@friebelnet.de)
 *
 * @version     0.1
 * @date        2019-07-18
 *
 * @copyright   Copyright (c) 2019
 */

#ifndef FXX_CXX_IS_NOTHROW_CONVERTIBLE_H
#define FXX_CXX_IS_NOTHROW_CONVERTIBLE_H
#pragma once

#if __cplusplus > 201703L
// C++20

#include <type_traits>
// std::(is_nothrow_convertible, is_nothrow_convertible_v)

#else
// C++17

#define FXX_CXX_IS_NOTHROW_CONVERTIBLE

#include <type_traits>
// std::(bool_constant, false_type, is_convertible)
#include <utility>
// std::declval

namespace std {

namespace detail {

template<class, class, bool>
struct is_nothrow_convertible_impl;

template<class From, class To>
struct is_nothrow_convertible_impl<From, To, true>
: std::bool_constant<noexcept(static_cast<To>(std::declval<From>()))> {};

template<class From, class To>
struct is_nothrow_convertible_impl<From, To, false>
: std::false_type {};

} // namespace detail

/** An std::bool_constant that indicates whether a non-throwing conversion exists between types.
 *
 * Extends the is_convertible trait with a test that checks whether the conversion expression is
 * noexcept qualified.
 *
 * @tparam  From    Source type.
 * @tparam  To      Target type.
 */
template<class From, class To>
struct is_nothrow_convertible
: detail::is_nothrow_convertible_impl<From, To, std::is_convertible<From, To>::value> {};

/** A compile-time constant indicating whether a non-throwing conversion exists between types.
 *
 * See is_nothrow_convertible for more details.
 *
 * @tparam  From    Source type.
 * @tparam  To      Target type.
 */
template<class From, class To>
static constexpr bool is_nothrow_convertible_v = is_nothrow_convertible<From, To>::value;

} // namespace std

#endif

#endif

//--------------------------------------------------------------------------------------------------
// VERIFICATION USING STATIC ASSERTIONS
//--------------------------------------------------------------------------------------------------

#ifdef FXX_TEST_STATIC
#ifdef FXX_CXX_IS_NOTHROW_CONVERTIBLE

struct X { X(int) noexcept {} };
struct Y { Y(X) {} };

static_assert(
    !std::is_nothrow_convertible_v<Y, X>,
    "std::is_nothrow_convertible_v: No conversion"
);
static_assert(
    std::is_nothrow_convertible_v<int, X>,
    "std::is_nothrow_convertible_v: Detected noexcept conversion"
);
static_assert(
    !std::is_nothrow_convertible_v<X, Y>,
    "std::is_nothrow_convertible_v: Detected noexcept(false) conversion"
);

#endif
#undef FXX_TEST_STATIC
#endif