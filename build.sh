#!/bin/sh

if ./build-icemake.sh; then exec build/icemake $@; fi
