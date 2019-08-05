/** Implements std::tuple folding.
 *
 * @file        tuple/fold.h
 * @author      Karl F. A. Friebel (karl.friebel@friebelnet.de)
 *
 * @version     0.1
 * @date        2019-08-05
 *
 * @copyright   Copyright (c) 2019
 */

#ifndef FXX_TUPLE_FOLD_H
#define FXX_TUPLE_FOLD_H
#pragma once

#include <tuple>
// std::(get, tuple_element_t, tuple_size_v)
#include <utility>
// std::forward

namespace fxx { namespace tuple {

namespace detail {

// Recursive case.
template<std::size_t N>
struct fold_impl {
    template<class Fn, class Init, class Tuple>
    static constexpr auto fold(Fn&& fn, Init&& init, Tuple&& tuple) -> decltype(
        fn(
            fold_impl<N-1>::fold(
                std::forward<Fn>(fn),
                std::forward<Init>(init),
                std::forward<Tuple>(tuple)
            ),
            std::get<N-1>(std::forward<Tuple>(tuple))
        )
    ) {
        return fn(
            fold_impl<N-1>::fold(
                std::forward<Fn>(fn),
                std::forward<Init>(init),
                std::forward<Tuple>(tuple)
            ),
            std::get<N-1>(std::forward<Tuple>(tuple))
        );
    }
};

// Abort case.
template<>
struct fold_impl<0> {
    template<class Fn, class Init, class Tuple>
    static constexpr Init fold(Fn&&, Init&& init, Tuple&& tuple) {
        return std::forward<Init>(init);
    }
};

} // namespace detail

/** Functor for folding a std::tuple.
 *
 * @todo    Adapt documentation from fxx::meta::tuple_fold_t.
 */
struct fold_f {
    template<class Fn, class Init, class Tuple>
    constexpr auto operator()(Fn&& fn, Init&& init, Tuple&& tuple) -> decltype(
        detail::fold_impl<std::tuple_size_v<Tuple>>::fold(
            std::forward<Fn>(fn),
            std::forward<Init>(init),
            std::forward<Tuple>(tuple)
        )
    ) {
        return detail::fold_impl<std::tuple_size_v<Tuple>>::fold(
            std::forward<Fn>(fn),
            std::forward<Init>(init),
            std::forward<Tuple>(tuple)
        );
    }
};

/** Fold a std::tuple.
 *
 * @todo    Adapt documentation from fxx::meta::tuple_fold_t.
 *
 * @tparam  Fn      Reduction function type.
 * @tparam  Init    Initialization value type.
 * @tparam  Tuple   Input tuple type.
 *
 * @param   [in]    fn      Reduction function.
 * @param   [in]    init    Initialization value.
 * @param   [in]    tuple   Input tuple.
 *
 * @return  Result.
 */
template<class Fn, class Init, class Tuple>
constexpr auto fold(Fn&& fn, Init&& init, Tuple&& tuple) -> decltype(
    fold_f{}(std::forward<Fn>(fn), std::forward<Init>(init), std::forward<Tuple>(tuple))
) {
    return fold_f{}(std::forward<Fn>(fn), std::forward<Init>(init), std::forward<Tuple>(tuple));
}

} } // namespace fxx::tuple

#endif