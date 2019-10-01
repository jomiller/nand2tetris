set (target_name JackCompiler)

add_executable (${target_name} CompilationEngine.cpp
                               CompilationTask.cpp
                               JackCompiler.cpp
                               JackTokenizer.cpp
                               JackUtil.cpp
                               SymbolTable.cpp
                               VmWriter.cpp
                               XmlWriter.cpp)

target_include_directories (${target_name} SYSTEM PRIVATE ${Boost_INCLUDE_DIRS})

target_link_libraries (${target_name} Boost::program_options
                                      ${CMAKE_THREAD_LIBS_INIT})

install (TARGETS ${target_name} DESTINATION bin)
