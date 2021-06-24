cmake_minimum_required(VERSION 3.1.0)

project(framework)

# Add the cmake folder to the modules paths, so that we can use the tools and find_package for ctrulib
list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_LIST_DIR}/3ds-cmake/cmake)

if (GBA)
	# ASM must be enabled to support .S files
	enable_language(ASM)
	# Include all the macros and tools needed for GBA development.
	include(ToolsGBA)

	# Using some special compiler flags for GBA:
	# Link-time optimization: -flto
	# Assembly output: -save-temps / or -save-temps=obj (subdirectories) (delete build directory first!)
	# Minimum library includes: -ffreestanding -nostartfiles -nostdlib -nodefaultlibs
	# Replace compiler memory allocation with our own: -Wl,--wrap=malloc,--wrap=free,--wrap=alloc,--wrap=calloc
	# See also: https://gcc.gnu.org/onlinedocs/gcc/ARM-Options.html
	set(ARCH "-march=armv4t -mthumb -mthumb-interwork -mlong-calls -Wl,--wrap=malloc,--wrap=free,--wrap=alloc,--wrap=calloc")
	set(COMPILERFLAGS "-save-temps -Wall -mcpu=arm7tdmi -mtune=arm7tdmi -fomit-frame-pointer -ffast-math -fno-aggressive-loop-optimizations -no-pie -fno-stack-protector")
	set(CMAKE_ASM_FLAGS "${CMAKE_ASM_FLAGS} -Wa,--warn -x assembler-with-cpp ${ARCH}")
	set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${ARCH} ${COMPILERFLAGS} -std=c11")
	set(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS} -Og -g")
	set(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS} -O2")
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${ARCH} ${COMPILERFLAGS} -std=c++14 -fconserve-space -fno-threadsafe-statics -fno-rtti -fno-exceptions")
	set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS} -Og -g")
	set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS} -O2")
	set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} ${ARCH}")
	set(CMAKE_EXE_LINKER_FLAGS_DEBUG "${CMAKE_EXE_LINKER_FLAGS}")
	set(CMAKE_EXE_LINKER_FLAGS_RELEASE "${CMAKE_EXE_LINKER_FLAGS} -s")

	# message("CMAKE_BUILD_TYPE: " ${CMAKE_BUILD_TYPE})
	# message("CMAKE_C_FLAGS: " ${CMAKE_C_FLAGS})
	# message("CMAKE_C_FLAGS_RELEASE: " ${CMAKE_C_FLAGS_RELEASE})
	# message("CMAKE_C_FLAGS_DEBUG: " ${CMAKE_C_FLAGS_DEBUG})
	# message("CMAKE_CXX_FLAGS: " ${CMAKE_CXX_FLAGS})
	# message("CMAKE_CXX_FLAGS_RELEASE: " ${CMAKE_CXX_FLAGS_RELEASE})
	# message("CMAKE_CXX_FLAGS_DEBUG: " ${CMAKE_CXX_FLAGS_DEBUG})
	# message("CMAKE_EXE_LINKER_FLAGS: " ${CMAKE_EXE_LINKER_FLAGS})
endif()

add_subdirectory(src)
add_subdirectory(test)
