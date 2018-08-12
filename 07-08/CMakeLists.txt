set (target_name VmTranslator)

list (APPEND srcs "CodeWriter.cpp"
                  "Parser.cpp"
                  "TranslationEngine.cpp"
                  "VmTranslator.cpp"
                  "VmUtil.cpp")

add_executable (${target_name} ${srcs})

target_include_directories (${target_name} SYSTEM PRIVATE ${Boost_INCLUDE_DIRS})

target_link_libraries (${target_name} Boost::program_options)

if (("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU") OR ("${CMAKE_CXX_COMPILER_ID}" MATCHES "Clang"))
    target_link_libraries (${target_name} stdc++fs)
endif ()

install (TARGETS ${target_name} DESTINATION bin)
