/*
 * This file is part of the kyuba.org Curie project.
 * See the appropriate repository at http://git.kyuba.org/ for exact file
 * modification records.
*/

/*
 * Copyright (c) 2008, 2009, Kyuba Project Members
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
*/

/*! \defgroup icemake Icemake Internals
 *  \internal
 *
 * @{
 */

/*! \file
 *  \brief Global Definitions for the ICE-Make Programme
 *
 */

#include <curie/sexpr.h>
#include <icemake/icemake-system.h>

#ifndef ICEMAKE_ICEMAKE_H
#define ICEMAKE_ICEMAKE_H

/*! \brief Length of any default Data Buffers */
#define BUFFERSIZE 4096
/*! \brief Length of any uname Fields */
#define UNAMELENGTH 128

/*! \brief Toolchain Codes */
enum toolchain
{
    tc_gcc      /*!< GNU GCC */
};

/*! \brief Operating System Codes */
enum operating_system
{
    os_unknown, /*!< Unknown Operating System */
    os_darwin,  /*!< Darwin/MacOSX */
    os_linux    /*!< Linux */
};

/*! \brief Instruction Set Codes */
enum instruction_set
{
    is_arm,     /*!< ARM-based Instruction Set */
    is_generic  /*!< Generic Instruction Set */
};

/*! \brief Filesystem Layout Codes
 *
 *  These codes are used when installing files, they determine where to install
 *  files with icemake -i.
 */
enum fs_layout
{
    /*! \brief FHS Layout
     *
     *  This is the most common layout on Linux systems these days, see their
     *  specs for a summary. Basically things get installed in
     *  $destdir/{bin|lib|share|...}.
     */
    fs_fhs,

    /*! \brief AFSL
     *
     *  This uses the AFSL, see the papers on kyuba.org for details.
     */
    fs_proper,

    /*! \brief FHS Layout, binaries to /lib
     *
     *  This is a slightly modified FHS layout that installs binaries into
     *  $destdir/lib/$programme/bin instead of $destdir/bin.
     */
    fs_fhs_binlib
};

/*! \brief Icemake Target
 *
 *  This structure contains all the information that is needed to build binaries
 *  and libraries from source code, as well as install all of it with auxiliary
 *  files.
 */
struct target {
    /*!\brief Programme/Library Short Name */
    sexpr name;
    /*!\brief Boolean: Is the Target a Library? */
    sexpr library;
    /*!\brief Boolean: Is the Target a Programme? */
    sexpr programme;
    /*!\brief The Libraries to link against */
    sexpr libraries;
    /*!\brief The Libraries that the icemake.sx specifies to link against */
    sexpr olibraries;
    /*!\brief Boolean: Hosted Environment? */
    sexpr hosted;
    /*!\brief Boolean: Uses Curie? */
    sexpr use_curie;
    /*!\brief List with all Code Files */
    sexpr code;
    /*!\brief List with all Test Cases */
    sexpr test_cases;
    /*!\brief List with all Test Case Reference Files */
    sexpr test_reference;
    /*!\brief List with Bootstrap Files (Curie only) */
    sexpr bootstrap;
    /*!\brief List with all Header Files */
    sexpr headers;
    /*!\brief List with Objects */
    sexpr use_objects;
    /*!\brief List with Data Files (for installation) */
    sexpr data;
    /*!\brief Programme/Library Name */
    sexpr dname;
    /*!\brief Programme/Library Description */
    sexpr description;
    /*!\brief Programme/Library Version */
    sexpr dversion;
    /*!\brief Programme/Library URL */
    sexpr durl;
    /*!\brief List of Documentation Files */
    sexpr documentation;
    /*!\brief Boolean: Using any C++ Files? */
    sexpr have_cpp;
};

/*! \brief Effective Operating System Name
 *
 *  As reported by uname(), or through the -t flag.
 */
extern char uname_os     [UNAMELENGTH];

/*! \brief Effective Operating System Architecture
 *
 *  As reported by uname(), or through the -t flag.
 */
extern char uname_arch   [UNAMELENGTH];

/*! \brief Effective Operating System Vendor Name
 *
 *  As reported by uname(), or through the -t flag.
 */
extern char uname_vendor [UNAMELENGTH];

/*! \brief Effective Toolchain Code
 *
 *  Derived through the -t flag, or guessed based on the uname data.
 */
extern enum toolchain uname_toolchain;

/*! \brief Effective Filesystem Layout
 *
 *  Modified through programme flags.
 */
extern enum fs_layout i_fsl;

/*! \brief Effective Operating System Code
 *
 *  Derived from uname_os.
 */
extern enum operating_system i_os;

/*! \brief Effective Instruction Set Code
 *
 *  Derived from uname_arch.
 */
extern enum instruction_set i_is;

/*! \brief Build Targets
 *
 *  Derived from icemake.sx and command-line arguments.
 */
extern struct tree targets;

/*! \brief Architecture Descriptor
 *
 *  Derived from the effective OS, architecture, vendor and toolchain.
 */
extern char  archbuffer [BUFFERSIZE];

/*! \brief Effective Architecture Descriptor
 *
 *  Derived from the effective OS, architecture, vendor and toolchain.
 */
extern char *archprefix;

/*! \brief Toolchain Version
 *
 *  Used to find specific toolchain binaries.
 */
extern char *tcversion;

/*! \brief Boolean: Optimise Linking */
extern sexpr i_optimise_linking;

/*! \brief Boolean: Combine C Code Files (for Optimisations) */
extern sexpr i_combine;

/*! \brief Boolean: Use Debug Code (Files in a debug/ Directory) */
extern sexpr i_debug;

/*! \brief Boolean: Create Shared Objects for Libraries */
extern sexpr i_dynamic_libraries;

/*! \brief Boolean: A freestanding Version of Curie is available */
extern sexpr co_freestanding;

/*! \brief List: Programmes to execute */
extern sexpr workstack;

/*! \brief Destination Directory
 *
 *  Set with the -d flag.
 */
extern sexpr i_destdir;

/*! \brief Programme Name
 *
 *  Set with the -b flag, used for the modified FHS layout.
 */
extern sexpr i_pname;

/*! \brief Boolean: Enforce Static Link
 *
 *  Set with the -S and -R flags.
 */
extern sexpr i_static;

/*! \brief (FHS) Library Directory
 *
 *  Used to decide between lib, lib32 and lib64.
 */
extern sexpr i_destlibdir;

/*! \brief Boolean: Run Test Cases
 *
 *  Set with the -r flag.
 */
extern sexpr do_tests;

/*! \brief Boolean: Install Binaries
 *
 *  Set with the -i flag.
 */
extern sexpr do_install;

/*! \brief Boolean: Build Documentation
 *
 *  Set with the -x flag.
 */
extern sexpr do_build_documentation;

/*! \brief C Compiler Binary
 *
 *  Automatically searched in the PATH according to the toolchain type.
 */
extern sexpr p_c_compiler;

/*! \brief C++ Compiler Binary
 *
 *  Automatically searched in the PATH according to the toolchain type.
 */
extern sexpr p_cpp_compiler;

/*! \brief Assembler Binary
 *
 *  Automatically searched in the PATH according to the toolchain type.
 */
extern sexpr p_assembler;

/*! \brief Linker Binary
 *
 *  Automatically searched in the PATH according to the toolchain type.
 */
extern sexpr p_linker;

/*! \brief Archiver Binary
 *
 *  Automatically searched in the PATH according to the toolchain type.
 */
extern sexpr p_archiver;

/*! \brief Diff Binary
 *
 *  Automatically searched in the PATH according to the toolchain type.
 */
extern sexpr p_diff;

/*! \brief LaTeX Binary
 *
 *  Automatically searched in the PATH according to the toolchain type.
 */
extern sexpr p_latex;

/*! \brief PDFLaTeX Binary
 *
 *  Automatically searched in the PATH according to the toolchain type.
 */
extern sexpr p_pdflatex;

/*! \brief Doxygen Binary
 *
 *  Automatically searched in the PATH according to the toolchain type.
 */
extern sexpr p_doxygen;

/*! \brief S-Expression Standard I/O Port */
extern struct sexpr_io *stdio;

/*! \brief Predefined Symbol */
define_symbol (sym_library,             "library");
/*! \brief Predefined Symbol */
define_symbol (sym_libraries,           "libraries");
/*! \brief Predefined Symbol */
define_symbol (sym_test_cases,          "test-cases");
/*! \brief Predefined Symbol */
define_symbol (sym_test_case_reference, "test-case-reference");
/*! \brief Predefined Symbol */
define_symbol (sym_programme,           "programme");
/*! \brief Predefined Symbol */
define_symbol (sym_hosted,              "hosted");
/*! \brief Predefined Symbol */
define_symbol (sym_code,                "code");
/*! \brief Predefined Symbol */
define_symbol (sym_headers,             "headers");
/*! \brief Predefined Symbol */
define_symbol (sym_link,                "link");
/*! \brief Predefined Symbol */
define_symbol (sym_use_objects,         "use-objects");
/*! \brief Predefined Symbol */
define_symbol (sym_assembly,            "assembly");
/*! \brief Predefined Symbol */
define_symbol (sym_preproc_assembly,    "preprocessed-assembly");
/*! \brief Predefined Symbol */
define_symbol (sym_cpp,                 "C++");
/*! \brief Predefined Symbol */
define_symbol (sym_c,                   "C");
/*! \brief Predefined Symbol */
define_symbol (sym_assembly_pic,        "assembly-pic");
/*! \brief Predefined Symbol */
define_symbol (sym_preproc_assembly_pic,"preprocessed-assembly-pic");
/*! \brief Predefined Symbol */
define_symbol (sym_c_pic,               "C-PIC");
/*! \brief Predefined Symbol */
define_symbol (sym_cpp_pic,             "C++-PIC");
/*! \brief Predefined Symbol */
define_symbol (sym_tex,                 "TeX");
/*! \brief Predefined Symbol */
define_symbol (sym_man,                 "man");
/*! \brief Predefined Symbol */
define_symbol (sym_libc,                "libc");
/*! \brief Predefined Symbol */
define_symbol (sym_libcurie,            "libcurie");
/*! \brief Predefined Symbol */
define_symbol (sym_freestanding,        "freestanding");
/*! \brief Predefined Symbol */
define_symbol (sym_freestanding_if_asm, "freestanding-if-assembly");
/*! \brief Predefined Symbol */
define_symbol (sym_data,                "data");
/*! \brief Predefined Symbol */
define_symbol (sym_description,         "description");
/*! \brief Predefined Symbol */
define_symbol (sym_version,             "version");
/*! \brief Predefined Symbol */
define_symbol (sym_name,                "name");
/*! \brief Predefined Symbol */
define_symbol (sym_url,                 "url");
/*! \brief Predefined Symbol */
define_symbol (sym_failed,              "failed");
/*! \brief Predefined Symbol */
define_symbol (sym_documentation,       "documentation");
/*! \brief Predefined Symbol */
define_symbol (sym_missing_programme,   "missing-programme");
/*! \brief Predefined Symbol */
define_symbol (sym_latex,               "latex");
/*! \brief Predefined Symbol */
define_symbol (sym_pdflatex,            "pdflatex");
/*! \brief Predefined Symbol */
define_symbol (sym_doxygen,             "doxygen");
/*! \brief Predefined Symbol */
define_symbol (sym_chdir,               "chdir");
/*! \brief Predefined Symbol */
define_symbol (sym_symlink,             "symlink");
/*! \brief Predefined Symbol */
define_symbol (sym_install,             "install");
/*! \brief Predefined Symbol */
define_symbol (sym_items_remaining,     "items-remaining");
/*! \brief Predefined Symbol */
define_symbol (sym_items_total,         "items-total");
/*! \brief Predefined Symbol */
define_symbol (sym_completed,           "completed");
/*! \brief Predefined Symbol */
define_symbol (sym_phase,               "phase");
/*! \brief Predefined Symbol */
define_symbol (sym_build,               "build");
/*! \brief Predefined Symbol */
define_symbol (sym_build_documentation, "build-documentation");
/*! \brief Predefined Symbol */
define_symbol (sym_run_tests,           "run-tests");
/*! \brief Predefined Symbol */
define_symbol (sym_cross_link,          "cross-link");
/*! \brief Predefined Symbol */
define_symbol (sym_post_process,        "post-process");
/*! \brief Predefined Symbol */
define_symbol (sym_execute,             "execute");
/*! \brief Predefined Symbol */
define_symbol (sym_targets,             "targets");

/*! \brief Predefined String */
define_string (str_bootstrap,           "bootstrap");
/*! \brief Predefined String */
define_string (str_curie,               "curie");
/*! \brief Predefined String */
define_string (str_curie_bootstrap,     "curie-bootstrap");
/*! \brief Predefined String */
define_string (str_static,              "-static");
/*! \brief Predefined String */
define_string (str_dshared,             "-shared");
/*! \brief Predefined String */
define_string (str_ddynamiclib,         "-dynamiclib");
/*! \brief Predefined String */
define_string (str_lc,                  "c");
/*! \brief Predefined String */
define_string (str_dlc,                 "-lc");
/*! \brief Predefined String */
define_string (str_dlcurie,             "-lcurie");
/*! \brief Predefined String */
define_string (str_do,                  "-o");
/*! \brief Predefined String */
define_string (str_dc,                  "-c");
/*! \brief Predefined String */
define_string (str_dr,                  "-r");
/*! \brief Predefined String */
define_string (str_ds,                  "-s");
/*! \brief Predefined String */
define_string (str_dposix,              "-DPOSIX");
/*! \brief Predefined String */
define_string (str_dgcc,                "-DGCC");
/*! \brief Predefined String */
define_string (str_src,                 "src");
/*! \brief Predefined String */
define_string (str_tests,               "tests");
/*! \brief Predefined String */
define_string (str_include,             "include");
/*! \brief Predefined String */
define_string (str_data,                "data");
/*! \brief Predefined String */
define_string (str_stdc99,              "--std=c99");
/*! \brief Predefined String */
define_string (str_wall,                "-Wall");
/*! \brief Predefined String */
define_string (str_pedantic,            "-pedantic");
/*! \brief Predefined String */
define_string (str_dcombine,            "-combine");
/*! \brief Predefined String */
define_string (str_lib,                 "lib");
/*! \brief Predefined String */
define_string (str_gcc,                 "gcc");
/*! \brief Predefined String */
define_string (str_documentation,       "documentation");
/*! \brief Predefined String */
define_string (str_dstart_group,        "-Wl,--start-group");
/*! \brief Predefined String */
define_string (str_dend_group,          "-Wl,--end-group");
/*! \brief Predefined String */
define_string (str_dfpic,               "-fPIC");
/*! \brief Predefined String */
define_string (str_supcpp,              "supc++");
/*! \brief Predefined String */
define_string (str_gcc_eh,              "gcc_eh");
/*! \brief Predefined String */
define_string (str_nostdlib,            "-nostdlib");
/*! \brief Predefined String */
define_string (str_nodefaultlibs,       "-nodefaultlibs");
/*! \brief Predefined String */
define_string (str_nostartfiles,        "-nostartfiles");
/*! \brief Predefined String */
define_string (str_doxyfile,            "doxyfile");

/*! \brief Prefix with a C String
 *  \param[in] f The C string to add a prefix to.
 *  \param[in] p The prefix to add.
 *  \return The prefixed directory.
 */
sexpr sx_string_dir_prefix_c (char *f, sexpr p);

/*! \brief Build Targets
 *  \param[in] targets The targets to build.
 */
void build (sexpr targets);

/*! \brief Install Targets
 *  \param[in] targets The targets to install.
 */
void install (sexpr targets);

/*! \brief Run Test Cases
 *  \param[in] targets The targets to run test cases for.
 */
void run_tests (sexpr targets);

/*! \brief Link Targets
 *  \param[in] targets The targets to link.
 */
void ice_link (sexpr targets);

/*! \brief Post Process Targets
 *  \param[in] targets The targets to post process.
 */
void post_process (sexpr targets);

/*! \brief Build Documentation
 *  \param[in] targets The targets to build documentation for.
 */
void build_documentation (sexpr targets);

/*! \brief Crosslink Targets
 *  \param[in] targets The targets to cross-link.
 *
 *  This step is a bit tricky, it's supposed to be used when directly joining
 *  multiple libraries during the build.
 */
void crosslink_objects ();

/*! \brief Loop over all Processes
 *
 *  This will go through the workstack and process all of the items. If any of
 *  the programmes fail, icemake terminates.
 */
void loop_processes ();

/*! \brief Loop over all Processes
 *
 *  This will go through the workstack and process all of the items. Icemake
 *  won't die if any of the executed programmes die, instead this will count
 *  how many have failed and finally return that. This is used for test cases.
 */
void loop_processes_nokill ();

/*! \brief Count Workstack Items
 *
 *  This function will count the number of workstack items and print them to
 *  stdio as "(items-total X)", if there's more than one item.
 */
void count_print_items ();

#endif

/*! @} */
