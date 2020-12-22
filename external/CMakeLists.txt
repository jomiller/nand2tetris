set (CXXOPTS_BUILD_EXAMPLES FALSE CACHE BOOL "Set to ON to build examples")
set (CXXOPTS_BUILD_TESTS FALSE CACHE BOOL "Set to ON to build tests")
set (CXXOPTS_ENABLE_INSTALL FALSE CACHE BOOL "Generate the install target")

add_subdirectory (cxxopts)
add_subdirectory (fmt)
add_subdirectory (frozen)
