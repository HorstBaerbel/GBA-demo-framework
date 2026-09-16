cmake_minimum_required(VERSION 3.21)

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
    gbadfw
)

add_executable(${PROJECT_NAME} ${TARGET_SOURCES}) # Create the elf file
add_gba_executables(${PROJECT_NAME} "TEST" "TEST" "WR" 1) # Generate the .gba from the elf. See: https://problemkaputt.de/gbatek-gba-cartridge-header.htm
target_include_directories(${PROJECT_NAME} PRIVATE ${TARGET_INCLUDE_DIRS})
target_link_directories(${PROJECT_NAME} PRIVATE ${TARGET_LIB_DIRS})
target_link_libraries(${PROJECT_NAME} LINK_PUBLIC ${TARGET_LIBS}) # Link the application
