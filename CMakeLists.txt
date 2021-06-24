cmake_minimum_required(VERSION 3.1.0)

project(framework)

# Add the 3ds-cmake folder to the modules paths, so that we can use the GBA tools
list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_LIST_DIR}/3ds-cmake/cmake)
# Add the cmake folder to the modules paths, so that we can use the compiler flags
list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_LIST_DIR}/cmake)

if (GBA)
	message("Building for GBA")
	# ASM must be enabled to support .S files
	enable_language(ASM)
	# Include all the macros and tools needed for GBA development
	include(ToolsGBA)
	# Using some special compiler flags for GBA
	include(CompilerFlagsGBA)
	# add the framework library subdirectory
	add_subdirectory(src)
else()
	message("Building for host")
endif()

add_subdirectory(test)
