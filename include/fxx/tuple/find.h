/** Implements std::tuple element searching.
 *
 * @file        tuple/find.h
 * @author      Karl F. A. Friebel (karl.friebel@friebelnet.de)
 *
 * @version     0.1
 * @date        2019-08-05
 *
 * @copyright   Copyright (c) 2019
 */

#ifndef FXX_TUPLE_FIND_H
#define FXX_TUPLE_FIND_H
#pragma once

#include <fxx/tuple/first.h>
// fxx::tuple::first

#include <utility>
// std::forward

namespace fxx { namespace tuple {

/** Functor for finding std::tuple elements.
 *
 * @todo    Adapt documentation from fxx::meta::find.
 */
struct find_f {
    template<class T, class Tuple>
    constexpr auto operator()(T&& value, Tuple&& tuple) {
        return first(
            [&value](auto&& x) { return value == x; },
            std::forward<Tuple>(tuple)
        );
    }
};

/** Find the first occurence of an element in a std::tuple.
 *
 * @todo    Adapt documentation from fxx::meta::find.
 *
 * @tparam  T       Element type.
 * @tparam  Tuple   Input tuple type.
 *
 * @param   [in]    value   Value.
 * @param   [in]    tuple   Input tuple.
 *
 * @return  std::optional<std::size_t>
 */
template<class T, class Tuple>
constexpr auto find(T&& value, Tuple&& tuple) {
    return find_f{}(std::forward<T>(value), std::forward<Tuple>(tuple));
}

} } // namespace fxx::tuple

#endif