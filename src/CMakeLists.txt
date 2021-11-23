cmake_minimum_required(VERSION 3.1.0)

project(gbafw)

file(GLOB_RECURSE TARGET_SOURCES *.cpp *.s)

LIST(APPEND TARGET_INCLUDE_DIRS
    ${CMAKE_CURRENT_SOURCE_DIR}
	${DEVKITPRO}/libgba/include
	${DEVKITARM}/arm-none-eabi/include
)

LIST(APPEND TARGET_LIBS
	${DEVKITPRO}/libgba/lib
)

link_directories(${TARGET_LIBS})
include_directories(${TARGET_INCLUDE_DIRS})
add_library(${PROJECT_NAME} STATIC ${TARGET_SOURCES})
target_link_libraries(${PROJECT_NAME} mm) # link maxmod

target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
