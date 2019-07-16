/** Implements std::tuple element skipping.
 *
 * @file        tuple/skip.h
 * @author      Karl F. A. Friebel (karl.friebel@friebelnet.de)
 *
 * @version     0.1
 * @date        2019-06-05
 *
 * @copyright   Copyright (c) 2019
 */

#ifndef FXX_TUPLE_SKIP_H
#define FXX_TUPLE_SKIP_H
#pragma once

#include <fxx/meta/indices.h>
// fxx::meta::(apply_index_sequence_t, make_index_range)

#include <fxx/tuple/pick.h>
// fxx::tuple::pick_f

namespace fxx { namespace tuple {

/** Functor for skipping std::tuple elements.
 *
 * @todo    Adapt documentation from fxx::meta::tuple_skip_t.
 *
 * @tparam  N   Number of elements to skip.
 */
template<std::size_t N>
struct skip_f {
    template<class Tuple>
    constexpr auto operator()(Tuple&& tuple) noexcept {
        using idx_range_t = fxx::meta::make_index_range<N, std::tuple_size_v<Tuple> - N>;
        using pick_f_t = fxx::meta::apply_index_sequence_t<pick_f, idx_range_t>;
        return pick_f_t{}(std::forward<Tuple>(tuple));
    }
};

/** Skip the first @p N std::tuple elements.
 *
 * @todo    Adapt documentation from fxx::meta::tuple_skip_t.
 *
 * @tparam  N       Number of elements to skip.
 * @tparam  Tuple   Input tuple type.
 *
 * @param   [in]    tuple   Input tuple.
 *
 * @return  Result tuple.
 */
template<std::size_t N, class Tuple>
constexpr auto skip(Tuple&& tuple) noexcept {
    return skip_f<N>{}(std::forward<Tuple>(tuple));
}

} } // namespace fxx::tuple

#endif