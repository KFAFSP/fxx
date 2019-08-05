/** Metaprogramming utilities for the std::tuple type.
 *
 * std::tuple can be used effectively to store variadic template parameter packs and reason with
 * them instead of re-implementing recursive templates for specific tasks.
 *
 * The following types are defined in this file, grouped by use case.
 *
 * @code{.unparsed}
 * Declaring:
 *
 *     make_tuple_t ... Turn a variadic parameter pack into a std::tuple type.
 *
 * Consuming:
 *
 *          apply_t ... Forward std::tuple types as variadic arguments to another template.
 *    apply_partial ... Forward std::tuple types as partial variadic arguments to another template.
 *
 * Reasoning:
 *
 *            first ... Find the first type matching a predicate in a std::tuple.
 *             find ... Find the first appearance of a type in a std::tuple.
 *
 * Restructuring:
 *
 *      tuple_cat_t ... Concatenate std::tuple types.
 *     tuple_flip_t ... Flip (reverse the order of) a std::tuple type.
 *     tuple_pick_t ... Pick (select elements) from a std::tuple type.
 *      tuple_dup_t ... Duplicate (and concatenate) a std::tuple type.
 *     tuple_skip_t ... Skip elements in a std::tuple type.
 *     tuple_take_t ... Take elements from a std::tuple type.
 *    tuple_slice_t ... Obtain a sub-range from a std::tuple type.
 *
 * Transforming:
 *
 *      tuple_map_t ... Map all elements of a std::tuple type.
 *   tuple_reduce_t ... Reduce a std::tuple type.
 *     tuple_fold_t ... Fold a std::tuple type from the left.
 *   tuple_filter_t ... Filter a std::tuple based on type.
 * @endcode
 *
 * @file        meta/tuple.h
 *
 * @author      Karl F. A. Friebel (karl.friebel@friebelnet.de)
 *
 * @version     0.5
 * @date        2019-06-04
 *
 * @copyright   Copyright (c) 2019
 */

#ifndef FXX_META_TUPLE_H
#define FXX_META_TUPLE_H
#pragma once

#include <fxx/meta/functional.h>
// fxx::meta::(tautology, partial_t)

#include <tuple>
// std::(tuple, tuple_element_t)
#include <type_traits>
// std::(bool_constant, conditional_t, is_same)

#include <cstddef>
// std::size_t

namespace fxx { namespace meta {

/// @cond
namespace detail {

// Dispatch case.
template<template<class...> class, class>
struct apply_impl {};

// Variadic case.
template<template<class...> class Target, class... Ts>
struct apply_impl<Target, std::tuple<Ts...>> {
    using type = Target<Ts...>;
};

template<bool Value, std::size_t Offset>
struct index_helper : std::bool_constant<Value> {
    static constexpr std::size_t index = Offset;
};

// Dispatch case.
template<template<class> class, std::size_t, class>
struct first_impl {};

// Abort case.
template<template<class> class Pred, std::size_t Offset>
struct first_impl<Pred, Offset, std::tuple<>> : index_helper<false, Offset> {};

// Recursive case.
template<template<class> class Pred, std::size_t Offset, class Head, class... Tail>
struct first_impl<Pred, Offset, std::tuple<Head, Tail...>>
: std::conditional_t<
    Pred<Head>::value,
    index_helper<true, Offset>,
    first_impl<Pred, Offset + 1, std::tuple<Tail...>>
> {};

// Dispatch case.
template<class...>
struct tuple_cat_impl {};

// Abort case.
template<>
struct tuple_cat_impl<> {
    using type = std::tuple<>;
};

// Abort case.
template<class... Ts>
struct tuple_cat_impl<std::tuple<Ts...>> {
    using type = std::tuple<Ts...>;
};

// Recursive case.
template<class... Ls, class... Rs, class... Rest>
struct tuple_cat_impl<std::tuple<Ls...>, std::tuple<Rs...>, Rest...> {
    using type = typename tuple_cat_impl<std::tuple<Ls..., Rs...>, Rest...>::type;
};

// Dispatch case.
template<class Tuple>
struct tuple_flip_impl {};

// Recursive case.
template<class Head, class... Ts>
struct tuple_flip_impl<std::tuple<Head, Ts...>> {
    using prefix = typename tuple_flip_impl<std::tuple<Ts...>>::type;
    using type = typename tuple_cat_impl<prefix, std::tuple<Head>>::type;
};

// Abort case.
template<>
struct tuple_flip_impl<std::tuple<>> {
    using type = std::tuple<>;
};

// Dispatch case.
template<class Tuple, std::size_t... Ns>
struct tuple_pick_impl {};

// Recursive case.
template<class Tuple, std::size_t Head, std::size_t... Tail>
struct tuple_pick_impl<Tuple, Head, Tail...> {
    using suffix = typename tuple_pick_impl<Tuple, Tail...>::type;
    using type = typename tuple_cat_impl<
        std::tuple<std::tuple_element_t<Head, Tuple>>,
        suffix
    >::type;
};

// Abort case.
template<class Tuple>
struct tuple_pick_impl<Tuple> {
    using type = std::tuple<>;
};

// Recursive case.
template<std::size_t N, class Tuple>
struct tuple_dup_impl {
    using suffix = typename tuple_dup_impl<N-1, Tuple>::type;
    using type = typename tuple_cat_impl<Tuple, suffix>::type;
};

// Abort case.
template<class Tuple>
struct tuple_dup_impl<0, Tuple> {
    using type = std::tuple<>;
};

// Dispatch case.
template<class>
struct tuple_skip_one {};

// Variadic case.
template<class Head, class... Tail>
struct tuple_skip_one<std::tuple<Head, Tail...>> {
    using type = std::tuple<Tail...>;
};

// Fallback case.
/*
template<>
struct tuple_skip_one<std::tuple<>> {
    using type = std::tuple<>;
};
*/

// Recursive case.
template<std::size_t N, class Tuple>
struct tuple_skip_impl {
    using prefix = typename tuple_skip_one<Tuple>::type;
    using type = typename tuple_skip_impl<N-1, prefix>::type;
};

// Abort case.
template<class Tuple>
struct tuple_skip_impl<0, Tuple> {
    using type = Tuple;
};

// Dispatch case.
template<class>
struct tuple_take_one {};

// Variadic case.
template<class Head, class... Tail>
struct tuple_take_one<std::tuple<Head, Tail...>> {
    using type = std::tuple<Head>;
};

// Fallback case.
/*
template<>
struct tuple_take_one<std::tuple<>> {
    using type = std::tuple<>;
};
*/

// Recursive case.
template<std::size_t N, class Tuple>
struct tuple_take_impl {
    using prefix = typename tuple_take_one<Tuple>::type;
    using suffix = typename tuple_skip_one<Tuple>::type;
    using type = typename tuple_cat_impl<prefix, typename tuple_take_impl<N-1, suffix>::type>::type;
};

// Abort case.
template<class Tuple>
struct tuple_take_impl<0, Tuple> {
    using type = std::tuple<>;
};

// Dispatch case.
template<template<class> class, class>
struct tuple_map_impl {};

// Variadic case.
template<template<class> class Fn, class... Ts>
struct tuple_map_impl<Fn, std::tuple<Ts...>> {
    using type = std::tuple<Fn<Ts>...>;
};

// Recursive case.
template<template<class, class> class Fn, std::size_t N, class Tuple>
struct tuple_reduce_impl {
    using pred = typename tuple_reduce_impl<Fn, N-1, Tuple>::type;
    using type = Fn<pred, std::tuple_element_t<N-1, Tuple>>;
};

// Abort case.
template<template<class, class> class Fn, class Tuple>
struct tuple_reduce_impl<Fn, 1, Tuple> {
    using type = std::tuple_element_t<0, Tuple>;
};

// Recursion case.
template<template<class> class Pred, std::size_t N, class Tuple>
struct tuple_filter_impl {
    using pred = typename tuple_filter_impl<Pred, N-1, Tuple>::type;
    using cur = std::tuple_element_t<N-1, Tuple>;
    using flt = std::conditional_t<Pred<cur>::value, std::tuple<cur>, std::tuple<>>;
    using type = typename tuple_cat_impl<pred, flt>::type;
};

// Abort case.
template<template<class> class Pred, class Tuple>
struct tuple_filter_impl<Pred, 0, Tuple> {
    using type = std::tuple<>;
};

} // namespace detail
/// @endcond

/** Get the result type of constructing a std::tuple.
 *
 * @note    Mirrors the std::make_tuple function.
 * @note    Is effectively an alias for std::tuple.
 *
 * @code{.unparsed}
 * make_tuple_t<T_0, T_1, ..., T_(N-1)> = std::tuple<T_0, T_1, ..., T_(N-1)>
 *
 *      where T_i is the i-th input type
 *        and N is the number of input types.
 * @endcode
 *
 * @tparam  Ts      Input types.
 */
template<class... Ts>
using make_tuple_t = std::tuple<Ts...>;

/** Get the result type of applying a tuple of types to a template.
 *
 * @note    Mirrors the std::apply function.
 *
 * @code{.unparsed}
 * apply_t<T, t> = T<t_0, t_1, ..., t_(N-1)>
 *
 *      where T is the target template
 *        and t is the input tuple type
 *        and N is the size of the input tuple type
 * @endcode
 *
 * @warning Behavior is undefined when @p Tuple is not a std::tuple type.
 *
 * @tparam  Target  Target template.
 * @tparam  Tuple   Input tuple type.
 */
template<template<class...> class Target, class Tuple>
using apply_t = typename detail::apply_impl<Target, Tuple>::type;

/** Get a new template by partially applying a tuple of types to a template.
 *
 * @code{.unparsed}
 * apply_partial<T, t>::type<U...> = T<t_0, t_1, ..., t_(N-1), U...>
 *
 *      where T is the target template
 *        and t is the input tuple type
 *        and t_i is the type of the i-th element.
 *        and N is the size of the input tuple type
 *        and U are the free template arguments.
 * @endcode
 *
 * @warning Behavior is undefined when @p Tuple is not a std::tuple type.
 *
 * @tparam  Target  Target template.
 * @tparam  Tuple   Input tuple type.
 */
template<template<class...> class Target, class... Ts>
struct apply_partial {};
template<template<class...> class Target, class... Ts>
struct apply_partial<Target, std::tuple<Ts...>> {
    template<class... Args>
    using type = Target<Ts..., Args...>;
};

/** Find the first type in a std::tuple that matches a predicate.
 *
 * The 0-tuple never contains any matches.
 * The tuple is searched left to right.
 * The index at which the first matching type was found will be returned in the ::index member of
 * the result type.
 *
 * @code{.unparsed}
 * first<P, t>::value <=> Ex. i el. [0, N): P<t_i>::value = true; ::index = i
 *
 *      where P is the predicate
 *        and t is the input tuple type
 *        and t_i is the type of the i-th element
 *        and N is the size of the input tuple type
 * @endcode
 *
 * @warning Behavior is undefined when @p Pred instances do not provide a
 *          `static constexpr bool value`.
 * @warning Behavior is undefined when @p Tuple is not a std::tuple type.
 * @warning ::index is undefined when ::value is false.
 *
 * @tparam  Pred    Predicate template.
 * @tparam  Tuple   Input tuple type.
 */
template<template<class> class Pred, class Tuple>
using first = typename detail::first_impl<Pred, 0, Tuple>;

/** Get a std::bool_constant indicating whether a type is contained in a std::tuple.
 *
 * The 0-tuple does not contain any types.
 * The index at which the type was found will be returned in the ::index member of the result type.
 *
 * @code{.unparsed}
 * find<x, t>::value <=> Ex. i el. [0, N): t_i = x; ::index = i
 *
 *      where x is the matcher type.
 *        and t is the input tuple type
 *        and t_i is the type of the i-th element
 *        and N is the size of the input tuple type
 * @endcode
 *
 * @warning Behavior is undefined when @p Tuple is not a std::tuple type.
 * @warning ::index is undefined when ::value is false.
 *
 * @tparam  T       Matcher type.
 * @tparam  Tuple   Input tuple type.
 */
template<class T, class Tuple>
using find = first<partial<std::is_same, T>::template type, Tuple>;

/** Get the result type of concatenating std::tuples.
 *
 * @note    Mirrors the std::tuple_cat function.
 *
 * A concatenation of an empty list of @p Tuples results in the 0-tuple (std::tuple<>). Any 0-tuples
 * in @p Tuples do not contribute to the output.
 *
 * @code{.unparsed}
 * tuple_cat_t<t_0, t_1, ..., t_(N-1)> = std::tuple<t_0..., t_1..., t_(N-1)...>
 *
 *      where t_i is the i-th input tuple type
 *        and N is the number of input tuple types
 *        and t_i... is the expansion t_i_0, t_i_1, ... t_i_(M - 1)
 *              where t_i_j is the type of the j-th element
 *                and M is the size of t_i
 * @endcode
 *
 * @warning Behavior is undefined when any type in @p Tuples is not a std::tuple type.
 *
 * @tparam  Tuples  Input tuple types.
 */
template<class... Tuples>
using tuple_cat_t = typename detail::tuple_cat_impl<Tuples...>::type;

/** Get the result type of flipping a std::tuple.
 *
 * Flipping is defined here as reversing the order of the tuple elements. Flipping a 0- or 1- tuple
 * results in the input tuple.
 *
 * @code{.unparsed}
 * tuple_flip_t<t> = std::tuple<t_(N-1), ..., t_1, t_0>
 *
 *      where t is the input tuple type
 *        and N is the size of the input tuple type
 *        and t_i is the type of the i-th element
 * @endcode
 *
 * @warning Behavior is undefined when @p Tuple is not a std::tuple type.
 *
 * @tparam  Tuple   Input tuple type.
 */
template<class Tuple>
using tuple_flip_t = typename detail::tuple_flip_impl<Tuple>::type;

/** Get the result type of picking from a std::tuple.
 *
 * Picking is defined here as constructing a new N-tuple by selecting N elements from the input
 * tuple and concatenating them in order. The same element may be selected multiple times, and not
 * all elements have to be selected.
 *
 * @code{.unparsed}
 * tuple_pick_t<t, i_0, i_1, ..., i_(N-1)> = std::tuple<t_(i_0), t_(i_1), ..., t_(i_(N-1))>
 *
 *      where t is the input tuple type
 *        and i_0 is the i-th input index
 *        and N is the number of input indices
 *        and t_j is the type of the j-th element
 * @endcode
 *
 * @warning Behavior is undefined when @p Tuple is not a std::tuple type.
 * @warning Behavior is undefined when @p Tuple is the 0-tuple, and @p Ns is not empty.
 * @warning Behavior is undefined when @p Ns contains invalid indexes for @p Tuple.
 *
 * @tparam  Tuple   Input tuple type.
 * @tparam  Ns      Pick indices.
 */
template<class Tuple, std::size_t... Ns>
using tuple_pick_t = typename detail::tuple_pick_impl<Tuple, Ns...>::type;

/** Get the result type of duplicate-concatenating a std::tuple.
 *
 * Duplication is defined here as concatenating a tuple to itself. The 0-tuple is trivially
 * duplicatable, resulting in itself. Duplication count is counted as total number of appearances of
 * the input tuple in the result. Thus, 0-duplication always yields the 0-tuple, and 1-duplication
 * always yields the input tuple.
 *
 * @code{.unparsed}
 * tuple_dup_t<N, t> = std::tuple<t..., t..., ..., t...>
 *                                |------ xN ------|
 *
 *      where t is the input tuple type
 *        and N is the number of duplications
 * @endcode
 *
 * @warning Behavior is undefined when @p Tuple is not a std::tuple type.
 *
 * @tparam  N       Number of duplications.
 * @tparam  Tuple   Input tuple type.
 */
template<std::size_t N, class Tuple>
using tuple_dup_t = typename detail::tuple_dup_impl<N, Tuple>::type;

/** Get the result type of skipping elements in a std::tuple.
 *
 * Skipping is defined here as constructing a new tuple from all input elements except the first
 * @p Skip elements.
 *
 * @code{.unparsed}
 * tuple_skip_t<S, t> = std::tuple<t_S, t_(S+1), ..., t_(S+N-1)>
 *
 *      where t is the input tuple
 *        and N is the number of elements in the input tuple
 * @endcode
 *
 * @warning Behavior is undefined when @p Tuple is not a std::tuple type.
 * @warning Behavior is undefined when @p Skip is larger then the size of @p Tuple.
 *
 * @tparam  Skip    Number of elements to skip.
 * @tparam  Tuple   Input tuple type.
 */
template<std::size_t Skip, class Tuple>
using tuple_skip_t = typename detail::tuple_skip_impl<Skip, Tuple>::type;

/** Get the result type of taking elements from a std::tuple.
 *
 * Taking is defined here as constructing a new tuple from only the first @p Take elements of the
 * input tuple.
 *
 * @code{.unparsed}
 * tuple_take_t<T, t> = std::tuple<t_0, t_1, ..., t_(T-1)>
 *
 *      where t is the input tuple
 * @endcode
 *
 * @warning Behavior is undefined when @p Tuple is not a std::tuple type.
 * @warning Behavior is undefined when @p Take is larger then the size of @p Tuple.
 *
 * @tparam  Take    Number of elements to take.
 * @tparam  Tuple   Input tuple type.
 */
template<std::size_t Take, class Tuple>
using tuple_take_t = typename detail::tuple_take_impl<Take, Tuple>::type;

/** Get the result type of slicing a std::tuple.
 *
 * Slicing is defined here as constructing a new std::tuple from a sub range of the input tuple.
 *
 * @code{.unparsed}
 * tuple_slice_t<S, L, t> = std::tuple<t_S, t_(S+1), ..., t_(S+L-1)>
 *
 *      where t is the input tuple.
 * @endcode
 *
 * @warning Behavior is undefined when @p Tuple is not a std::tuple type.
 * @warning Behavior is undefined when @p Start is larger then the size of @p Tuple.
 * @warning Behavior is undefined when @p Start + @p Length is larger then the size of @p Tuple.
 *
 * @tparam  Start   Start index.
 * @tparam  Length  Slice length.
 * @tparam  Tuple   Input tuple type.
 */
template<std::size_t Start, std::size_t Length, class Tuple>
using tuple_slice_t = tuple_take_t<Length, tuple_skip_t<Start, Tuple>>;

/** Get the result type of mapping a std::tuple.
 *
 * Mapping is defined here as constructing a new tuple by applying a mapping transformation to all
 * elements of the input tuple. Mapping the 0-tuple always results in itself.
 *
 * @code{.unparsed}
 * tuple_map_t<Fn, t> = std::tuple<Fn<t_0>, Fn<t_1>, ..., Fn<t_(N-1)>>
 *
 *      where t is the input tuple type
 *        and N is the size of the input tuple type
 *        and t_i is the i-th element type
 * @endcode
 *
 * @warning Behavior is undefined when @p Tuple is not a std::tuple type.
 *
 * @tparam  Fn      Mapping template.
 * @tparam  Tuple   Input tuple type.
 */
template<template<class> class Fn, class Tuple>
using tuple_map_t = typename detail::tuple_map_impl<Fn, Tuple>::type;

/** Get the result type of reducing a std::tuple.
 *
 * Reduction is defined here as left-folding a tuple with its first element as the initialization
 * value. Reducing a 1-tuple results in its only element.
 *
 * @code{.unparsed}
 * tuple_reduce_t<Fn, t> = Fn<Fn<Fn<t_0, t_1>, ...>, t_(N-1)>
 *
 *      where t is the input tuple type
 *        and N is the size of the input tuple type
 * @endcode
 *
 * @warning Behavior is undefined when @p Tuple is not a std::tuple type.
 * @warning Behavior is undefined when @p Tuple is the 0-tuple.
 *
 * @tparam  Fn      Reduction template.
 * @tparam  Tuple   Input tuple type.
 */
template<template<class, class> class Fn, class Tuple>
using tuple_reduce_t = typename detail::tuple_reduce_impl<
    Fn,
    std::tuple_size_v<Tuple>,
    Tuple
>::type;

/** Get the result type of left-folding a std::tuple.
 *
 * Left-folding is defined here as Haskell's foldl, meaning that a binary fold operation visits all
 * tuple elements from left-to-right, starting with the initialization element.
 *
 * @code{.unparsed}
 * tuple_foldl_t<Fn, I, t> = Fn<Fn<Fn<Fn<I, t_0>, t_1>, ...>, t_(N-1)>
 *
 *      where t is the input tuple type
 *        and N is the size of the input tuple type
 * @endcode
 *
 * @warning Behavior is undefined when @p Tuple is not a std::tuple type.
 *
 * @tparam  Fn      Folding template.
 * @tparam  Init    Initial type.
 * @tparam  Tuple   Input tuple type.
 */
template<template<class, class> class Fn, class Init, class Tuple>
using tuple_fold_t = tuple_reduce_t<Fn, tuple_cat_t<std::tuple<Init>, Tuple>>;

/** Get the result type of filtering a std::tuple by type.
 *
 * Filtering by type is defined here as evaluating a Filter predicate for each element type and
 * constructing a new tuple by selecting only the matching elements. The 0-tuple always filters to
 * itself.
 *
 * @code{.unparsed}
 * tuple_filter_t<Pred, t> = tuple_cat_t<F_i_0, F_i_1, ..., F_i_N>
 *                   F_i_n = Pred<t_i>::value ? std::tuple<t_i> : std::tuple<>
 *
 *      where t is the input tuple type
 *        and t_i is the i-th element type
 *        and N is the size of the input tuple type
 *        and F_i_N is the filter tuple for the i-th element
 * @endcode
 *
 * @warning Behavior is undefined when @p Pred instances do not provide a
 *          `static constexpr bool value`.
 *
 * @tparam  Pred    Predicate template.
 * @tparam  Tuple   Input tuple type.
 */
template<template<class> class Pred, class Tuple>
using tuple_filter_t = typename detail::tuple_filter_impl<
    Pred,
    std::tuple_size_v<Tuple>,
    Tuple
>::type;

} } // namespace fxx::meta

#endif

//--------------------------------------------------------------------------------------------------
// VERIFICATION USING STATIC ASSERTIONS
//--------------------------------------------------------------------------------------------------

#ifdef FXX_TEST_STATIC

#include <type_traits>
// std::(is_reference_v, is_same_v,, is_signed_v remove_reference_t)

namespace fxx { namespace meta {

// make_tuple_t
static_assert(
    std::is_same_v<std::tuple<>, make_tuple_t<>>,
    "fxx::meta::make_tuple_t: Empty case"
);
static_assert(
    std::is_same_v<std::tuple<int,int&,int&&>, make_tuple_t<int,int&,int&&>>,
    "fxx::meta::make_tuple_t: Trivial case"
);

// apply_t
static_assert(
    std::is_same_v<std::tuple<>, apply_t<std::tuple, std::tuple<>>>,
    "fxx::meta::apply_t: Empty case"
);
static_assert(
    apply_t<std::is_same, std::tuple<int, int>>::value,
    "fxx::meta::apply_t: Forwarding case"
);

// apply_partial_t
static_assert(
    std::is_same_v<
        std::tuple<int>,
        typename apply_partial<std::tuple, std::tuple<>>::template type<int>
    >,
    "fxx::meta::apply_partial_t: Empty case"
);
static_assert(
    std::is_same_v<
        std::tuple<int, int>,
        typename apply_partial<std::tuple, std::tuple<int>>::template type<int>
    >,
    "fxx::meta::apply_partial_t: Forwarding case"
);

// first
static_assert(
    !first<tautology, std::tuple<>>::value,
    "fxx::meta::first: Empty case"
);
static_assert(
    first<tautology, std::tuple<int>>::value
    && first<tautology, std::tuple<int>>::index == 0,
    "fxx::meta::first: Trivial case"
);
static_assert(
    first<std::is_signed, std::tuple<ushort, int, long>>::value
    && first<std::is_signed, std::tuple<ushort, int, long>>::index == 1,
    "fxx::meta::first: Recursive case"
);

// find
static_assert(
    !find<int, std::tuple<>>::value,
    "fxx::meta::find: Empty case"
);
static_assert(
    find<int, std::tuple<int>>::value
    && find<int, std::tuple<int>>::index == 0,
    "fxx::meta::find: Trivial case"
);
static_assert(
    find<int, std::tuple<short, int, long>>::value
    && find<int, std::tuple<short, int, long>>::index == 1,
    "fxx::meta::find: Recursive case"
);

// tuple_cat_t
static_assert(
    std::is_same_v<std::tuple<>, tuple_cat_t<>>,
    "fxx::meta::tuple_cat_t: Empty case"
);
static_assert(
    std::is_same_v<
        std::tuple<int, int, int&>,
        tuple_cat_t<std::tuple<int>, std::tuple<>, std::tuple<int, int&>>
    >,
    "fxx::meta::tuple_cat_t: Regular case"
);

// tuple_flip_t
static_assert(
    std::is_same_v<std::tuple<>, tuple_flip_t<std::tuple<>>>,
    "fxx::meta::tuple_flip_t: Empty case"
);
static_assert(
    std::is_same_v<std::tuple<int>, tuple_flip_t<std::tuple<int>>>,
    "fxx::meta::tuple_flip_t: Trivial case"
);
static_assert(
    std::is_same_v<std::tuple<int&&,int&,int>, tuple_flip_t<std::tuple<int,int&,int&&>>>,
    "fxx::meta::tuple_flip_t: Regular case"
);

// tuple_pick_t
static_assert(
    std::is_same_v<std::tuple<>, tuple_pick_t<std::tuple<int,int&,int&&>>>,
    "fxx::meta::tuple_pick_t: Trivial case"
);
static_assert(
    std::is_same_v<
        std::tuple<int&&, int&, int&&>,
        tuple_pick_t<std::tuple<int,int&,int&&>, 2, 1, 2>
    >,
    "fxx::meta::tuple_pick_t: Regular case"
);

// tuple_dup_t
static_assert(
    std::is_same_v<std::tuple<>, tuple_dup_t<10, std::tuple<>>>,
    "fxx::meta::tuple_dup_t: Trivial case"
);
static_assert(
    std::is_same_v<
        std::tuple<int,int&,int&&,int,int&,int&&>,
        tuple_dup_t<2, std::tuple<int,int&,int&&>>
    >,
    "fxx::meta::tuple_dup_t: Regular case"
);

// tuple_skip_t
static_assert(
    std::is_same_v<
        std::tuple<>,
        tuple_skip_t<3, std::tuple<int,int&,int&&>>
    >,
    "fxx::meta::tuple_skip_t: Trivial case"
);
static_assert(
    std::is_same_v<
        std::tuple<int&&>,
        tuple_skip_t<2, std::tuple<int,int&,int&&>>
    >,
    "fxx::meta::tuple_skip_t: Regular case"
);
/*
static_assert(
    std::is_same_v<
        std::tuple<>,
        tuple_skip_t<4, std::tuple<int,int&,int&&>>
    >,
    "fxx::meta::tuple_skip_t: Overflow case"
);
*/

// tuple_take_t
static_assert(
    std::is_same_v<
        std::tuple<>,
        tuple_take_t<0, std::tuple<int,int&,int&&>>
    >,
    "fxx::meta::tuple_take_t: Trivial case"
);
static_assert(
    std::is_same_v<
        std::tuple<int,int&>,
        tuple_take_t<2, std::tuple<int,int&,int&&>>
    >,
    "fxx::meta::tuple_take_t: Regular case"
);
/*
static_assert(
    std::is_same_v<
        std::tuple<int,int&,int&&>,
        tuple_take_t<4, std::tuple<int,int&,int&&>>
    >,
    "fxx::meta::tuple_take_t: Overflow case"
);
*/

// tuple_slice_t
static_assert(
    std::is_same_v<
        std::tuple<>,
        tuple_slice_t<2, 0, std::tuple<int,int&,int&&>>
    >,
    "fxx::meta::tuple_slice_t: Trivial case"
);
static_assert(
    std::is_same_v<
        std::tuple<int&>,
        tuple_slice_t<1, 1, std::tuple<int,int&,int&&>>
    >,
    "fxx::meta::tuple_slice_t: Regular case"
);

// tuple_map_t
static_assert(
    std::is_same_v<std::tuple<>, tuple_map_t<std::remove_reference_t, std::tuple<>>>,
    "fxx::meta::tuple_map_t: Empty case"
);
static_assert(
    std::is_same_v<
        std::tuple<int, int, int>,
        tuple_map_t<std::remove_reference_t, std::tuple<int, int&, int&&>>
    >,
    "fxx::meta::tuple_map_t: Regular case"
);

// tuple_reduce_t
static_assert(
    std::is_same_v<int, tuple_reduce_t<std::tuple, std::tuple<int>>>,
    "fxx::meta::tuple_reduce_t: Trivial case"
);
static_assert(
    std::is_same_v<
        std::tuple<std::tuple<int,int&>,int&&>,
        tuple_reduce_t<std::tuple, std::tuple<int,int&,int&&>>
    >,
    "fxx::meta::tuple_reduce_t: Regular case"
);

// tuple_fold_t
static_assert(
    std::is_same_v<int, tuple_fold_t<std::tuple, int, std::tuple<>>>,
    "fxx::meta::tuple_fold_t: Trivial case"
);
static_assert(
    std::is_same_v<
        std::tuple<std::tuple<int,int&>,int&&>,
        tuple_fold_t<std::tuple, int, std::tuple<int&,int&&>>
    >,
    "fxx::meta::tuple_fold_t: Regular case"
);

// tuple_filter_t
static_assert(
    std::is_same_v<std::tuple<>, tuple_filter_t<std::is_reference, std::tuple<>>>,
    "fxx::meta::tuple_filter_t: Trivial case"
);
static_assert(
    std::is_same_v<
        std::tuple<int&,int&&>,
        tuple_filter_t<std::is_reference, std::tuple<int,int&,int&&>>
    >,
    "fxx::meta::tuple_filter_t: Regular case"
);

} } // namespace fxx::meta

#endif