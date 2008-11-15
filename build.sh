#!/bin/sh

if ./build-icemake.sh; then exec build/bin/icemake $@; fi
