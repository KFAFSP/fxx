/** An implementation of the detection idiom.
 *
 * A detector is a compile-time expression that examines whether an expression is well-defined, and
 * what type it evaluates to. It can be used to detect whether members and operators are defined.
 *
 * Largely based on the libary fundamentals TS v2, with some additions.
 *
 * @file        detect.h
 * @author      Karl F. A. Friebel (karl.friebel@friebelnet.de)
 *
 * @version     0.1
 * @date        2019-07-17
 *
 * @copyright   Copyright (c) 2019
 */

#ifndef FXX_META_DETECT_H
#define FXX_META_DETECT_H
#pragma once

#include <type_traits>
// std::(false_type, is_convertible, true_type, void_t)

#include <fxx/cxx/is_nothrow_convertible.h>
// std::is_nothrow_convertible

namespace fxx { namespace meta {

namespace detail {

// Empty type without any members.
struct nonesuch {
    nonesuch() = delete;
    nonesuch(const nonesuch&) = delete;
    nonesuch(nonesuch&&) = delete;
    nonesuch& operator=(const nonesuch&) = delete;
    nonesuch& operator=(nonesuch&&) = delete;
};

template<class Default, class, template<class...> class Op, class... Args>
struct detect_impl {
    using value_t = std::false_type;
    using type = Default;
};

template<class Default, template<class...> class Op, class... Args>
struct detect_impl<Default, std::void_t<Op<Args...>>, Op, Args...> {
    using value_t = std::true_type;
    using type = Op<Args...>;
};

} // namespace detail

/** An std::bool_constant indicating whether a template instantiation is valid.
 *
 * Can be used to detect whether certain expressions are valid by use of a template alias that
 * evaluates to the expression's decltype.
 *
 * @code{.unparsed}
 * template<class Lhs, class Rhs = Lhs>
 * using add_t = decltype(std::declval<const Lhs&>() + std::declval<const Rhs&>());
 *
 * is_detected<add_t, int>::value == true
 * @endcode
 *
 * @tparam  Op      Test template.
 * @tparam  Args    Arguments to @p Op.
 */
template <template<class...> class Op, class... Args>
using is_detected = typename detail::detect_impl<detail::nonesuch, void, Op, Args...>::value_t;

/** A compile-time constant indicating whether a template instantiation is valid.
 *
 * See is_detected for more details.
 *
 * @tparam  Op      Test template.
 * @tparam  Args    Arguments to @p Op.
 */
template <template<class...> class Op, class... Args>
static constexpr bool is_detected_v = is_detected<Op, Args...>::value;

/** An alias that evaluates to the given template instantiation, or the default if invalid.
 *
 * See is_detected for more details.
 *
 * @tparam  Default     Default type.
 * @tparam  Op          Test template.
 * @tparam  Args        Arguments to @p Op.
 */
template <class Default, template<class...> class Op, class... Args>
using detected_or_t = typename detail::detect_impl<Default, void, Op, Args...>::type;

/** An alias that evaluates to the given template instantiation, or an empty memberless type if
 * invalid.
 *
 * See detected_or_t for more details.
 *
 * @tparam  Op      Test template.
 * @tparam  Args    Arguments to @p Op.
 */
template <template<class...> class Op, class... Args>
using detected_t = detected_or_t<detail::nonesuch, Op, Args...>;

/** An std::bool_constant indicating whether a template instantiation is valid and evaluates to the
 * given type.
 *
 * See std::is_same and detected_t for more details.
 *
 * @tparam  T       Expected type.
 * @tparam  Op      Test template.
 * @tparam  Args    Arguments to @p Op.
 */
template<class T, template<class...> class Op, class... Args>
using is_detected_exact = std::is_same<detected_t<Op, Args...>, T>;

/** A compile-time constant indicating whether a template instantiation is valid and evaluates to
 * the given type.
 *
 * See is_detected_exact for more details.
 *
 * @tparam  T       Expected type.
 * @tparam  Op      Test template.
 * @tparam  Args    Arguments to @p Op.
 */
template<class T, template<class...> class Op, class... Args>
static constexpr bool is_detected_exact_v = is_detected_exact<T, Op, Args...>::value;

/** An std::bool_constant indicating whether a template instantiation is valid and evalutes to a
 * type that is convertible to the given type.
 *
 * See std::is_convertible and detected_t for more details.
 *
 * @tparam  To      Target type.
 * @tparam  Op      Test template.
 * @tparam  Args    Arguments to @p Op.
 */
template<class To, template<class...> class Op, class... Args>
using is_detected_convertible = std::is_convertible<detected_t<Op, Args...>, To>;

/** A compile-time constant indicating whether a template instantiation is valid and evalutes to a
 * type that is convertible to the given type.
 *
 * See is_detected_convertible for more details.
 *
 * @tparam  To      Target type.
 * @tparam  Op      Test template.
 * @tparam  Args    Arguments to @p Op.
 */
template<class To, template<class...> class Op, class... Args>
static constexpr bool is_detected_convertible_v = is_detected_convertible<To, Op, Args...>::value;

/** An std::bool_constant indicating whether a template instantiation is valid and evalutes to a
 * type that is nothrow convertible to the given type.
 *
 * See std::is_nothrow_convertible and detected_t for more details.
 *
 * @tparam  To      Target type.
 * @tparam  Op      Test template.
 * @tparam  Args    Arguments to @p Op.
 */
template<class To, template<class...> class Op, class... Args>
using is_detected_nothrow_convertible = std::is_nothrow_convertible<detected_t<Op, Args...>, To>;

/** A compile-time constant indicating whether a template instantiation is valid and evalutes to a
 * type that is nothrow convertible to the given type.
 *
 * See is_detected_nothrow_convertible for more details.
 *
 * @tparam  To      Target type.
 * @tparam  Op      Test template.
 * @tparam  Args    Arguments to @p Op.
 */
template<class To, template<class...> class Op, class... Args>
static constexpr bool is_detected_nothrow_convertible_v =
    is_detected_nothrow_convertible<To, Op, Args...>::value;

} } // namespace fxx::meta

#endif

//--------------------------------------------------------------------------------------------------
// VERIFICATION USING STATIC ASSERTIONS
//--------------------------------------------------------------------------------------------------

#ifdef FXX_TEST_STATIC

#include <utility>
// std::declval

namespace fxx_meta_detect_h {

template<class T>
using it_member_t = decltype(std::declval<T>().it);

struct X {};
struct Y {
    Y(int) noexcept(false) {}
};

struct has_no_it {};
struct has_int_it { int it; };
struct has_char_it { char it; };
struct has_X_it { X x; };
struct has_Y_it { Y x; };

static_assert(
    !fxx::meta::is_detected_v<it_member_t, has_no_it>,
    "fxx::meta::is_detected_v: Not detected"
);
static_assert(
    fxx::meta::is_detected_v<it_member_t, has_int_it>,
    "fxx::meta::is_detected_v: Detected"
);

static_assert(
    !fxx::meta::is_detected_exact_v<int, it_member_t, has_no_it>,
    "fxx::meta::is_detected_exact_v: Not detected"
);
static_assert(
    !fxx::meta::is_detected_exact_v<int, it_member_t, has_char_it>,
    "fxx::meta::is_detected_exact_v: Wrong type detected"
);
static_assert(
    fxx::meta::is_detected_exact_v<int, it_member_t, has_int_it>,
    "fxx::meta::is_detected_exact_v: Exact type detected"
);

static_assert(
    !fxx::meta::is_detected_convertible_v<int, it_member_t, has_no_it>,
    "fxx::meta::is_detected_convertible_v: Not detected"
);
static_assert(
    !fxx::meta::is_detected_convertible_v<int, it_member_t, has_X_it>,
    "fxx::meta::is_detected_convertible_v: Non-convertible detected"
);
static_assert(
    fxx::meta::is_detected_convertible_v<int, it_member_t, has_char_it>,
    "fxx::meta::is_detected_convertible_v: Convertible detected"
);
static_assert(
    fxx::meta::is_detected_convertible_v<int, it_member_t, has_int_it>,
    "fxx::meta::is_detected_convertible_v: Exact type detected"
);

static_assert(
    fxx::meta::is_detected_nothrow_convertible_v<int, it_member_t, has_char_it>,
    "fxx::meta::is_detected_nothrow_convertible_v: No-throw convertible detected"
);
static_assert(
    !fxx::meta::is_detected_nothrow_convertible_v<int, it_member_t, has_Y_it>,
    "fxx::meta::is_detected_nothrow_convertible_v: Throwing conversion detected"
);

} // namespace fxx_meta_detect_h

#endif