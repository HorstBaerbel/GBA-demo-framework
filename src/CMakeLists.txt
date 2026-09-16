cmake_minimum_required(VERSION 3.21)

project(gbadfw)

file(GLOB_RECURSE FW_SOURCES *.c *.cpp *.s)

LIST(APPEND FW_INCLUDE_DIRS
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/data
	${DEVKITPRO}/libgba/include
	${DEVKITARM}/arm-none-eabi/include
)

LIST(APPEND FW_LIBS
	${DEVKITPRO}/libgba/lib
)

add_library(${PROJECT_NAME} STATIC ${FW_SOURCES})
target_include_directories(${PROJECT_NAME} PRIVATE ${FW_INCLUDE_DIRS})
target_link_directories(${PROJECT_NAME} PRIVATE ${FW_LIBS})
target_link_libraries(${PROJECT_NAME} mm) # Link MaxMOD

target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
