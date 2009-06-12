#!/bin/sh
# Build System Wrapper
# Win32/cmd Variant

if ! [ -f build/bin/ice ] || ! [ -f build/bin/icemake ]; then
    if ./build-icemake.sh -fLoid build/; then
        PATH="./build/bin/:${PATH}" exec ice $@;
    fi
else
    PATH="./build/bin/:${PATH}" exec ice $@;
fi
