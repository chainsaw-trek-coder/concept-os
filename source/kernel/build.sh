#!/bin/bash

mkdir -p ../../build/kernel
cmake ./source -B ../../build/kernel

pushd ../../build/kernel
make
popd