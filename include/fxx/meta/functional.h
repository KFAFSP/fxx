/** Metaprogramming utilities for functional templates.
 *
 * Templates can be used to implement compile-time functions that transform types. As such, there
 * may be predicate templates (which evaluate to types that have a `static constexpr bool value`
 * member) as well as functor types (which evaluate to result types).
 *
 * For easier declaration and combination of these, tis file provides some functional wrappers that
 * work on templates.
 *
 * The following types are defined in this file, grouped by use case.
 *
 * @code{.unparsed}
 * Building blocks:
 *
 *      identity ... Functor type that always returns the first argument.
 *     tautology ... Predicate type that always evaluates to true.
 * contradiction ... Predicate type that always evaluates to false.
 *      constant ... Obtain a functor that always returns the same type.
 *
 * Combination:
 *
 *          bind ... Bind template parameters to functor types.
 *       partial ... Partially bind template parameters to concrete types.
 * @endcode
 *
 * @file        meta/functional.h
 *
 * @author      Karl F. A. Friebel (karl.friebel@friebelnet.de)
 *
 * @version     0.1
 * @date        2019-06-04
 *
 * @copyright   Copyright (c) 2019
 */

#ifndef FXX_META_FUNCTIONAL_H
#define FXX_META_FUNCTIONAL_H
#pragma once

#include <type_traits>
// std::(conditional_t, false_type, true_type)

namespace fxx { namespace meta {

/** Functor type that always returns the first argument.
 *
 * @code{.unparsed}
 * identity<T_0, T_1, ..., T_(N-1)> = T_0
 *
 *      where T_i is the i-th template argument
 *        and N is the number of template arguments.
 * @encode
 */
template<class T, class...>
using identity = T;

/** Predicate type that always returns true.
 *
 * @code{.unparsed}
 * identity<T_0, T_1, ..., T_(N-1)>::value = true
 *
 *      where T_i is the i-th template argument
 *        and N is the number of template arguments.
 * @encode
 */
template<class...>
using tautology = std::true_type;

/** Predicate type that always returns false.
 *
 * @code{.unparsed}
 * identity<T_0, T_1, ..., T_(N-1)>::value = false
 *
 *      where T_i is the i-th template argument
 *        and N is the number of template arguments.
 * @encode
 */
template<class...>
using contradiction = std::false_type;

/** Obtain a functor that always evaluates to the given constant.
 *
 * @code{.unparsed}
 * constant<T>::type<U_0, U_1, ..., U_(N-1)> = T
 *
 *      where U_i is the i-th template functor argument
 *        and N is the number of functor arguments.
 * @encode
 *
 * @tparam  T   Constant.
 */
template<class T>
struct constant {
    /** Constant functor template. */
    template<class...>
    using type = T;
};

/** Bind parameters of a template to functors.
 *
 * @code{.unparsed}
 * bind<T, F_0, F_1, ..., F_(N-1)>::type<U_0, U_1, ..., U_(N-1)>
 *  = T<F_0<U_0>, F_1<U_1>, ..., F_(N-1)<U_(N-1)>>
 *
 *      where F_i is the i-th bound functor
 *        and U_i is the i-th template output functor argument
 *        and N is the number of output functor arguments.
 * @encode
 *
 * @warning Behavior is undefined when sizeof...(@p Fns) does not match the number of arguments that
 *          @p Target accepts.
 *
 * @tparam  Target  Target template.
 * @tparam  Fns     Functors to bind.
 */
template<template<class...> class Target, template<class...> class... Fns>
struct bind {
    /** Bound result template. */
    template<class... Args>
    using type = Target<Fns<Args>...>;
};

/** Bind parameters of a template to concrete types.
 *
 * @code{.unparsed}
 * bind<T, U_0, U_1, ..., U_(N-1)>::type<V_0, V_1, ..., V_(M-1)>
 *  = T<U_0, U_1, ..., U_(N-1), V_0, V_1, ..., V_(M-1)>
 *
 *      where U_i is the i-th bound argument
 *        and V_i is the i-th free argument.
 *        and N is the number of bound arguments.
 *        and M is the number of free arguments.
 * @encode
 *
 * @warning Behavior is undefined when sizeof...(@p First) is greater than the number of arguments
 *          that @p Target accepts.
 *
 * @tparam  Target  Target template.
 * @tparam  First   Arguments to bind.
 */
template<template<class...> class Target, class... First>
struct partial {
    /** Bound result template. */
    template<class... Last>
    using type = Target<First..., Last...>;
};

namespace detail {

// Dispatch case.
template<template<class> class Pred, class... Args>
struct any_impl {};

// Trivial case.
template<template<class> class Pred>
struct any_impl<Pred> : std::false_type {};

// Recursive case.
template<template<class> class Pred, class Head, class... Tail>
struct any_impl<Pred, Head, Tail...>
: std::conditional_t<Pred<Head>::value, std::true_type, any_impl<Pred, Tail...>> {};

} // namespace detail

/** A std::bool_constant indicating whether a predicate is matched by any argument.
 *
 * An empty list of arguments never matches any predicate.
 *
 * @code{.unparsed}
 * any<P, T_0, T_1, ..., T_(N-1)> <=> Ex. i el [0, N): P<T_i>::value = true
 *
 *      where P is the predicate
 *        and T are the input arguments
 *        and T_i is the i-th input argument
 *        and N is the number of input arguments
 * @endcode
 *
 * @warning Behavior is undefined when @p Pred instances do not provide a
 *          `static constexpr bool value`.
 *
 * @tparam  Pred    Predicate type.
 * @tparam  Args    Arguments.
 */
template<template<class> class Pred, class... Args>
using any = detail::any_impl<Pred, Args...>;

/** A compile-time constant indicating whether a predicate is matched by any argument.
 *
 * See any for more details.
 *
 * @tparam  Pred    Predicate type.
 * @tparam  Args    Arguments.
 */
template<template<class> class Pred, class... Args>
static constexpr bool any_v = any<Pred, Args...>::value;

namespace detail {

// Dispatch case.
template<template<class> class Pred, class... Args>
struct all_impl {};

// Trivial case.
template<template<class> class Pred>
struct all_impl<Pred> : std::true_type {};

// Recursive case.
template<template<class> class Pred, class Head, class... Tail>
struct all_impl<Pred, Head, Tail...>
: std::conditional_t<Pred<Head>::value, all_impl<Pred, Tail...>, std::false_type> {};

} // namespace detail

/** A std::bool_constant indicating whether a predicate is matched by all arguments.
 *
 * An empty list of arguments matches all predicates.
 *
 * @code{.unparsed}
 * all<P, T_0, T_1, ..., T_(N-1)> <=> A. i el [0, N): P<T_i>::value = true
 *
 *      where P is the predicate
 *        and T are the input arguments
 *        and T_i is the i-th input argument
 *        and N is the number of input arguments
 * @endcode
 *
 * @warning Behavior is undefined when @p Pred instances do not provide a
 *          `static constexpr bool value`.
 *
 * @tparam  Pred    Predicate type.
 * @tparam  Args    Arguments.
 */
template<template<class> class Pred, class... Args>
using all = detail::all_impl<Pred, Args...>;

/** A compile-time constant indicating whether a predicate is matched by all arguments.
 *
 * See all for more details.
 *
 * @tparam  Pred    Predicate type.
 * @tparam  Args    Arguments.
 */
template<template<class> class Pred, class... Args>
static constexpr bool all_v = all<Pred, Args...>::value;

} } // namespace fxx::meta

#endif

//--------------------------------------------------------------------------------------------------
// VERIFICATION USING STATIC ASSERTIONS
//--------------------------------------------------------------------------------------------------

#ifdef FXX_TEST_STATIC

#include <tuple>
// std::tuple
#include <type_traits>
// std::(is_reference_v, is_same_v, is_signed_v, remove_reference_t)

namespace fxx { namespace meta {

// identity
static_assert(
    std::is_same_v<int, identity<int, bool>>,
    "fxx::meta::identity: Regular case"
);

// tautology
static_assert(
    tautology<int, bool>::value,
    "fxx::meta::tautology: Regular case"
);

// contradiction
static_assert(
    !contradiction<int, bool>::value,
    "fxx::meta::contradiction: Regular case"
);

// constant
static_assert(
    std::is_same_v<int, typename constant<int>::template type<bool>>,
    "fxx::meta::constant: Regular case"
);

// bind
static_assert(
    std::is_same_v<
        std::tuple<std::is_signed<int>, int>,
        typename bind<std::tuple, std::is_signed, identity>::template type<int, int>
    >,
    "fxx::meta::bind: Regular case"
);

// partial
static_assert(
    std::is_same_v<
        std::tuple<int, bool>,
        typename partial<std::tuple, int>::template type<bool>
    >,
    "fxx::meta::partial: Regular case"
);

// any_v
static_assert(
    !any_v<tautology>,
    "fxx::meta::any: Empty case"
);
static_assert(
    any_v<std::is_signed, ushort, ushort, short, ushort>,
    "fxx::meta::any: Regular case"
);

// all_v
static_assert(
    all_v<contradiction>,
    "fxx::meta::all: Empty case"
);
static_assert(
    !all_v<std::is_signed, short, short, short, ushort>,
    "fxx::meta::any: Regular case"
);

} } // namespace fxx::meta

#endif