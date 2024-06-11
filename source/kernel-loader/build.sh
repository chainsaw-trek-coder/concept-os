#!/bin/bash

mkdir -p ../../build/kernel-loader
cmake ./source -B ../../build/kernel-loader

pushd ../../build/kernel-loader
make
popd