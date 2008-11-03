#!/bin/sh

if ./build-icemake.sh; then exec build/b-icemake $@; fi
