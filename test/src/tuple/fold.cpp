#include <catch2/catch.hpp>
#include <tuple/unsafe.h>

#include <functional>
// std::(bind, minus, multiplies, placeholders::*, plus)
#include <tuple>
// std::(forward_as_tuple, get, make_tuple)
#include <utility>
// std::move
#include <vector>
// std::vector

#include <fxx/tuple/fold.h>

using namespace std;
using namespace fxx::tuple;

TEST_CASE("fxx::tuple::fold", "[tuple]") {
    SECTION("Trivial case") {
        auto t = make_tuple();

        auto r = fold(
            std::plus{},
            1,
            std::forward<decltype(t)>(t)
        );

        REQUIRE(r == 1);
    }

    SECTION("Regular case") {
        SECTION("Values") {
            auto t = make_tuple(1, 2, 3);

            auto r = fold(
                std::minus{},
                6,
                std::forward<decltype(t)>(t)
            );

            REQUIRE(r == 0);
        }

        SECTION("References") {
            vector<int> v;
            v.reserve(3);
            auto v_b = v.data();

            int a = 1, b = 2, c = 3;
            auto t = forward_as_tuple(move(a), move(b), move(c));

            auto r = fold(
                [](std::vector<int>&& x, auto&& y) {
                    x.push_back(y); y = 0; return std::move(x);
                },
                std::move(v),
                std::forward<decltype(t)>(t)
            );

            static_assert(std::is_same_v<std::vector<int>, decltype(r)>);
            REQUIRE(r.size() == 3);
            REQUIRE(v_b == r.data());
            REQUIRE(a == 0);
            REQUIRE(b == 0);
            REQUIRE(c == 0);
        }
    }
}