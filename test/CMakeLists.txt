find_package(Catch2 REQUIRED)

# Add the test executable target.
add_executable(fxx-test
    src/main.cpp
    src/meta.cpp

    src/tuple/dup.cpp
    src/tuple/flip.cpp
    src/tuple/pick.cpp
    src/tuple/skip.cpp
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