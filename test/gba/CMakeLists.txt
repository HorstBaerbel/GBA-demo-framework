cmake_minimum_required(VERSION 3.1.0)

project(tests_gba)

# List all the source files in out directory
LIST(APPEND TARGET_SOURCES
    main.cpp
    test_copy.cpp
    test_fp32.cpp
    test_memory.cpp
)

LIST(APPEND TARGET_INCLUDE_DIRS
	${DEVKITPRO}/libgba/include
	${DEVKITARM}/arm-none-eabi/include
)

LIST(APPEND TARGET_LIB_DIRS
	${DEVKITPRO}/libgba/lib
)

LIST(APPEND TARGET_LIBS
    gbafw
)

link_directories(${TARGET_LIB_DIRS})
include_directories(${TARGET_INCLUDE_DIRS})
add_executable(${PROJECT_NAME}.elf ${TARGET_SOURCES}) # Create the elf file
add_gba_executable(${PROJECT_NAME}.elf) # Generate the .gba from the elf
target_link_libraries(${PROJECT_NAME}.elf LINK_PUBLIC ${TARGET_LIBS}) # Link the application
