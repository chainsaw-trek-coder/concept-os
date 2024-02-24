#!/bin/bash

cmake . -B ../../build/kernel

pushd ../../build/kernel
make
popd