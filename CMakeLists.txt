cmake_minimum_required(VERSION 3.21)

project(BacusExplorer
    VERSION 0.1.0
    DESCRIPTION "Bacus Explorer - Guafguaf Company"
    LANGUAGES C CXX
)

# --------------------------------------------------
# C++ STANDARD
# --------------------------------------------------

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

include(CTest)

# --------------------------------------------------
# PLATFORM DETECTION
# --------------------------------------------------

message(STATUS "Bacus Explorer ${PROJECT_VERSION}")
message(STATUS "Platform: ${CMAKE_SYSTEM_NAME}")
message(STATUS "Architecture: ${CMAKE_SYSTEM_PROCESSOR}")
message(STATUS "Compiler: ${CMAKE_CXX_COMPILER_ID}")

if(NOT CMAKE_SYSTEM_NAME MATCHES
    "^(Linux|Windows|Darwin|FreeBSD|OpenBSD|NetBSD)$"
)
    message(FATAL_ERROR
        "Unsupported platform: ${CMAKE_SYSTEM_NAME}"
    )
endif()

# --------------------------------------------------
# BROWSER ENGINE BACKEND
# --------------------------------------------------

if(CMAKE_SYSTEM_NAME STREQUAL "Linux")

    set(BACUS_BROWSER_BACKEND "WebKitGTK")

    find_package(PkgConfig REQUIRED)

    pkg_check_modules(GTK3 REQUIRED IMPORTED_TARGET
        gtk+-3.0
    )

    pkg_check_modules(WEBKITGTK REQUIRED IMPORTED_TARGET
        webkit2gtk-4.1
    )

else()

    message(FATAL_ERROR
        "Bacus Explorer currently implements the WebKitGTK backend on Linux. "
        "A native browser backend for ${CMAKE_SYSTEM_NAME} has not been implemented yet."
    )

endif()

# --------------------------------------------------
# COMMON INCLUDE DIRECTORIES
# --------------------------------------------------

set(BACUS_INCLUDE_DIRS
    "${CMAKE_CURRENT_SOURCE_DIR}"
)

# --------------------------------------------------
# TESTS
# --------------------------------------------------

if(BUILD_TESTING)

    add_executable(bacus-config-tests
        tests/config_tests.cpp
        app/command_line.cpp
    )

    target_include_directories(bacus-config-tests PRIVATE
        "${CMAKE_CURRENT_SOURCE_DIR}"
    )

    if(MSVC)
        target_compile_options(bacus-config-tests PRIVATE
            /W4
            /permissive-
        )
    else()
        target_compile_options(bacus-config-tests PRIVATE
            -Wall
            -Wextra
            -Wpedantic
        )
    endif()

    add_test(
        NAME BacusConfigTests
        COMMAND bacus-config-tests
    )

endif()

# --------------------------------------------------
# BACUS EXPLORER
# --------------------------------------------------

add_executable(bacus-explorer
    app/main.cpp
    app/command_line.cpp
    app/preferences.cpp
    app/browser_window.cpp
    app/internal_pages.cpp
)

target_include_directories(bacus-explorer PRIVATE
    ${BACUS_INCLUDE_DIRS}
)

target_compile_definitions(bacus-explorer PRIVATE
    BACUS_EXPLORER_VERSION="${PROJECT_VERSION}"
)

# --------------------------------------------------
# LINK LIBRARIES
# --------------------------------------------------

target_link_libraries(bacus-explorer PRIVATE
    PkgConfig::GTK3
    PkgConfig::WEBKITGTK
)

# --------------------------------------------------
# COMPILER WARNINGS
# --------------------------------------------------

if(MSVC)

    target_compile_options(bacus-explorer PRIVATE
        /W4
        /permissive-
    )

else()

    target_compile_options(bacus-explorer PRIVATE
        -Wall
        -Wextra
        -Wpedantic
    )

endif()

# --------------------------------------------------
# BUILD INFORMATION
# --------------------------------------------------

message(STATUS "Bacus Explorer configuration:")
message(STATUS "  Version: ${PROJECT_VERSION}")
message(STATUS "  Platform: ${CMAKE_SYSTEM_NAME}")
message(STATUS "  Architecture: ${CMAKE_SYSTEM_PROCESSOR}")
message(STATUS "  C++ Standard: ${CMAKE_CXX_STANDARD}")
message(STATUS "  Browser backend: ${BACUS_BROWSER_BACKEND}")
message(STATUS "  GTK3: enabled")
message(STATUS "  WebKitGTK: enabled")
message(STATUS "  Tests: ${BUILD_TESTING}")
