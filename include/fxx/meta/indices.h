/** Extensions for the std::index_sequence type.
 *
 * std::index_sequences can be used to express access patterns or selectors for statically indexable
 * containers, i.e. std::tuple.
 *
 * The types declared in this file can be used to manipulate these sequences to achieve complex
 * mappings independent of the container access functions.
 *
 * @file        indices.h
 * @author      Karl F. A. Friebel (karl.friebel@friebelnet.de)
 *
 * @version     0.4
 * @date        2019-06-04
 *
 * @copyright   Copyright (c) 2019
 */

#ifndef FXX_META_INDICES_H
#define FXX_META_INDICES_H
#pragma once

#include <utility>
// std::(index_sequence, make_index_sequence)
#include <type_traits>
// std::integral_constant

#include <cstddef>
// std::size_t

namespace fxx { namespace meta {

/// @cond
namespace detail {

// Dispatch case.
template<template<std::size_t...> class, class>
struct apply_index_sequence_impl {};

// Variadic case.
template<template<std::size_t...> class Target, std::size_t... Ns>
struct apply_index_sequence_impl<Target, std::index_sequence<Ns...>> {
    using type = Target<Ns...>;
};

// Dispatch case.
template<template<std::size_t> class, class>
struct map_index_sequence_impl {};

// Variadic case.
template<template<std::size_t> class Fn, std::size_t... Ns>
struct map_index_sequence_impl<Fn, std::index_sequence<Ns...>> {
    using type = std::index_sequence<Fn<Ns>::value...>;
};

template<std::size_t Shift>
struct shift_index_impl {
    template<std::size_t N>
    using type = std::integral_constant<std::size_t, N + Shift>;
};

template<std::size_t Scale>
struct scale_index_impl {
    template<std::size_t N>
    using type = std::integral_constant<std::size_t, N * Scale>;
};

} // namespace detail
/// @endcond

/** Apply elements of an std::index_sequence to a template.
 *
 * @code{.unparsed}
 * apply_index_sequence_t<T, s> = T<s_0, s_1, ... s_(N-1)>
 *
 *      where s is the input sequence.
 *        and N is the length of the input sequence.
 * @endcode
 *
 * @warning Behavior is undefined when @p Seq is not an std::index_sequence.
 *
 * @tparam  Target  Target template.
 * @tparam  Seq     Input sequence.
 */
template<template<std::size_t...> class Target, class Seq>
using apply_index_sequence_t = typename detail::apply_index_sequence_impl<Target, Seq>::type;

/** Map the elements of an std::index_sequence.
 *
 * Mapping is defined here as obtaining a new sequence by projecting all elements using the given
 * template that resolves to an std::integral_constant<std::size_t>. The output sequence will have
 * the same size as the input sequence.
 *
 * @code{.unparsed}
 * map_index_sequence_t<Fn, s> = std::index_sequence<Fn<s_0>, Fn<s_1>, ..., Fn<s_(N-1)>>
 *
 *      where s is the input sequence.
 *        and N is the length of the input sequence.
 * @endcode
 *
 * @warning Behavior is undefined when instances of @p Fn do not declare a
 *           `static constexpr std::size_t value`.
 * @warning Behavior is undefined when @p Seq is not an std::index_sequence.
 *
 * @tparam  Fn      Mapping template.
 * @tparam  Seq     Input sequence.
 */
template<template<std::size_t> class Fn, class Seq>
using map_index_sequence_t = typename detail::map_index_sequence_impl<Fn, Seq>::type;

/** Shift the elements in an std::index_sequence.
 *
 * Shifting is defined here as obtaining a new sequence by adding a constant offset to all elements
 * of the input sequence. The output sequence will have the same size as the input sequence.
 *
 * @code{.unparsed}
 * shift_index_sequence_t<A, s> = std::index_sequence<s_0 + A, s_1 + A, ...,s_(N-1) + A>
 *
 *      where s is the input sequence.
 *        and N is the length of the input sequence.
 * @endcode
 *
 * @warning Behavior is undefined when @p Seq is not an std::index_sequence.
 *
 * @tparam  Shift   Constant shift summand.
 * @tparam  Seq     Input sequence.
 */
template<std::size_t Shift, class Seq>
using shift_index_sequence_t = map_index_sequence_t<
        detail::shift_index_impl<Shift>::template type,
        Seq
    >;

/** Scale the elements in an std::index_sequence.
 *
 * Scaling is defined here as obtaining a new sequence by multiplying all elements of the input
 * sequence with a constant factor. The output sequence will have the same size as the input
 * sequence.
 *
 * @code{.unparsed}
 * scale_index_sequence_t<A, s> = std::index_sequence<s_0 * A, s_1 * A, ...,s_(N-1) * A>
 *
 *      where s is the input sequence.
 *        and N is the length of the input sequence.
 * @endcode
 *
 * @warning Behavior is undefined when @p Seq is not an std::index_sequence.
 *
 * @tparam  Scale   Constant scale factor.
 * @tparam  Seq     Input sequence.
 */
template<std::size_t Scale, class Seq>
using scale_index_sequence_t = map_index_sequence_t<
        detail::scale_index_impl<Scale>::template type,
        Seq
    >;

/** Make an index range.
 *
 * @code{.unparsed}
 * make_index_range<S, L> = std::index_range<S, S + 1, ..., S + L - 1>
 * @endcode
 *
 * @tparam  Start   Start of the range.
 * @tparam  Length  Length of the range.
 */
template<std::size_t Start, std::size_t Length>
using make_index_range = shift_index_sequence_t<Start, std::make_index_sequence<Length>>;

} } // namespace fxx::meta

#endif

//--------------------------------------------------------------------------------------------------
// VERIFICATION USING STATIC ASSERTIONS
//--------------------------------------------------------------------------------------------------

#ifdef FXX_TEST_STATIC

namespace fxx { namespace meta {

// apply_index_sequence_t
static_assert(
    std::is_same_v<
        std::index_sequence<>,
        apply_index_sequence_t<std::index_sequence, std::make_index_sequence<0>>
    >,
    "fxx::meta::apply_index_sequence_t: Empty case"
);
static_assert(
    std::is_same_v<
        std::index_sequence<1, 3, 9, 4>,
        apply_index_sequence_t<std::index_sequence, std::index_sequence<1, 3, 9, 4>>
    >,
    "fxx::meta::apply_index_sequence_t: Regular case"
);

// map_index_sequence_t

// shift_index_sequence_t
static_assert(
    std::is_same_v<
        std::index_sequence<>,
        shift_index_sequence_t<1, std::make_index_sequence<0>>
    >,
    "fxx::meta::shift_index_sequence_t: Trivial case"
);
static_assert(
    std::is_same_v<
        std::index_sequence<1, 2>,
        shift_index_sequence_t<1, std::make_index_sequence<2>>
    >,
    "fxx::meta::shift_index_sequence_t: Regular case"
);

// scale_index_sequence_t
static_assert(
    std::is_same_v<
        std::index_sequence<>,
        scale_index_sequence_t<1, std::make_index_sequence<0>>
    >,
    "fxx::meta::scale_index_sequence_t: Trivial case"
);
static_assert(
    std::is_same_v<
        std::index_sequence<0, 2, 4>,
        scale_index_sequence_t<2, std::make_index_sequence<3>>
    >,
    "fxx::meta::scale_index_sequence_t: Regular case"
);

// make_index_range
static_assert(
    std::is_same_v<std::index_sequence<>, make_index_range<4, 0>>,
    "fxx::meta::make_index_range: Trivial case"
);
static_assert(
    std::is_same_v<std::index_sequence<3, 4, 5>, make_index_range<3, 3>>,
    "fxx::meta::make_index_range: Regular case"
);

} } // namespace fxx::meta

#endif