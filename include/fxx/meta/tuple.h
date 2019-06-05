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
 *    make_tuple_t ... Turn a variadic parameter pack into a std::tuple type.
 *
 * Consuming:
 *
 *          apply_t ... Forward std::tuple types as variadic parameters to another template.
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
 *    tuple_foldl_t ... Fold a std::tuple type from the left.
 *    tuple_foldr_t ... Fold a std::tuple type from the right.
 *   tuple_filter_t ... Filter a std::tuple based on type.
 * @endcode
 *
 * @file        meta/tuple.h
 *
 * @author      Karl F. A. Friebel (karl.friebel@friebelnet.de)
 *
 * @version     0.3
 * @date        2019-06-04
 *
 * @copyright   Copyright (c) 2019
 */

#ifndef FXX_META_TUPLE_H
#define FXX_META_TUPLE_H
#pragma once

#include <tuple>
// std::(tuple, tuple_element_t)
#include <type_traits>
// std::conditional_t

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
template<template<class> class Fn, std::size_t N, class Tuple>
struct tuple_filter_impl {
    using pred = typename tuple_filter_impl<Fn, N-1, Tuple>::type;
    using cur = std::tuple_element_t<N-1, Tuple>;
    using flt = std::conditional_t<Fn<cur>::value, std::tuple<cur>, std::tuple<>>;
    using type = typename tuple_cat_impl<pred, flt>::type;
};

// Abort case.
template<template<class> class Fn, class Tuple>
struct tuple_filter_impl<Fn, 0, Tuple> {
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
 *     where t is the input tuple type
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
using tuple_foldl_t = tuple_reduce_t<Fn, tuple_cat_t<std::tuple<Init>, Tuple>>;

/** Get the result type of right-folding a std::tuple.
 *
 * Right-folding is defined here as Haskell's foldl, meaning that a binary fold operation visits all
 * tuple elements from right-to-left, starting with the initialization element.
 *
 * @code{.unparsed}
 * tuple_foldr_t<Fn, I, t> = Fn<Fn<Fn<Fn<I, t_(N-1)>, ...>, t_1>, t_0>
 *
 *     where t is the input tuple type
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
using tuple_foldr_t = tuple_foldl_t<Fn, Init, tuple_flip_t<Tuple>>;

/** Get the result type of filtering a std::tuple by type.
 *
 * Filtering by type is defined here as evaluating a Filter predicate to an std::bool_constant for
 * each element type and constructing a new tuple by selecting only the matching elements. The
 * 0-tuple always filters to itself.
 *
 * @code{.unparsed}
 * tuple_filter_t<Fn, t> = tuple_cat_t<F_i_0, F_i_1, >
 * @endcode
 *
 * @warning Behavior is undefined when @p Fn instances do not provide a
 *          `static constexpr bool value`.
 *
 * @tparam  Fn      Predicate template std::bool_constant.
 * @tparam  Tuple   Input tuple type.
 */
template<template<class> class Fn, class Tuple>
using tuple_filter_t = typename detail::tuple_filter_impl<
        Fn,
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
// std::(is_reference_v, is_same_v, remove_reference_t)

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

// tuple_foldl_t
static_assert(
    std::is_same_v<int, tuple_foldl_t<std::tuple, int, std::tuple<>>>,
    "fxx::meta::tuple_foldl_t: Trivial case"
);
static_assert(
    std::is_same_v<
        std::tuple<std::tuple<int,int&>,int&&>,
        tuple_foldl_t<std::tuple, int, std::tuple<int&,int&&>>
    >,
    "fxx::meta::tuple_foldl_t: Regular case"
);

// tuple_foldr_t
static_assert(
    std::is_same_v<int, tuple_foldr_t<std::tuple, int, std::tuple<>>>,
    "fxx::meta::tuple_foldr_t: Trivial case"
);
static_assert(
    std::is_same_v<
        std::tuple<std::tuple<int,int&>,int&&>,
        tuple_foldr_t<std::tuple, int, std::tuple<int&&,int&>>
    >,
    "fxx::meta::tuple_foldr_t: Regular case"
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