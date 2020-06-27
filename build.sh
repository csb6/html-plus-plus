#!/usr/bin/env sh

set -e

compiler=g++-9
standard=-"-std=c++2a"
flags="-Wall -Wextra -pedantic-errors"

$compiler $standard $flags $@ -o example-all-elements example-all-elements.cpp
$compiler $standard $flags $@ -o example-readme-1 example-readme-1.cpp
