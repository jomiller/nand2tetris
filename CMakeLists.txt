cmake_minimum_required (VERSION "3.10.3")

project (Nand2Tetris LANGUAGES CXX)

find_package (Threads REQUIRED)

find_package (Boost REQUIRED COMPONENTS program_options)

set (CMAKE_CXX_STANDARD 17)
set (CMAKE_CXX_STANDARD_REQUIRED TRUE)
set (CMAKE_CXX_EXTENSIONS FALSE)

set (CMAKE_EXPORT_COMPILE_COMMANDS TRUE)

if (CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT)
    set (CMAKE_INSTALL_PREFIX "${CMAKE_BINARY_DIR}")
endif ()

option (CPPCHECK "Enable Cppcheck static analysis tool" FALSE)

if (${CPPCHECK})
    find_program (CMAKE_CXX_CPPCHECK "cppcheck")

    if (CMAKE_CXX_CPPCHECK)
        list (APPEND CMAKE_CXX_CPPCHECK #"--project=${CMAKE_BINARY_DIR}/compile_commands.json"
                                        "--enable=warning,style,performance,portability"
                                        "--suppress=passedByValue"
                                        #"--std=c++17"
                                        "--std=c++14"
                                        "--quiet")
    else ()
        message (FATAL_ERROR "Could not find program: cppcheck")
    endif ()
endif ()

if (("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU") OR ("${CMAKE_CXX_COMPILER_ID}" MATCHES "Clang"))

    option (CLANG_ANALYZER "Enable Clang Static Analyzer" FALSE)

    if (${CLANG_ANALYZER})
        find_program (cxx_analyzer "c++-analyzer")

        if (cxx_analyzer)
            set (ENV{CCC_CXX} "${CMAKE_CXX_COMPILER}")
            set (CMAKE_CXX_COMPILER "${cxx_analyzer}")
        else ()
            message (FATAL_ERROR "Could not find program: c++-analyzer")
        endif ()
    endif ()

    option (CLANG_TIDY "Enable clang-tidy linter tool" FALSE)

    if (${CLANG_TIDY})
        find_program (CMAKE_CXX_CLANG_TIDY "clang-tidy")

        if (CMAKE_CXX_CLANG_TIDY)
            list (APPEND CMAKE_CXX_CLANG_TIDY "-p=${CMAKE_BINARY_DIR}")
        else ()
            message (FATAL_ERROR "Could not find program: clang-tidy")
        endif ()
    endif ()

    set (sanitizers "address" "thread" "undefined")

    if ("${CMAKE_CXX_COMPILER_ID}" MATCHES "Clang")
        list (APPEND sanitizers "memory" "integer" "nullability")
    endif ()

    set (SANITIZER FALSE CACHE STRING "Enable Clang/GCC sanitizer")
    set_property (CACHE SANITIZER PROPERTY STRINGS ${sanitizers} FALSE)

    if (${SANITIZER})
        if ("${SANITIZER}" IN_LIST sanitizers)
            set (sanitizer_option "-fsanitize=${SANITIZER}")
            add_compile_options (${sanitizer_option} -fno-omit-frame-pointer)
            set (CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} ${sanitizer_option}")
        else ()
            message (FATAL_ERROR "Unsupported sanitizer type: ${SANITIZER}")
        endif ()
    endif ()

    if ("${CMAKE_BUILD_TYPE}" STREQUAL "RelWithDebInfo")
        add_compile_options (-fno-omit-frame-pointer)
    endif ()

    add_compile_options (-Wall -Wextra -Werror)

elseif ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "MSVC")

    add_compile_options (/Wall /WX)

endif ()

add_subdirectory (06)
add_subdirectory (07-08)
add_subdirectory (10-11)
