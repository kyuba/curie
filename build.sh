#!/bin/sh
# Build System Bootstrap
# POSIX/sh Variant

CURIE_BUILD_FILES="build sexpr memory memory-pool memory-allocator io string io-system sexpr-read-write sexpr-library tree immutable"

if [ ! -n "${CC}" ]; then CC='cc'; fi
if [ ! -n "${LD}" ]; then LD='cc'; fi
if [ ! -n "${TOOLCHAINTYPE}" ]; then TOOLCHAINTYPE='gcc'; fi
if [ ! -d build ]; then mkdir build/; fi

gcc_buildfile() {
    COMMAND="${CC} -o build/${2}.o -c ${1} -DPOSIX -DGCC -Iinclude/generic/  -Iinclude/internal/ -Wall -pedantic --std=c99"

    echo ${COMMAND}

    if ${COMMAND}; then return; else exit 3; fi
}

gcc_linkbinary() {
    COMMAND="${LD} -o build/${BINARY}"
    for i in $@; do
        COMMAND="${COMMAND} build/${i}.o";
    done

    echo ${COMMAND}

    if ${COMMAND}; then return; else exit 4; fi
}

gcc_buildall() {
    for i in $@; do
        if [ -e src/posix/${i}.c ]; then
            buildfile src/posix/${i}.c ${i};
        elif [ -e src/generic/${i}.c ]; then
            buildfile src/generic/${i}.c ${i};
        else
            echo Missing Code File: ${i}.c;
            exit 2;
        fi
    done
}

buildfile() {
    if [ -e build/${2}.o ]; then
        if [ "$(stat -c %Y build/${2}.o)" -lt "$(stat -c %Y ${1})" ]; then
            rm build/${2}.o;
        else
#            echo File exists: build/${2}.o;
            return;
        fi;
    fi;

    ${TOOLCHAINTYPE}_buildfile $@;
}

buildall() {
    ${TOOLCHAINTYPE}_buildall $@;
}

linkbinary() {
    if [ -e build/${BINARY} ]; then
        for i in $@; do
            if [ "$(stat -c %Y build/${BINARY})" -lt "$(stat -c %Y build/${i}.o)" ]; then
                rm build/${BINARY};
                ${TOOLCHAINTYPE}_linkbinary $@;
                return;
            fi;
        done
#        echo File exists: build/${BINARY};
        return;
    fi;

    ${TOOLCHAINTYPE}_linkbinary $@;
}

buildall ${CURIE_BUILD_FILES}
BINARY=curie-build linkbinary ${CURIE_BUILD_FILES}

exec build/curie-build $@
