#include <catch2/catch.hpp>
#include <tuple/unsafe.h>

#include <tuple>
// std::(forward_as_tuple, get, make_tuple)
#include <utility>
// std::move

#include <fxx/tuple/slice.h>

using namespace std;
using namespace fxx::tuple;

TEST_CASE("fxx::tuple::slice", "[tuple]") {
    SECTION("Trivial case") {
        auto t = make_tuple(1, 2, 3);

        auto t_slice_1_0 = slice<1, 0>(std::forward<decltype(t)>(t));

        static_assert(std::tuple_size_v<decltype(t_slice_1_0)> == 0);
    }

    SECTION("Regular case") {
        SECTION("Values") {
            auto t = make_tuple(1, 2, 3);

            auto t_slice_1_1 = slice<1, 1>(std::forward<decltype(t)>(t));

            static_assert(std::tuple_size_v<decltype(t_slice_1_1)> == 1);
            REQUIRE(get<0>(t_slice_1_1) == 2);
        }

        SECTION("References") {
            int a = 1, b = 2, c = 3;
            auto t = forward_as_tuple(move(a), move(b), move(c));

            auto t_slice_1_1 = slice<1, 1>(std::forward<decltype(t)>(t));

            static_assert(std::tuple_size_v<decltype(t_slice_1_1)> == 1);
            REQUIRE(addressof(get<0>(std::forward<decltype(t_slice_1_1)>(t_slice_1_1))) == &b);
        }
    }
}