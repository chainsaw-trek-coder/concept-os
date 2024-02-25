#!/bin/bash

mkdir -p ../../build/kernel
cmake . -B ../../build/kernel

pushd ../../build/kernel
make
popd