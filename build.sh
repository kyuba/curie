#!/bin/sh

if ./build-icemake.sh -fLoid build/; then
  PATH="./build/bin/:${PATH}" exec ice $@;
fi
