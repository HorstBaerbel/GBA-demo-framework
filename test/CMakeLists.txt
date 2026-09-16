cmake_minimum_required(VERSION 3.21)

if (GBA)
	add_subdirectory(gba)
else()
	add_subdirectory(pc)
endif()
