/** Implements std::tuple picking.
 *
 * @file        pick.h
 * @author      Karl F. A. Friebel (karl.friebel@friebelnet.de)
 *
 * @version     0.1
 * @date        2019-06-05
 *
 * @copyright   Copyright (c) 2019
 */

#ifndef FXX_TUPLE_PICK_H
#define FXX_TUPLE_PICK_H
#pragma once

#include <tuple>
// std::(tuple, tuple_cat)
#include <utility>
// std::forward

#include <cstddef>
// std::size_t

namespace fxx { namespace tuple {

/** Functor for picking from an std::tuple.
 *
 * @todo    Adapt documentation from fxx::meta::tuple_pick_t.
 *
 * @tparam  Ns      Picking indices.
 */
// Forward declaration.
template<std::size_t... Ns>
struct pick_f {};

/// @cond
// Recursive case.
template<std::size_t Head, std::size_t... Tail>
struct pick_f<Head, Tail...> {
    template<class Tuple>
    constexpr auto operator()(Tuple&& tuple) noexcept {
        return std::tuple_cat(
            std::tuple<std::tuple_element_t<Head, Tuple>>(
                std::get<Head>(std::forward<Tuple>(tuple))
            ),
            pick_f<Tail...>{}(std::forward<Tuple>(tuple))
        );
    }
};

// Abort case.
template<>
struct pick_f<> {
    template<class Tuple>
    constexpr std::tuple<> operator()(Tuple&& tuple) noexcept {
        return {};
    }
};
/// @endcond

/** Pick elements from an std::tuple.
 *
 * @todo    Adapt documentation from fxx::meta::tuple_pick_t.
 *
 * @note    This is a `constexpr` lambda so that the tuple type will be deduced by late binding.
 *
 * @tparam  Ns      Picking indices.
 */
template<std::size_t... Ns>
static constexpr auto pick = [](auto&& tuple) constexpr noexcept {
    return pick_f<Ns...>{}(std::forward<decltype(tuple)>(tuple));
};

} } // namespace fxx::tuple

#endif