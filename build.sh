#!/bin/sh

if ./build-icemake.sh -fLoid build/; then exec build/bin/icemake $@; fi
