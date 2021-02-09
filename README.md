# Game Boy Advance demo framework

This might not exactly fit your needs, but maybe you can learn / rip something from it. Uses devKitPro for its toolchain, linker scripts, headers and maxmod, but does not link libgba.

The [test](/test) subdirectory contains tests for some of the math / memory / etc. functions on GBA and PC.

If you find a bug or make an improvement your pull requests are appreciated.

## License

All of this is under the [MIT License](LICENSE).

## How to build?

### From the command line

* You need [CMake](https://cmake.org/)
* To compile for GBA you need [devkitPro / devKitARM](https://devkitpro.org) r52-1 or higher [installed](https://devkitpro.org/wiki/Getting_Started).
* Navigate to the folder, then:

```sh
mkdir build && cd build
cmake .. -DCMAKE_TOOLCHAIN_FILE=DevkitArmGBA.cmake
make -j $(grep -c '^processor' /proc/cpuinfo 2>/dev/null)
```

to compile the GBA version. Use ```cmake ..``` for the PC version. If you want to switch versions, you need to delete the build directory, because currently only on toolchain file can be used.

# Todo

* Clean up and improve code
* Make examples
* Make debugging from VS Code work
* Add amap task
* Disassenmbler task / info
* More tests
* Finish 3D stuff
