/** Implements std::tuple duplication.
 *
 * @file        dup.h
 * @author      Karl F. A. Friebel (karl.friebel@friebelnet.de)
 *
 * @version     0.1
 * @date        2019-06-05
 *
 * @copyright   Copyright (c) 2019
 */

#ifndef FXX_TUPLE_DUP_H
#define FXX_TUPLE_DUP_H
#pragma once

#include <tuple>
// std::(tuple, tuple_cat)
#include <utility>
// std::forward

#include <cstddef>
// std::size_t

namespace fxx { namespace tuple {

/** Functor for duplicating std::tuples.
 *
 * @todo    Adapt documentation from fxx::meta::tuple_dup_t.
 *
 * @tparam  N   Number of duplications.
 */
// Recursive case.
template<std::size_t N>
struct dup_f {
    template<class Tuple>
    constexpr auto operator()(Tuple&& tuple) noexcept {
        return std::tuple_cat(
            std::forward<Tuple>(tuple),
            dup_f<N-1>{}(std::forward<Tuple>(tuple))
        );
    }
};

/// @cond
// Abort case
template<>
struct dup_f<0> {
    template<class Tuple>
    constexpr std::tuple<> operator()(Tuple&&) noexcept {
        return {};
    }
};
/// @endcond

/** Duplicate-concatenate an std::tuple.
 *
 * @todo    Adapt documentation from fxx::meta::tuple_dup_t.
 *
 * @tparam  N       Number of duplications.
 * @tparam  Tuple   Input tuple type.
 *
 * @param   [in]    tuple   Input tuple.
 *
 * @return  Result tuple.
 */
template<std::size_t N, class Tuple>
constexpr auto dup(Tuple&& tuple) noexcept {
    return dup_f<N>{}(std::forward<Tuple>(tuple));
}

} } // namespace fxx::tuple

#endif