cmake_minimum_required(VERSION 3.1.0)

if (GBA)
	add_subdirectory(gba)
else()
	add_subdirectory(pc)
endif()
