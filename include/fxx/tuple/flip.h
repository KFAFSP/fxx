/** Implements std::tuple flipping.
 *
 * @file        tuple/flip.h
 * @author      Karl F. A. Friebel (karl.friebel@friebelnet.de)
 *
 * @version     0.1
 * @date        2019-06-05
 *
 * @copyright   Copyright (c) 2019
 */

#ifndef FXX_TUPLE_FLIP_H
#define FXX_TUPLE_FLIP_H
#pragma once

#include <tuple>
// std::tuple_cat
#include <utility>
// std::forward

#include <cstddef>
// std::size_t

namespace fxx { namespace tuple {

/** Functor for flipping std::tuples.
 *
 * @todo    Adapt documentation from fxx::meta::tuple_flip_t.
 */
struct flip_f {
    template<class Tuple, std::size_t N = std::tuple_size_v<Tuple>>
    constexpr auto operator()(Tuple&& tuple) noexcept {
        if constexpr (N == 0) {
            return std::tuple<>{};
        } else {
            return std::tuple_cat(
                std::tuple<std::tuple_element_t<N-1, Tuple>>(
                    std::get<N-1>(std::forward<Tuple>(tuple))
                ),
                flip_f{}.operator()<Tuple, N-1>(std::forward<Tuple>(tuple))
            );
        }
    }
};

/** Flip (reverse the element order of) a std::tuple.
 *
 * @todo    Adapt documentation from fxx::meta::tuple_flip_t.
 *
 * @tparam  Tuple   Input tuple type.
 *
 * @param   [in]    tuple   Input tuple.
 *
 * @return  Return tuple.
 */
template<class Tuple>
constexpr auto flip(Tuple&& tuple) noexcept {
    return flip_f{}(std::forward<Tuple>(tuple));
}

} } // namespace fxx::tuple

#endif