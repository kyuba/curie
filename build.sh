#!/bin/sh
# Build System Wrapper
# POSIX/sh Variant

if ! [ -f build/bin/icemake ]; then
    if ./build-icemake.sh -foid build/; then
        PATH="./build/bin/:${PATH}" exec icemake $@;
    fi
else
    PATH="./build/bin/:${PATH}" exec icemake $@;
fi
