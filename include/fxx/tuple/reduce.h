/** Implements std::tuple reduction.
 *
 * @file        reduce.h
 * @author      Karl F. A. Friebel (karl.friebel@friebelnet.de)
 *
 * @version     0.1
 * @date        2019-06-05
 *
 * @copyright   Copyright (c) 2019
 */

#ifndef FXX_TUPLE_REDUCE_H
#define FXX_TUPLE_REDUCE_H
#pragma once

#include <tuple>
// std::(get, tuple_element_t, tuple_size_v)
#include <utility>
// std::forward

namespace fxx { namespace tuple {

namespace detail {

// Recursive case.
template<std::size_t N>
struct reduce_impl {
    static_assert(N > 0, "0-Tuple is irreducible!");

    template<class Fn, class Tuple>
    static constexpr auto reduce(Fn&& fn, Tuple&& tuple) -> decltype(
        fn(
            reduce_impl<N-1>::reduce(std::forward<Fn>(fn), std::forward<Tuple>(tuple)),
            std::get<N-1>(std::forward<Tuple>(tuple))
        )
    ) {
        return fn(
            reduce_impl<N-1>::reduce(std::forward<Fn>(fn), std::forward<Tuple>(tuple)),
            std::get<N-1>(std::forward<Tuple>(tuple))
        );
    }
};

// Abort case.
template<>
struct reduce_impl<1> {
    template<class Fn, class Tuple>
    static constexpr std::tuple_element_t<0, Tuple> reduce(Fn&&, Tuple&& tuple) {
        return std::get<0>(std::forward<Tuple>(tuple));
    }
};

} // namespace detail

/** Functor for reducing a std::tuple.
 *
 * @todo    Adapt documentation from fxx::meta::tuple_reduce_t.
 */
struct reduce_f {
    template<class Fn, class Tuple>
    constexpr auto operator()(Fn&& fn, Tuple&& tuple) -> decltype(
        detail::reduce_impl<std::tuple_size_v<Tuple>>::reduce(
            std::forward<Fn>(fn),
            std::forward<Tuple>(tuple)
        )
    ) {
        return detail::reduce_impl<std::tuple_size_v<Tuple>>::reduce(
            std::forward<Fn>(fn),
            std::forward<Tuple>(tuple)
        );
    }
};

/** Reduce a std::tuple.
 *
 * @todo    Adapt documentation from fxx::meta::tuple_reduce_t.
 *
 * @tparam  Fn      Reduction function type.
 * @tparam  Tuple   Input tuple type.
 *
 * @param   [in]    fn      Reduction function.
 * @param   [in]    tuple   Input tuple.
 *
 * @return  Result.
 */
template<class Fn, class Tuple>
constexpr auto reduce(Fn&& fn, Tuple&& tuple) -> decltype(
    reduce_f{}(std::forward<Fn>(fn), std::forward<Tuple>(tuple))
) {
    return reduce_f{}(std::forward<Fn>(fn), std::forward<Tuple>(tuple));
}

} } // namespace fxx::tuple

#endif