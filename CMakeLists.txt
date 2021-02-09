cmake_minimum_required(VERSION 3.1.0)

project(framework)

# Add the cmake folder to the modules paths, so that we can use the tools and find_package for ctrulib
list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_LIST_DIR}/cmake)

if (GBA)
	# ASM must be enabled to support .S files
	enable_language(ASM)
	# Include all the macros and tools needed for GBA development.
	include(ToolsGBA)
	add_subdirectory(src)
	add_subdirectory(test/gba)
else()
	add_subdirectory(test/pc)
endif()
