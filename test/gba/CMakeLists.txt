cmake_minimum_required(VERSION 3.1.0)

project(tests_gba)

# List all the source files in out directory
LIST(APPEND TARGET_SOURCES
    main.cpp
    test_copy.cpp
    test_fp32.cpp
    test_memory.cpp
    ${CMAKE_SOURCE_DIR}/src/math/fp32.cpp
    ${CMAKE_SOURCE_DIR}/src/math/random.cpp
    ${CMAKE_SOURCE_DIR}/src/math/random_u32.s
    ${CMAKE_SOURCE_DIR}/src/memory/dma.cpp
    ${CMAKE_SOURCE_DIR}/src/memory/memory.cpp
    ${CMAKE_SOURCE_DIR}/src/memory/memcpy.s
    ${CMAKE_SOURCE_DIR}/src/memory/memset.s
    ${CMAKE_SOURCE_DIR}/src/debug/args.cpp
    ${CMAKE_SOURCE_DIR}/src/debug/output.cpp
    ${CMAKE_SOURCE_DIR}/src/debug/itoa.cpp
    ${CMAKE_SOURCE_DIR}/src/sys/interrupts.cpp
    ${CMAKE_SOURCE_DIR}/src/sys/intdispatcher.s
    ${CMAKE_SOURCE_DIR}/src/video.cpp
    ${CMAKE_SOURCE_DIR}/src/fptime.cpp
)

LIST(APPEND TARGET_INCLUDE_DIRS
    ${CMAKE_SOURCE_DIR}/src
	${DEVKITPRO}/libgba/include
	${DEVKITARM}/arm-none-eabi/include
)

LIST(APPEND TARGET_LIBS
	${DEVKITPRO}/libgba/lib
)

link_directories(${TARGET_LIBS})
include_directories(${TARGET_INCLUDE_DIRS})
add_executable(${PROJECT_NAME}.elf ${TARGET_SOURCES}) # Create the elf file
add_gba_executable(${PROJECT_NAME}.elf) # Generate the .gba from the elf
target_link_libraries(${PROJECT_NAME}.elf) # Link the application
