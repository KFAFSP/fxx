#include <catch2/catch.hpp>
#include <tuple/unsafe.h>

#include <tuple>
// std::(forward_as_tuple, get, make_tuple)
#include <utility>
// std::move

#include <fxx/tuple/flip.h>

using namespace std;
using namespace fxx::tuple;

TEST_CASE("fxx::tuple::flip", "[tuple]") {
    SECTION("Regular case") {
        SECTION("Values") {
            auto t = make_tuple(1, 2, 3);

            auto t_flip = flip(std::forward<decltype(t)>(t));

            static_assert(std::tuple_size_v<decltype(t_flip)> == 3);
            REQUIRE(get<0>(t_flip) == 3);
            REQUIRE(get<1>(t_flip) == 2);
            REQUIRE(get<2>(t_flip) == 1);
        }

        SECTION("References") {
            int a = 1, b = 2, c = 3;
            auto t = forward_as_tuple(move(a), move(b), move(c));

            auto t_flip = flip(std::forward<decltype(t)>(t));

            static_assert(std::tuple_size_v<decltype(t_flip)> == 3);
            REQUIRE(addressof(get<0>(std::forward<decltype(t_flip)>(t_flip))) == &c);
            REQUIRE(addressof(get<1>(std::forward<decltype(t_flip)>(t_flip))) == &b);
            REQUIRE(addressof(get<2>(std::forward<decltype(t_flip)>(t_flip))) == &a);
        }
    }
}