#include <catch2/catch.hpp>
#include <tuple/unsafe.h>

#include <functional>
// std::(bind, placeholders::*)
#include <tuple>
// std::(forward_as_tuple, get, make_tuple)
#include <utility>
// std::move

#include <fxx/tuple/find.h>

using namespace std;
using namespace fxx::tuple;

TEST_CASE("fxx::tuple::find", "[tuple]") {
    SECTION("Trivial case") {
        auto t = make_tuple();

        auto idx = find(
            1,
            std::forward<decltype(t)>(t)
        );

        static_assert(std::is_same_v<decltype(idx), nullopt_t>);
    }

    SECTION("Regular case") {
        auto t = make_tuple(1, 2, 3);

        auto idx = find<int>(
            3,
            std::forward<decltype(t)>(t)
        );

        static_assert(std::is_same_v<decltype(idx), std::optional<std::size_t>>);
        REQUIRE(idx);
        REQUIRE(idx.value() == 2);
    }
}