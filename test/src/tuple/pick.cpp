#include <catch2/catch.hpp>
#include <tuple/unsafe.h>

#include <tuple>
// std::(forward_as_tuple, get, make_tuple)
#include <utility>
// std::move

#include <fxx/tuple/pick.h>

using namespace std;
using namespace fxx::tuple;

TEST_CASE("fxx::tuple::pick", "[tuple]") {
    SECTION("Trivial case") {
        auto t = make_tuple(1, 2);

        auto t_pick0 = pick<>(std::forward<decltype(t)>(t));

        static_assert(std::tuple_size_v<decltype(t_pick0)> == 0);
    }

    SECTION("Regular case") {
        SECTION("Values") {
            auto t = make_tuple(1, 2);

            auto t_dup2 = pick<1, 1, 0>(std::forward<decltype(t)>(t));

            static_assert(std::tuple_size_v<decltype(t_dup2)> == 3);
            REQUIRE(get<0>(t_dup2) == 2);
            REQUIRE(get<1>(t_dup2) == 2);
            REQUIRE(get<2>(t_dup2) == 1);
        }

        SECTION("References") {
            int a = 1, b = 2;
            auto t = forward_as_tuple(move(a), move(b));

            auto t_dup2 = pick<1, 1, 0>(std::forward<decltype(t)>(t));

            static_assert(std::tuple_size_v<decltype(t_dup2)> == 3);
            REQUIRE(addressof(get<0>(std::forward<decltype(t_dup2)>(t_dup2))) == &b);
            REQUIRE(addressof(get<1>(std::forward<decltype(t_dup2)>(t_dup2))) == &b);
            REQUIRE(addressof(get<2>(std::forward<decltype(t_dup2)>(t_dup2))) == &a);
        }
    }
}