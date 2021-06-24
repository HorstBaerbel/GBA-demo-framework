# Game Boy Advance C(++) demo framework

This is unfinished and might not exactly fit your needs, but maybe you can learn / rip something from it. Uses [devKitPro](https://devkitpro.org/) for its tools, compilers, linker scripts, headers and maxmod, but does not link libgba. Also cmake toolchain files from [3ds-cmake](https://github.com/Xtansia/3ds-cmake) are used.

The [src](src) directory contains the framework while the [test](/test) directory contains tests for some of the math / memory / etc. functions on GBA and PC.

If you find a bug or make an improvement your pull requests are appreciated.

## License

All of this is under the [MIT License](LICENSE).

## How to build?

### From the command line

* You need [CMake](https://cmake.org/) 3.1.0 or higher
* To compile for GBA you need [devkitPro / devKitARM](https://devkitpro.org) r52-1 or higher [installed](https://devkitpro.org/wiki/Getting_Started).
* Navigate to the folder, then:

```sh
mkdir build && cd build
cmake .. -DCMAKE_TOOLCHAIN_FILE=3ds-cmake/DevkitArmGBA.cmake
make -j $(grep -c '^processor' /proc/cpuinfo 2>/dev/null)
```

to compile the GBA version. Use ```cmake ..``` (aka default toolchain) for the PC version. If you want to switch versions, you need to delete the build directory, because currently only one toolchain file can be used.

## Visual Studio Code extensions

### Necessary

* C/C++ IntelliSense, debugging, and code browsing [ms-vscode.cpptools](https://marketplace.visualstudio.com/items?itemName=ms-vscode.cpptools)
* Extended CMake support in Visual Studio Code [ms-vscode.cmake-tools](https://marketplace.visualstudio.com/items?itemName=ms-vscode.cmake-tools)
* GDB, LLDB & Mago-MI Debugger support for VSCode [webfreak.debug](https://marketplace.visualstudio.com/items?itemName=webfreak.debug)

### Recommended

* ARM support for Visual Studio Code [dan-c-underwood.arm](https://marketplace.visualstudio.com/items?itemName=dan-c-underwood.arm)
* Show disassembly to source relations [dseight.disasexpl](https://marketplace.visualstudio.com/items?itemName=dseight.disasexpl)
* CMake langage support for Visual Studio Code [twxs.cmake](https://marketplace.visualstudio.com/items?itemName=twxs.cmake)

## Tools and further info

### Visualizing .map files

The build process generates .map files for each executable built. It can be interesting to see which code and variables go into which section, e.g. in case you're running out of IWRAM etc. There are a couple of tool to visualize those .map files:

* [AMAP](http://www.sikorskiy.net/prj/amap/) (Windows / Linux / Mac)
* [ebs-universe / fpv-gcc](https://github.com/ebs-universe/fpv-gcc) (Python. Command line tool)
* [govind-mukundan / MapViewer](https://github.com/govind-mukundan/MapViewer) (Releases Windows only)
* [PromyLOPh / linkermapviz](https://github.com/PromyLOPh/linkermapviz) (Python, generates HTML view)
* [jotux / GccMapVisualizer](https://github.com/jotux/GccMapVisualizer) (Online [Upload and view .map here](http://jotux.github.io/GccMapVisualizer/))

### Analyzing ELF files

* [CapeLeidokos / elf_diff](https://github.com/CapeLeidokos/elf_diff) (Python. Compares two ELF files and displays adiff of their memory layout)

### Running the toolchain in a container

There's a nice writeup by James Grimwood [here](https://www.gamedev.net/blogs/entry/2268899-gameboy-advance-dev-workflow-in-2020/).

## Todo

* Clean up and improve code
* Add example source code
* Document files and usage
* Try to get disassembly explorer to work
* More tests
* Finish 2D and 3D stuff
