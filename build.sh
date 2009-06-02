#!/bin/sh
# Build System Wrapper
# Win32/cmd Variant

if ./build-icemake.sh -fLoid build/; then
  PATH="./build/bin/:${PATH}" exec ice $@;
fi
