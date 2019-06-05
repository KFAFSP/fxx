/** Implements std::tuple mapping.
 *
 * @file        map.h
 * @author      Karl F. A. Friebel (karl.friebel@friebelnet.de)
 *
 * @version     0.1
 * @date        2019-06-05
 *
 * @copyright   Copyright (c) 2019
 */

#ifndef FXX_TUPLE_MAP_H
#define FXX_TUPLE_MAP_H
#pragma once

#include <tuple>
// std::get
#include <utility>
// std::(forward, index_sequence, make_index_sequence)

namespace fxx { namespace tuple {

namespace detail {

template<class Fn, class Tuple, std::size_t... Ns>
static constexpr auto map_impl(Fn&& fn, Tuple&& tuple, std::index_sequence<Ns...>) {
    return std::tuple<
        decltype(fn(std::declval<std::tuple_element_t<Ns, Tuple>>()))...
    >(
        fn(std::get<Ns>(std::forward<Tuple>(tuple)))...
    );
}

} // namespace detail

/** Functor for mapping std::tuple elements.
 *
 * @todo    Adapt documentation from fxx::meta::tuple_map_t.
 */
struct map_f {
    template<class Fn, class Tuple>
    constexpr auto operator()(Fn&& fn, Tuple&& tuple) {
        return detail::map_impl(
            std::forward<Fn>(fn),
            std::forward<Tuple>(tuple),
            std::make_index_sequence<std::tuple_size_v<Tuple>>{}
        );
    }
};

/** Map std::tuple elements using a function.
 *
 * @todo    Adapt documentation from fxx::meta::tuple_map_t.
 *
 * @tparam  Fn      Mapping function type.
 * @tparam  Tuple   Input tuple type.
 *
 * @param   [in]    fn      Mapping function.
 * @param   [in]    tuple   Input tuple.
 *
 * @return  Result tuple.
 */
template<class Fn, class Tuple>
constexpr auto map(Fn&& fn, Tuple&& tuple) {
    return map_f{}(std::forward<Fn>(fn), std::forward<Tuple>(tuple));
}

} } // namespace fxx::tuple

#endif