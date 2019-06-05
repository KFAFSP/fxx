#include <catch2/catch.hpp>
#include <tuple/unsafe.h>

#include <tuple>
// std::(forward_as_tuple, get, make_tuple)
#include <utility>
// std::move

#include <fxx/tuple/skip.h>

using namespace std;
using namespace fxx::tuple;

TEST_CASE("fxx::tuple::skip", "[tuple]") {
    SECTION("Trivial case") {
        auto t = make_tuple(1, 2, 3);

        auto t_skip0 = skip<0>(std::forward<decltype(t)>(t));

        static_assert(std::tuple_size_v<decltype(t_skip0)> == 3);
        REQUIRE(get<0>(t_skip0) == 1);
        REQUIRE(get<1>(t_skip0) == 2);
        REQUIRE(get<2>(t_skip0) == 3);
    }

    SECTION("Regular case") {
        SECTION("Values") {
            auto t = make_tuple(1, 2, 3);

            auto t_skip1 = skip<1>(std::forward<decltype(t)>(t));

            static_assert(std::tuple_size_v<decltype(t_skip1)> == 2);
            REQUIRE(get<0>(t_skip1) == 2);
            REQUIRE(get<1>(t_skip1) == 3);
        }

        SECTION("References") {
            int a = 1, b = 2, c = 3;
            auto t = forward_as_tuple(move(a), move(b), move(c));

            auto t_skip1 = skip<1>(std::forward<decltype(t)>(t));

            static_assert(std::tuple_size_v<decltype(t_skip1)> == 2);
            REQUIRE(addressof(get<0>(std::forward<decltype(t_skip1)>(t_skip1))) == &b);
            REQUIRE(addressof(get<1>(std::forward<decltype(t_skip1)>(t_skip1))) == &c);
        }
    }
}