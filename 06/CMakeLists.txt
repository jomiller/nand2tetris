set (target_name Assembler)

add_executable (${target_name} Assembler.cpp
                               AssemblyEngine.cpp
                               Code.cpp
                               Parser.cpp
                               SymbolTable.cpp)

target_include_directories (${target_name} SYSTEM PRIVATE ${Boost_INCLUDE_DIRS})

target_link_libraries (${target_name} Boost::program_options)

install (TARGETS ${target_name} DESTINATION bin)
