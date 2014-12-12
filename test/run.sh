#!/bin/bash

# Make sure cmake is installed and use it to build
# our native extension before running the tests
sudo apt-get install cmake
cmake .
make

# Some tests rely on the native extension performing type checks on the
# input parameters. To test this we need to run our tests in non-checked mode
dart --package-root=../packages lz4_test.dart

