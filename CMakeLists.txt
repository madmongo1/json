#
# Copyright (c) 2019 Vinnie Falco (vinnie.falco@gmail.com)
#
# Distributed under the Boost Software License, Version 1.0. (See accompanying
# file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
#
# Official repository: https://github.com/vinniefalco/json
#

cmake_minimum_required(VERSION 3.10.0)
cmake_policy(VERSION 3.10.0)

project(JSON VERSION 1 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 11 CACHE STRING "")
set(CMAKE_CXX_STANDARD_REQUIRED ON CACHE STRING "")
set(CMAKE_CXX_EXTENSIONS OFF CACHE STRING "")

#-------------------------------------------------------------------------------

set_property(GLOBAL PROPERTY USE_FOLDERS ON)

function(DoGroupSources curdir rootdir folder)
    file(GLOB children RELATIVE ${PROJECT_SOURCE_DIR}/${curdir} ${PROJECT_SOURCE_DIR}/${curdir}/*)
    foreach(child ${children})
        if(IS_DIRECTORY ${PROJECT_SOURCE_DIR}/${curdir}/${child})
            DoGroupSources(${curdir}/${child} ${rootdir} ${folder})
        elseif(${child} STREQUAL "CMakeLists.txt")
            source_group("" FILES ${PROJECT_SOURCE_DIR}/${curdir}/${child})
        else()
            string(REGEX REPLACE ^${rootdir} ${folder} groupname ${curdir})
            string(REPLACE "/" "\\" groupname ${groupname})
            source_group(${groupname} FILES ${PROJECT_SOURCE_DIR}/${curdir}/${child})
        endif()
    endforeach()
endfunction()

function(GroupSources curdir folder)
    DoGroupSources(${curdir} ${curdir} ${folder})
endfunction()

#-------------------------------------------------------------------------------
#
# JSON
#
#-------------------------------------------------------------------------------

if(MSVC)
    # Hide compiler logos.
    set(CMAKE_CL_NOLOGO "/nologo" CACHE STRING "")

    add_definitions(
        /D_WIN32_WINNT=0x0601
        /D_CRT_SECURE_NO_WARNINGS
    )

    add_compile_options(
        /permissive-  # strict C++
        /MP           # multi-processor compilation
    )

    set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /Ob2 /Oi /Ot /GL")
    set(CMAKE_CXX_FLAGS_RELWITHDEBINFO "${CMAKE_CXX_FLAGS_RELWITHDEBINFO} /Oi /Ot")

    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} /SAFESEH:NO")
    set(CMAKE_EXE_LINKER_FLAGS_RELEASE "${CMAKE_EXE_LINKER_FLAGS_RELEASE} /LTCG")

    # Enable all warnings.
    string(REPLACE "/W3" "/W4" CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS})

    # Use static run-time library.
    foreach(config DEBUG MINSIZEREL RELEASE RELWITHDEBINFO)
        string(REPLACE "/MD" "/MT"
            CMAKE_CXX_FLAGS_${config} ${CMAKE_CXX_FLAGS_${config}})
    endforeach()

    # Disable incremental linking for debug configurations.
    foreach(config DEBUG RELWITHDEBINFO)
        string(REGEX REPLACE "/INCREMENTAL(:YES)?" "/INCREMENTAL:NO"
            CMAKE_EXE_LINKER_FLAGS_${config} ${CMAKE_EXE_LINKER_FLAGS_${config}})
    endforeach()
else()
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -Wpedantic -Wno-unused-parameter")

    if("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wrange-loop-analysis")
    endif()
endif()

set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads)

# Must come before Boost includes, otherwise the
# IDE sees the wrong file due to boost/ symlinks.
include_directories(include)

#-------------------------------------------------------------------------------
#
# Boost
#
#-------------------------------------------------------------------------------

get_filename_component(BOOST_ROOT ../.. ABSOLUTE)

include_directories(${BOOST_ROOT})
link_directories(${BOOST_ROOT}/stage/lib)

# Enable auto-linking for static boost libraries.
add_definitions(-DBOOST_ALL_STATIC_LINK=1)

# Disable auto-linking for this library.
add_definitions(-DBOOST_JSON_NO_LIB=1)


#-------------------------------------------------------------------------------

if("${VARIANT}" STREQUAL "coverage")
    if(NOT MSVC)
        set(CMAKE_BUILD_TYPE DEBUG)
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -msse4.2 --coverage")
        set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} --coverage")
    endif()
elseif("${VARIANT}" STREQUAL "ubasan")
    if(NOT MSVC)
        set(CMAKE_BUILD_TYPE RELWITHDEBINFO)
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -msse4.2")
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -funsigned-char")
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fno-omit-frame-pointer")
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fsanitize=address,undefined")
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fno-sanitize-recover=address,undefined")
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fsanitize-blacklist=${PROJECT_SOURCE_DIR}/tools/blacklist.supp")
        set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -fsanitize=address,undefined")
        set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -fno-sanitize-recover=address,undefined")
    endif()
elseif("${VARIANT}" STREQUAL "debug")
    set(CMAKE_BUILD_TYPE DEBUG)
elseif("${VARIANT}" STREQUAL "release")
    set(CMAKE_BUILD_TYPE RELEASE)
endif()

#-------------------------------------------------------------------------------
#
# Tests and examples
#
#-------------------------------------------------------------------------------

file(GLOB_RECURSE PROJECT_FILES
    ${PROJECT_SOURCE_DIR}/include/boost/json/*.hpp
    ${PROJECT_SOURCE_DIR}/include/boost/json/*.ipp
    ${PROJECT_SOURCE_DIR}/include/boost/json/*.natvis
)

add_subdirectory(bench)
add_subdirectory(example)
add_subdirectory(test)


# Hide Visual Studio CMake warnings.
set(HIDE_VS_DEFINITIONS ${CMAKE_C_COMPILER})
set(HIDE_VS_DEFINITIONS)
