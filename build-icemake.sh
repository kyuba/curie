#!/bin/sh
# Build System Bootstrap
# POSIX/sh Variant

ICEMAKE_FILES="icemake icemake-build icemake-install icemake-link icemake-tc-gcc icemake-os-generic icemake-stubs sexpr memory memory-pool memory-allocator io string io-system sexpr-read-write sexpr-library tree-basic immutable multiplex multiplex-signal multiplex-process multiplex-io multiplex-system signal-system exec exec-system network network-system multiplex-sexpr filesystem sexpr-stdio stdio utf-8 sexpr-custom hash tree-string tree-library gcd variables libc-compat io-pool sexpr-set bootstrap shell regex graph sexpr-sort metadata-path metadata-unix time-unix icemake-vis-raw icemake-vis-ice"

if [ ! -n "${CC}" ]; then CC='cc'; fi
if [ ! -n "${LD}" ]; then LD='cc'; fi
if [ ! -n "${TOOLCHAINTYPE}" ]; then TOOLCHAINTYPE='gcc'; fi
if [ ! -d build ]; then mkdir build/; fi

gcc_buildfile() {
    COMMAND="${CC} ${CFLAGS} ${CCFLAGS} -DNOVERSION -o build/${2}.o -c ${1} -Iinclude/ -Wall -pedantic --std=c99"

    echo ${COMMAND}

    if ${COMMAND}; then return; else exit 3; fi
}

gcc_linkbinary() {
    COMMAND="${LD} ${LDFLAGS} -o build/${BINARY}"
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
        elif [ -e src/${i}.c ]; then
            buildfile src/${i}.c ${i};
        else
            echo Missing Code File: ${i}.c;
            exit 2;
        fi
    done
}

buildfile() {
    if [ -e build/${2}.o ]; then
        if [ build/${2}.o -ot ${1} ]; then
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
            if [ build/${BINARY} -ot build/${i}.o ]; then
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

buildall ${ICEMAKE_FILES}
BINARY=b-icemake linkbinary ${ICEMAKE_FILES}

./build/b-icemake $@ curie-bootstrap syscall curie sievert icemake ice
