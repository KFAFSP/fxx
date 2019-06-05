/** Implements std::tuple slicing.
 *
 * @file        tuple/slice.h
 * @author      Karl F. A. Friebel (karl.friebel@friebelnet.de)
 *
 * @version     0.1
 * @date        2019-06-05
 *
 * @copyright   Copyright (c) 2019
 */

#ifndef FXX_TUPLE_SLICE_H
#define FXX_TUPLE_SLICE_H
#pragma once

#include <fxx/meta/indices.h>
// fxx::meta::(apply_index_sequence_t, make_index_range)

#include <fxx/tuple/pick.h>
// fxx::tuple::pick_f

namespace fxx { namespace tuple {

/** Functor for std::tuple slicing.
 *
 * @todo    Adapt documentation from fxx::meta::tuple_slice_t.
 *
 * @tparam  Start   Start index.
 * @tparam  Length  Range length.
 */
template<std::size_t Start, std::size_t Length>
struct slice_f {
    template<class Tuple>
    constexpr auto operator()(Tuple&& tuple) noexcept {
        using idx_range_t = fxx::meta::make_index_range<Start, Length>;
        using pick_f_t = fxx::meta::apply_index_sequence_t<pick_f, idx_range_t>;
        return pick_f_t{}(std::forward<Tuple>(tuple));
    }
};

/** Slice a range of std::tuple elements.
 *
 * @todo    Adapt documentation from fxx::meta::tuple_slice_t.
 *
 * @tparam  Start   Start index.
 * @tparam  Length  Range length.
 * @tparam  Tuple   Input tuple type.
 *
 * @param   [in]    tuple   Input tuple.
 *
 * @return  Result tuple.
 */
template<std::size_t Start, std::size_t Length, class Tuple>
constexpr auto slice(Tuple&& tuple) noexcept {
    return slice_f<Start, Length>{}(std::forward<Tuple>(tuple));
}

} } // namespace fxx::tuple

#endif