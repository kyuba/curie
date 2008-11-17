#!/bin/sh

if ./build-icemake.sh -i build/ -f; then exec build/bin/icemake $@; fi
