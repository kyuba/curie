#!/bin/sh

if ./build-icemake.sh -d build/ -f -i; then exec build/bin/icemake $@; fi
