#ifndef FXX_TEST_TUPLE_UNSAFE_H
#define FXX_TEST_TUPLE_UNSAFE_H
#pragma once

#include <memory>
// std::addressof

template<class T>
constexpr auto addressof(T&& t) {
    return std::addressof(t);
}

#endif