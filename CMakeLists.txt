#
# This file is part of Nand2Tetris.
#
# Copyright © 2013-2020 Jonathan Miller
#
# Permission is hereby granted, free of charge, to any person obtaining a copy
# of this software and associated documentation files (the "Software"), to deal
# in the Software without restriction, including without limitation the rights
# to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
# copies of the Software, and to permit persons to whom the Software is
# furnished to do so, subject to the following conditions:
#
# The above copyright notice and this permission notice shall be included in all
# copies or substantial portions of the Software.
#
# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
# IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
# FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
# AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
# LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
# OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
# SOFTWARE.
#

cmake_minimum_required (VERSION "3.10.2")

project (Nand2Tetris LANGUAGES CXX)

find_package (Threads REQUIRED)

find_package (Boost REQUIRED COMPONENTS program_options)

set (CMAKE_CXX_EXTENSIONS FALSE)

set (CMAKE_EXPORT_COMPILE_COMMANDS TRUE CACHE BOOL "Enable/Disable output of compile commands during generation." FORCE)

if (CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT)
    set (CMAKE_INSTALL_PREFIX "${CMAKE_BINARY_DIR}" CACHE PATH "Install path prefix, prepended onto install directories." FORCE)
endif ()

option (CPPCHECK "Enable Cppcheck static analysis tool" FALSE)

if (${CPPCHECK})
    find_program (CMAKE_CXX_CPPCHECK "cppcheck")

    if (CMAKE_CXX_CPPCHECK)
        list (APPEND CMAKE_CXX_CPPCHECK #"--project=${CMAKE_BINARY_DIR}/compile_commands.json"
                                        "--enable=warning,style,performance,portability"
                                        "--suppress=passedByValue"
                                        "--std=c++17"
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

    add_compile_options (-Wall -Wextra -Wpedantic -Werror)

elseif ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "MSVC")

    add_compile_options (/W4 /WX)

endif ()

add_subdirectory (06)
add_subdirectory (07-08)
add_subdirectory (10-11)
