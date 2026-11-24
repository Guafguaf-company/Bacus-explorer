
cmake_minimum_required(VERSION 3.20)

project(BacusExplorer
    VERSION 0.1.0
    DESCRIPTION "Bacus Explorer - Guafguaf Company"
    LANGUAGES CXX
)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

include(CTest)

if(BUILD_TESTING)
    add_executable(bacus-config-tests
        tests/config_tests.cpp
    )

    target_include_directories(bacus-config-tests PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}
    )

    target_compile_options(bacus-config-tests PRIVATE
        -Wall
        -Wextra
        -Wpedantic
    )

    add_test(
        NAME BacusConfigTests
        COMMAND bacus-config-tests
    )
endif()

find_package(PkgConfig REQUIRED)
pkg_check_modules(GTK3 REQUIRED IMPORTED_TARGET gtk+-3.0)

add_executable(bacus-explorer
    app/main.cpp
    app/browser_window.cpp
    app/internal_pages.cpp
)

target_include_directories(bacus-explorer PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
)

target_link_libraries(bacus-explorer PRIVATE
    PkgConfig::GTK3
)

target_compile_options(bacus-explorer PRIVATE
    -Wall
    -Wextra
    -Wpedantic
)
