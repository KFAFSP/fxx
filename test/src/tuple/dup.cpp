#include <catch2/catch.hpp>
#include <tuple/unsafe.h>

#include <tuple>
// std::(forward_as_tuple, get, make_tuple)
#include <utility>
// std::move

#include <fxx/tuple/dup.h>

using namespace std;
using namespace fxx::tuple;

TEST_CASE("fxx::tuple::dup", "[tuple]") {
    SECTION("Trivial case") {
        auto t = make_tuple(1, 2);

        auto t_dup0 = dup<0>(std::forward<decltype(t)>(t));

        static_assert(std::tuple_size_v<decltype(t_dup0)> == 0);
    }

    SECTION("Regular case") {
        SECTION("Values") {
            auto t = make_tuple(1, 2);

            auto t_dup2 = dup<2>(std::forward<decltype(t)>(t));

            static_assert(std::tuple_size_v<decltype(t_dup2)> == 4);
            REQUIRE(get<0>(t_dup2) == 1);
            REQUIRE(get<1>(t_dup2) == 2);
            REQUIRE(get<2>(t_dup2) == 1);
            REQUIRE(get<3>(t_dup2) == 2);
        }

        SECTION("References") {
            int a = 1, b = 2;
            auto t = forward_as_tuple(move(a), move(b));

            auto t_dup2 = dup<2>(std::forward<decltype(t)>(t));

            static_assert(std::tuple_size_v<decltype(t_dup2)> == 4);
            REQUIRE(addressof(get<0>(std::forward<decltype(t_dup2)>(t_dup2))) == &a);
            REQUIRE(addressof(get<1>(std::forward<decltype(t_dup2)>(t_dup2))) == &b);
            REQUIRE(addressof(get<2>(std::forward<decltype(t_dup2)>(t_dup2))) == &a);
            REQUIRE(addressof(get<3>(std::forward<decltype(t_dup2)>(t_dup2))) == &b);
        }
    }
}