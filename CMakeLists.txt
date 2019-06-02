cmake_minimum_required(VERSION 3.9)

list(INSERT CMAKE_MODULE_PATH 0 ${CMAKE_SOURCE_DIR}/cmake)

find_package(Doxygen)
if (DOXYGEN_FOUND)
    set(DOXYGEN_IN ${CMAKE_CURRENT_SOURCE_DIR}/doc/Doxyfile.in)
    set(DOXYGEN_OUT ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile)
    configure_file(${DOXYGEN_IN} ${DOXYGEN_OUT} @ONLY)

    add_custom_target(doc
        COMMAND ${DOXYGEN_EXECUTABLE} ${DOXYGEN_OUT}
        WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
        COMMENT "Generating API documentation with Doxygen"
        VERBATIM
    )

    message(STATUS "Doxygen found. Build taret 'doc' to build the API documentation.")
else (DOXYGEN_FOUND)
    message(WARNING "Doxygen not found. Documentation cannot be generated.")
endif (DOXYGEN_FOUND)

#######
# FXX #
#######

project(fxx
    VERSION
        0.2.0
    LANGUAGES
        CXX
)

# Add main library project.
add_library(fxx
    src/fxx.cpp
)
add_library(fxx::fx ALIAS fxx)
target_include_directories(fxx
    PUBLIC
        $<INSTALL_INTERFACE:include>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/src
)
target_compile_features(fxx
    PUBLIC
        cxx_std_17
)

# Add test project.
enable_testing()
add_subdirectory(test)