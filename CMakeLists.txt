cmake_minimum_required(VERSION 3.22)
project(utilities
    DESCRIPTION "Branes AI utilities. "
    VERSION "0.0.1"
    LANGUAGES C CXX ASM
    HOMEPAGE_URL "https://github.com/branes-ai/utilities")

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Default to Release build type if not specified
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release CACHE STRING "Choose the type of build." FORCE)
endif()

if(MSVC)
    # Correct __cplusplus variable setting
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /Zc:__cplusplus")
    # Ensure consistent runtime library usage (Release mode uses /MD, Debug uses /MDd)
    if(CMAKE_BUILD_TYPE MATCHES "Debug")
        add_compile_options(/MDd)
    else()
        add_compile_options(/MD)
    endif()
endif()

find_package(Boost)
if(Boost_FOUND)
    message(STATUS "Boost_INCLUDE_DIR: ${Boost_INCLUDE_DIR}")
    include_directories(${Boost_INCLUDE_DIR})
endif()

set(BUTILS_ROOT_DIR ${PROJECT_SOURCE_DIR})
set(BUTILS_INSTALL_INCLUDE_DIR "${PROJECT_SOURCE_DIR}/include")
set(BUTILS_INSTALL_LIB_DIR "${PROJECT_SOURCE_DIR}/lib")
set(BUTILS_INSTALL_BIN_DIR "${PROJECT_SOURCE_DIR}/bin")

add_definitions(-D BUTILS_ENABLE_TEST=ON)

# Include file for common includes 
include_directories(${BUTILS_INSTALL_INCLUDE_DIR})

# MLIR/IREE include directories
include_directories(C:/iree/iree/third_party/llvm-project/mlir/include)
include_directories(C:/iree/iree/build/include)

enable_testing()

option(BUTILS_CMAKE_TRACE "Trace cmake build file actions" ON)
option(BUTILS_USE_FOLDERS "Enable solution folders in Visual Studio, disable for Express" ON)
if(BUTILS_USE_FOLDERS)
    set_property(GLOBAL PROPERTY USE_FOLDERS ON)
endif()

####
# Macro to read all cpp files in a directory and create a test target for each
macro(compile_all testing prefix folder)
    foreach(source ${ARGN})
        get_filename_component(test ${source} NAME_WE)
        string(REPLACE " " ";" new_source ${source})
        set(test_name ${prefix}_${test})
        message(STATUS "Add test ${test_name} from source ${new_source}.")
        add_executable(${test_name} ${new_source})
        set_target_properties(${test_name} PROPERTIES FOLDER ${folder})
        if(${testing} STREQUAL "true")
            if(BUTILS_CMAKE_TRACE)
                message(STATUS "testing: ${test_name} ${RUNTIME_OUTPUT_DIRECTORY}/${test_name}")
            endif()
            add_test(${test_name} ${RUNTIME_OUTPUT_DIRECTORY}/${test_name})
        endif()
    endforeach()
endmacro()

####
# Macro to create an executable target from multiple files
macro(compile_multifile_target testing test_name folder)
    message(STATUS "Add test ${test_name} from source folder ${folder}.")
    add_executable(${test_name} ${ARGN})
    set_target_properties(${test_name} PROPERTIES FOLDER ${folder})
    if(${testing} STREQUAL "true")
        if(BUTILS_CMAKE_TRACE)
            message(STATUS "testing: ${test_name} ${RUNTIME_OUTPUT_DIRECTORY}/${test_name}")
        endif()
        add_test(${test_name} ${RUNTIME_OUTPUT_DIRECTORY}/${test_name})
    endif()
endmacro()

# Incorporate specific targets
add_subdirectory(src)
add_subdirectory(test)