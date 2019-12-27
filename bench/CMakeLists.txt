#
# Copyright (c) 2019 Vinnie Falco (vinnie.falco@gmail.com)
#
# Distributed under the Boost Software License, Version 1.0. (See accompanying
# file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
#
# Official repository: https://github.com/vinniefalco/json
#

source_group(json FILES ${PROJECT_SOURCE_DIR}/include/boost/json.hpp)
source_group(json FILES ${PROJECT_SOURCE_DIR}/include/boost/pilfer.hpp)
source_group(TREE ${PROJECT_SOURCE_DIR}/include/boost/json PREFIX json FILES ${PROJECT_FILES})

GroupSources(bench "/")

# Disable auto-linking.
add_definitions(-DBOOST_ALL_NO_LIB=1)

include_directories(../test)

if(MSVC)
    foreach(config MINSIZEREL RELWITHDEBINFO)
        set(CMAKE_CXX_FLAGS_${config} "${CMAKE_CXX_FLAGS_${config}} /Ob2 /GL")
        set(CMAKE_EXE_LINKER_FLAGS_${config} "${CMAKE_EXE_LINKER_FLAGS_${config}} /LTCG")
    endforeach()
endif()

add_executable(bench
    ${PROJECT_FILES}
    ${PROJECT_SOURCE_DIR}/include/boost/json.hpp
    ${PROJECT_SOURCE_DIR}/include/boost/pilfer.hpp
    ${PROJECT_SOURCE_DIR}/src/src.cpp
    Jamfile
    bench.cpp
)

target_link_libraries(bench PRIVATE Threads::Threads)

add_test(json-tests json-tests)
