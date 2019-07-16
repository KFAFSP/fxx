/** Implements std::tuple element taking.
 *
 * @file        tuple/take.h
 * @author      Karl F. A. Friebel (karl.friebel@friebelnet.de)
 *
 * @version     0.1
 * @date        2019-06-05
 *
 * @copyright   Copyright (c) 2019
 */

#ifndef FXX_TUPLE_TAKE_H
#define FXX_TUPLE_TAKE_H
#pragma once

#include <tuple>
// std::(tuple, tuple_cat)
#include <utility>
// std::make_index_sequence

#include <cstddef>
// std::size_t

#include <fxx/meta/indices.h>
// fxx::meta::apply_index_sequence_t

#include <fxx/tuple/pick.h>
// fxx::tuple::pick_f

namespace fxx { namespace tuple {

/** Functor for taking std::tuple elements.
 *
 * @todo    Adapt documentation from fxx::meta::tuple_take_t
 *
 * @tparam  N   Number of elements to take.
 */
template<std::size_t N>
struct take_f {
    template<class Tuple>
    constexpr auto operator()(Tuple&& tuple) noexcept {
        using idx_seq_t = std::make_index_sequence<N>;
        using pick_f_t = fxx::meta::apply_index_sequence_t<pick_f, idx_seq_t>;
        return pick_f_t{}(std::forward<Tuple>(tuple));
    }
};

/** Take the first \@p N std::tuple elements.
 *
 * @todo    Adapt documentation from fxx::meta::tuple_take_t
 *
 * @tparam  N       Number of elements to take.
 * @tparam  Tuple   Input tuple type.
 *
 * @param   [in]    tuple   Input tuple.
 *
 * @return  Result tuple.
 */
template<std::size_t N, class Tuple>
constexpr auto take(Tuple&& tuple) noexcept {
    return take_f<N>{}(std::forward<Tuple>(tuple));
}

} } // namespace fxx::tuple

#endif