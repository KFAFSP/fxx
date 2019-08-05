#include <catch2/catch.hpp>
#include <tuple/unsafe.h>

#include <functional>
// std::(bind, placeholders::*)
#include <tuple>
// std::(forward_as_tuple, get, make_tuple)
#include <utility>
// std::move

#include <fxx/tuple/first.h>

using namespace std;
using namespace fxx::tuple;

TEST_CASE("fxx::tuple::first", "[tuple]") {
    SECTION("Trivial case") {
        auto t = make_tuple();

        auto idx = first(
            [](auto&&) { return true; },
            std::forward<decltype(t)>(t)
        );

        static_assert(std::is_same_v<decltype(idx), nullopt_t>);
    }

    SECTION("Regular case") {
        SECTION("Values") {
            auto t = make_tuple(1, 2, 3);

            auto idx = first(
                [](auto&& x) { return x > 1; },
                std::forward<decltype(t)>(t)
            );

            static_assert(std::is_same_v<decltype(idx), std::optional<std::size_t>>);
            REQUIRE(idx);
            REQUIRE(idx.value() == 1);
        }

        SECTION("References") {
            int a = 1, b = 2, c = 3;
            auto t = forward_as_tuple(move(a), move(b), move(c));

            auto idx = first(
                [](auto&& x) { return !std::is_same_v<int&&, decltype(x)>; },
                std::forward<decltype(t)>(t)
            );

            static_assert(std::is_same_v<decltype(idx), std::optional<std::size_t>>);
            REQUIRE(!idx);
        }
    }
}