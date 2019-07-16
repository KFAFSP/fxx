#include <catch2/catch.hpp>
#include <tuple/unsafe.h>

#include <tuple>
// std::(forward_as_tuple, get, make_tuple)
#include <utility>
// std::move

#include <fxx/tuple/take.h>

using namespace std;
using namespace fxx::tuple;

TEST_CASE("fxx::tuple::take", "[tuple]") {
    SECTION("Trivial case") {
        auto t = make_tuple(1, 2, 3);

        auto t_take0 = take<0>(std::forward<decltype(t)>(t));

        static_assert(std::tuple_size_v<decltype(t_take0)> == 0);
    }

    SECTION("Regular case") {
        SECTION("Values") {
            auto t = make_tuple(1, 2, 3);

            auto t_take2 = take<2>(std::forward<decltype(t)>(t));

            static_assert(std::tuple_size_v<decltype(t_take2)> == 2);
            REQUIRE(get<0>(t_take2) == 1);
            REQUIRE(get<1>(t_take2) == 2);
        }

        SECTION("References") {
            int a = 1, b = 2, c = 3;
            auto t = forward_as_tuple(move(a), move(b), move(c));

            auto t_take2 = take<2>(std::forward<decltype(t)>(t));

            static_assert(std::tuple_size_v<decltype(t_take2)> == 2);
            REQUIRE(addressof(get<0>(std::forward<decltype(t_take2)>(t_take2))) == &a);
            REQUIRE(addressof(get<1>(std::forward<decltype(t_take2)>(t_take2))) == &b);
        }
    }
}