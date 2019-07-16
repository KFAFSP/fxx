#include <catch2/catch.hpp>
#include <tuple/unsafe.h>

#include <functional>
// std::(bind, minus, multiplies, placeholders::*, plus)
#include <tuple>
// std::(forward_as_tuple, get, make_tuple)
#include <utility>
// std::move

#include <fxx/tuple/reduce.h>

using namespace std;
using namespace fxx::tuple;

TEST_CASE("fxx::tuple::reduce", "[tuple]") {
    SECTION("Trivial case") {
        auto t = make_tuple(1);

        auto r = reduce(
            std::plus{},
            std::forward<decltype(t)>(t)
        );

        REQUIRE(r == 1);
    }

    SECTION("Regular case") {
        SECTION("Values") {
            auto t = make_tuple(1, 2, 3);

            auto r = reduce(
                std::minus{},
                std::forward<decltype(t)>(t)
            );

            REQUIRE(r == -4);
        }

        SECTION("References") {
            int a = 1, b = 2, c = 3;
            auto t = forward_as_tuple(move(a), move(b), move(c));

            auto&& r = reduce(
                [](auto&& x, auto&& y) -> decltype(x) { x += y; return forward<decltype(x)>(x); },
                std::forward<decltype(t)>(t)
            );

            REQUIRE(a == 6);
            REQUIRE(b == 2);
            REQUIRE(c == 3);
            REQUIRE(addressof(r) == &a);
        }
    }
}