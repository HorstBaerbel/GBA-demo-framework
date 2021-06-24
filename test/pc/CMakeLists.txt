cmake_minimum_required(VERSION 3.1.0)

project(tests_pc)

#-------------------------------------------------------------------------------
#set up compiler flags and executable names

if(MSVC)
    set(CMAKE_DEBUG_POSTFIX "d")
    add_definitions(-D_CRT_SECURE_NO_DEPRECATE)
    add_definitions(-D_CRT_NONSTDC_NO_DEPRECATE)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /MP") #multi-processor compilation
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} /MP") #multi-processor compilation
endif()

if(CMAKE_COMPILER_IS_GNUCXX)
    #check for G++ 5.3+
    execute_process(COMMAND ${CMAKE_CXX_COMPILER} -dumpversion OUTPUT_VARIABLE G++_VERSION)
    if (G++_VERSION VERSION_LESS 5.)
            message(SEND_ERROR "You need at least G++ 5.3 to compile tests!")
    endif()
    #set up compiler flags for GCC
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14") #support C++14
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -std=c11") #support C11
endif()

# Tell the code we're running on PC
add_definitions(-DTARGET_PC)

#-------------------------------------------------------------------------------
# Find devKitPro

# DevkitPro paths are broken on windows, so we have to fix those
macro(msys_to_cmake_path MsysPath ResultingPath)
    if(WIN32)
        string(REGEX REPLACE "^/([a-zA-Z])/" "\\1:/" ${ResultingPath} "${MsysPath}")
    else()
        set(${ResultingPath} "${MsysPath}")
    endif()
endmacro()

msys_to_cmake_path("$ENV{DEVKITPRO}" DEVKITPRO)
if(NOT IS_DIRECTORY ${DEVKITPRO})
    message(FATAL_ERROR "Please set DEVKITPRO in your environment")
endif()

msys_to_cmake_path("$ENV{DEVKITARM}" DEVKITARM)
if(NOT IS_DIRECTORY ${DEVKITARM})
    message(FATAL_ERROR "Please set DEVKITARM in your environment")
endif()

#-------------------------------------------------------------------------------
# List all the source files in out directory

LIST(APPEND TARGET_SOURCES
    main.cpp
    test_fp32.cpp
    test_vec.cpp
    ../../src/math/fp32.cpp
    ../../src/math/fp32_trig.cpp
    ../../src/debug/args.cpp
    ../../src/debug/output.cpp
    ../../src/debug/itoa.cpp
)

LIST(APPEND TARGET_INCLUDE_DIRS
    ../../src
    ${DEVKITPRO}/libgba/include
)

#-------------------------------------------------------------------------------
#define targets

#link_directories(${TARGET_LIBS})
include_directories(${TARGET_INCLUDE_DIRS})
add_executable(${PROJECT_NAME} ${TARGET_SOURCES})
#target_link_libraries(${PROJECT_NAME} stdfs)
