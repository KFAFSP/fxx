#include <catch2/catch.hpp>
#include <tuple/unsafe.h>

#include <functional>
// std::(bind, multiplies, placeholders::*, plus)
#include <tuple>
// std::(forward_as_tuple, get, make_tuple)
#include <utility>
// std::move

#include <fxx/tuple/map.h>

using namespace std;
using namespace fxx::tuple;

TEST_CASE("fxx::tuple::map", "[tuple]") {
    SECTION("Trivial case") {
        auto t = make_tuple();

        auto t_m = map(
            std::bind(std::plus{}, std::placeholders::_1, 1),
            std::forward<decltype(t)>(t)
        );

        static_assert(std::tuple_size_v<decltype(t_m)> == 0);
    }

    SECTION("Regular case") {
        SECTION("Values") {
            auto t = make_tuple(1, 2, 3);

            auto t_m = map(
                std::bind(std::multiplies{}, std::placeholders::_1, 2),
                std::forward<decltype(t)>(t)
            );

            static_assert(std::tuple_size_v<decltype(t_m)> == 3);
            REQUIRE(get<0>(t_m) == 2);
            REQUIRE(get<1>(t_m) == 4);
            REQUIRE(get<2>(t_m) == 6);
        }

        SECTION("References") {
            int a = 1, b = 2, c = 3;
            auto t = forward_as_tuple(move(a), move(b), move(c));

            auto t_m = map(
                [](auto&& x) -> decltype(x) { x *= 2; return forward<decltype(x)>(x); },
                std::forward<decltype(t)>(t)
            );

            static_assert(std::tuple_size_v<decltype(t_m)> == 3);
            REQUIRE(a == 2);
            REQUIRE(b == 4);
            REQUIRE(c == 6);
            REQUIRE(addressof(get<0>(std::forward<decltype(t_m)>(t_m))) == &a);
            REQUIRE(addressof(get<1>(std::forward<decltype(t_m)>(t_m))) == &b);
            REQUIRE(addressof(get<2>(std::forward<decltype(t_m)>(t_m))) == &c);
        }
    }
}