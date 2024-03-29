find_package(Catch2 REQUIRED)

# Add the test executable target.
add_executable(fxx-test
    src/cxx.cpp
    src/main.cpp
    src/meta.cpp

    src/tuple/dup.cpp
    src/tuple/find.cpp
    src/tuple/first.cpp
    src/tuple/flip.cpp
    src/tuple/fold.cpp
    src/tuple/map.cpp
    src/tuple/pick.cpp
    src/tuple/reduce.cpp
    src/tuple/skip.cpp
    src/tuple/slice.cpp
    src/tuple/take.cpp
)
target_link_libraries(fxx-test
    PRIVATE
        Catch2::Catch2
        fxx
)
target_include_directories(fxx-test
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/src
)

# Add all the Catch2 test cases to CTest.
include(CTest)
include(Catch)
catch_discover_tests(fxx-test)