set (target_name JackCompiler)

list (APPEND srcs "CompilationEngine.cpp"
                  "CompilationTask.cpp"
                  "JackCompiler.cpp"
                  "JackTokenizer.cpp"
                  "JackUtil.cpp"
                  "SymbolTable.cpp"
                  "VmWriter.cpp"
                  "XmlWriter.cpp")

add_executable (${target_name} ${srcs})

target_include_directories (${target_name} SYSTEM PRIVATE ${Boost_INCLUDE_DIRS})

target_link_libraries (${target_name} Boost::program_options
                                      ${CMAKE_THREAD_LIBS_INIT})

if (("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU") OR ("${CMAKE_CXX_COMPILER_ID}" MATCHES "Clang"))
    target_link_libraries (${target_name} stdc++fs)
endif ()

install (TARGETS ${target_name} DESTINATION bin)
