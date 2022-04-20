# joc

## Install

Make sure git submodules are initialized and up to date. joc lib is built on top of nlohmann::json library.

> git submodule update --init --recursive

## Compile sample

The main CMakeLists.txt includes building the `joc` library, plus a sample executable.

> mkdir -p build && cd build
> cmake ..
> make
> ./joc_sample

## Compile unit-tests

Jump to tests/ directory and run the same commands as shown above:

> cd tests/
> mkdir -p build && cd build
> cmake ..
> make
> ./jsonobjectconverter_test
