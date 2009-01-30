/*
 *  icemake.h
 *  libcurie/icemake
 *
 *  Created by Magnus Deininger on 17/11/2008.
 *  Copyright 2008, 2009 Magnus Deininger. All rights reserved.
 *
 */

/*
 * Copyright (c) 2008, 2009, Magnus Deininger All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are
 * met:
 *
 * * Redistributions of source code must retain the above copyright
 * notice, this list of conditions and the following disclaimer. *
 * Redistributions in binary form must reproduce the above copyright
 * notice, this list of conditions and the following disclaimer in the
 * documentation and/or other materials provided with the distribution. *
 * Neither the name of the project nor the names of its contributors may
 * be used to endorse or promote products derived from this software
 * without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS
 * IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED
 * TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A
 * PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 * OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

/*! \internal
 *
 * @{
 */

/*! \file
 *  \brief Global Definitions for the ICE-Make Programme
 *
 */

#include <curie/sexpr.h>

#ifndef ICEMAKE_ICEMAKE_H
#define ICEMAKE_ICEMAKE_H

#define BUFFERSIZE 4096
#define UNAMELENGTH 128

enum toolchain
{
    tc_gcc
};

enum operating_system
{
    os_unknown,
    os_darwin,
    os_linux
};

struct target {
    sexpr name;
    sexpr library;
    sexpr libraries;
    sexpr olibraries;
    sexpr hosted;
    sexpr use_curie;
    sexpr code;
    sexpr test_cases;
    sexpr test_reference;
    sexpr bootstrap;
    sexpr headers;
    sexpr use_objects;
    sexpr data;
    sexpr dname;
    sexpr description;
    sexpr dversion;
    sexpr durl;
    sexpr documentation;
    sexpr have_cpp;
};

enum fs_layout
{
    fs_fhs,
    fs_proper,
    fs_fhs_binlib
};

extern char uname_os     [UNAMELENGTH];
extern char uname_arch   [UNAMELENGTH];
extern char uname_vendor [UNAMELENGTH];

extern enum toolchain uname_toolchain;
extern enum fs_layout i_fsl;
extern enum operating_system i_os;

extern struct tree targets;

extern char  archbuffer [BUFFERSIZE];
extern char *archprefix;
extern char *tcversion;

extern sexpr i_optimise_linking;
extern sexpr i_combine;
extern sexpr i_debug;

extern sexpr co_freestanding;
extern sexpr workstack;

extern sexpr i_destdir;
extern sexpr i_pname;
extern sexpr i_static;
extern sexpr i_destlibdir;
extern sexpr do_tests;
extern sexpr do_install;
extern sexpr do_build_documentation;

extern sexpr p_c_compiler;
extern sexpr p_cpp_compiler;
extern sexpr p_assembler;
extern sexpr p_linker;
extern sexpr p_archiver;
extern sexpr p_diff;

extern sexpr p_latex;
extern sexpr p_pdflatex;
extern sexpr p_doxygen;

extern struct sexpr_io *stdio;

define_symbol (sym_library,             "library");
define_symbol (sym_libraries,           "libraries");
define_symbol (sym_test_cases,          "test-cases");
define_symbol (sym_test_case_reference, "test-case-reference");
define_symbol (sym_programme,           "programme");
define_symbol (sym_hosted,              "hosted");
define_symbol (sym_code,                "code");
define_symbol (sym_headers,             "headers");
define_symbol (sym_link,                "link");
define_symbol (sym_use_objects,         "use-objects");
define_symbol (sym_assembly,            "assembly");
define_symbol (sym_preproc_assembly,    "preprocessed-assembly");
define_symbol (sym_cpp,                 "C++");
define_symbol (sym_c,                   "C");
define_symbol (sym_assembly_pic,        "assembly-pic");
define_symbol (sym_preproc_assembly_pic,"preprocessed-assembly-pic");
define_symbol (sym_c_pic,               "C-PIC");
define_symbol (sym_cpp_pic,             "C++-PIC");
define_symbol (sym_tex,                 "TeX");
define_symbol (sym_libc,                "libc");
define_symbol (sym_libcurie,            "libcurie");
define_symbol (sym_freestanding,        "freestanding");
define_symbol (sym_freestanding_if_asm, "freestanding-if-assembly");
define_symbol (sym_data,                "data");
define_symbol (sym_description,         "description");
define_symbol (sym_version,             "version");
define_symbol (sym_name,                "name");
define_symbol (sym_url,                 "url");
define_symbol (sym_failed,              "failed");
define_symbol (sym_documentation,       "documentation");
define_symbol (sym_missing_programme,   "missing-programme");
define_symbol (sym_latex,               "latex");
define_symbol (sym_pdflatex,            "pdflatex");
define_symbol (sym_doxygen,             "doxygen");
define_symbol (sym_chdir,               "chdir");

define_string (str_bootstrap,           "bootstrap");
define_string (str_curie,               "curie");
define_string (str_curie_bootstrap,     "curie-bootstrap");
define_string (str_static,              "-static");
define_string (str_dshared,             "-shared");
define_string (str_lc,                  "c");
define_string (str_do,                  "-o");
define_string (str_dc,                  "-c");
define_string (str_dr,                  "-r");
define_string (str_ds,                  "-s");
define_string (str_dposix,              "-DPOSIX");
define_string (str_dgcc,                "-DGCC");
define_string (str_src,                 "src");
define_string (str_tests,               "tests");
define_string (str_include,             "include");
define_string (str_data,                "data");
define_string (str_stdc99,              "--std=c99");
define_string (str_wall,                "-Wall");
define_string (str_pedantic,            "-pedantic");
define_string (str_dcombine,            "-combine");
define_string (str_lib,                 "lib");
define_string (str_gcc,                 "gcc");
define_string (str_documentation,       "documentation");
define_string (str_dstart_group,        "-Wl,--start-group");
define_string (str_dend_group,          "-Wl,--end-group");
define_string (str_buildicemakeld,      "build/icemake.ld");
define_string (str_dfpic,               "-fPIC");

sexpr sx_string_dir_prefix_c (char *, sexpr);

void build (sexpr);
void install (sexpr);
void run_tests (sexpr);
void ice_link (sexpr);
void post_process (sexpr);
void build_documentation (sexpr);
void crosslink_objects ();

void loop_processes ();

#endif

/*! @} */
