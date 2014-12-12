#!/bin/bash

# Some tests rely on the native extension performing type checks on the
# input parameters. To test this we need to run our tests in non-checked mode
dart --package-root=../packages lz4_test.dart

