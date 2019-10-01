set (target_name VmTranslator)

add_executable (${target_name} CodeWriter.cpp
                               Parser.cpp
                               TranslationEngine.cpp
                               VmTranslator.cpp
                               VmUtil.cpp)

target_include_directories (${target_name} SYSTEM PRIVATE ${Boost_INCLUDE_DIRS})

target_link_libraries (${target_name} Boost::program_options)

install (TARGETS ${target_name} DESTINATION bin)
