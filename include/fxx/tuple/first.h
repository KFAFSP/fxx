/** Implements std::tuple element matching.
 *
 * @file        tuple/first.h
 * @author      Karl F. A. Friebel (karl.friebel@friebelnet.de)
 *
 * @version     0.1
 * @date        2019-08-05
 *
 * @copyright   Copyright (c) 2019
 */

#ifndef FXX_TUPLE_FIRST_H
#define FXX_TUPLE_FIRST_H
#pragma once

#include <optional>
// std::(nullopt, nullopt_t, optional)
#include <tuple>
// std::get
#include <type_traits>
// std::enable_if_t
#include <utility>
// std::forward

namespace fxx { namespace tuple {

namespace detail {

template<std::size_t Offset, class Pred, class Tuple>
static constexpr std::enable_if_t<(std::tuple_size_v<Tuple> <= Offset), std::nullopt_t>
first_impl(Pred&& pred, Tuple&& tuple) {
    return std::nullopt;
}

template<std::size_t Offset, class Pred, class Tuple>
static constexpr std::enable_if_t<(std::tuple_size_v<Tuple> > Offset), std::optional<std::size_t>>
first_impl(Pred&& pred, Tuple&& tuple) {
    if (static_cast<bool>(pred(std::get<Offset>(std::forward<Tuple>(tuple))))) {
        return {Offset};
    } else {
        return first_impl<Offset + 1>(std::forward<Pred>(pred), std::forward<Tuple>(tuple));
    }
}

} // namespace detail

/** Functor for matching std::tuple elements.
 *
 * @todo    Adapt documentation from fxx::meta::first.
 */
struct first_f {
    template<class Pred, class Tuple>
    constexpr auto operator()(Pred&& pred, Tuple&& tuple) {
        return detail::first_impl<0>(
            std::forward<Pred>(pred),
            std::forward<Tuple>(tuple)
        );
    }
};

/** Match std::tuple elements using a predicate.
 *
 * @todo    Adapt documentation from fxx::meta::first.
 *
 * @tparam  Pred    Predicate type.
 * @tparam  Tuple   Input tuple type.
 *
 * @param   [in]    pred    Predicate.
 * @param   [in]    tuple   Input tuple.
 *
 * @return  std::optional<std::size_t>
 */
template<class Pred, class Tuple>
constexpr auto first(Pred&& pred, Tuple&& tuple) {
    return first_f{}(std::forward<Pred>(pred), std::forward<Tuple>(tuple));
}

} } // namespace fxx::tuple

#endif