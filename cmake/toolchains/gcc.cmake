# Include common options.
include(${CMAKE_CURRENT_LIST_DIR}/common.cmake)

# Static runtime linkage.
add_compile_options(-static)

# Compiler options.
add_compile_options(-Wall -Wextra -Wpedantic -Wno-unused-parameter)

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
