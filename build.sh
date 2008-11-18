#!/bin/sh

if ./build-icemake.sh -fLid build/; then exec build/bin/icemake $@; fi
