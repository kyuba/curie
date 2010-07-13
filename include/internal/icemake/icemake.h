/*
 * This file is part of the kyuba.org Curie project.
 * See the appropriate repository at http://git.kyuba.org/ for exact file
 * modification records.
*/

/*
 * Copyright (c) 2008-2010, Kyuba Project Members
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

#define _BSD_SOURCE

#include <sievert/tree.h>
#include <sievert/sexpr.h>
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
    tc_unknown, /*!< Unknown Toolchain */
    tc_generic, /*!< Generic Toolchain */
    tc_gcc,     /*!< GNU GCC */
    tc_borland, /*!< Borland C/C++ Compiler */
    tc_msvc,    /*!< Microsoft Visual C++ */
    tc_latex,   /*!< TeX (LaTeX distribution) */
    tc_doxygen  /*!< Doxygen */
};

/*! \brief Operating System Codes */
enum operating_system
{
    os_unknown,     /*!< Unknown Operating system */
    os_generic,     /*!< Generic Operating System */
    os_darwin,      /*!< Darwin/MacOSX */
    os_linux,       /*!< Linux */
    os_windows,     /*!< Windows */
    os_freebsd,     /*!< FreeBSD */
    os_netbsd,      /*!< NetBSD */
    os_openbsd,     /*!< OpenBSD */
    os_dragonflybsd /*!< DragonflyBSD */
};

/*! \brief Instruction Set Codes */
enum instruction_set
{
    is_unknown, /*!< Unknown Instruction Set */
    is_generic, /*!< Generic Instruction Set */
    is_arm,     /*!< ARM-based Instruction Set */
    is_x86,     /*!< X86-based Instruction Set */
    is_mips,    /*!< MIPS-based Instruction Set */
    is_powerpc, /*!< PowerPC-based Instruction Set */
    is_sh       /*!< SH-based Instruction Set */
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
    fs_afsl,

    /*! \brief FHS Layout, binaries to /lib
     *
     *  This is a slightly modified FHS layout that installs binaries into
     *  $destdir/lib/$programme/bin instead of $destdir/bin.
     */
    fs_fhs_binlib
};

enum icemake_error
{
    ie_missing_description_file,
    ie_missing_tool,
    ie_missing_header,
    ie_missing_test_case,
    ie_missing_documentation,
    ie_missing_data,
    ie_missing_code_file,
    ie_invalid_choice,
    ie_failed_to_spawn_subprocess,
    ie_failed_to_execute_binary_image,
    ie_problematic_signal,
    ie_unknown_code_file_type,
    ie_programme_failed
};

enum icemake_invocation
{
    ii_build,
    ii_link,
    ii_install,
    ii_test,
    ii_documentation
};

enum visualiser
{
    vs_raw,
    vs_ice
};

struct target;
struct icemake;

struct toolchain_gcc
{
    sexpr gcc;
    sexpr ld;
    sexpr gpp;
    sexpr as;
    sexpr ar;
};

struct toolchain_borland
{
    sexpr bcc32;
    sexpr tlib;
};

struct toolchain_msvc
{
    sexpr cl;
    sexpr rc;
    sexpr link;
    sexpr lib;
};

struct toolchain_latex
{
    sexpr latex;
    sexpr pdflatex;
};

struct toolchain_doxygen
{
    sexpr doxygen;
};

struct operating_system_generic
{
    sexpr diff;
};

#define IS_32_BIT 0x0001
#define IS_64_BIT 0x0002

struct toolchain_descriptor
{
    enum toolchain        toolchain;
    enum operating_system operating_system;
    enum instruction_set  instruction_set;

    sexpr                 original_toolchain;

    const char           *uname_arch;
    const char           *uname_os;
    const char           *uname_vendor;
    const char           *uname_toolchain;

    int (*build)         (struct target *);
    int (*link)          (struct target *);
    int (*install)       (struct target *);
    int (*test)          (struct target *);

    int (*install_file)  (struct icemake *im, sexpr spec);

    union
    {
        struct toolchain_gcc     gcc;
        struct toolchain_borland borland;
        struct toolchain_msvc    msvc;
        struct toolchain_latex   latex;
        struct toolchain_doxygen doxygen;
    } meta_toolchain;

    union
    {
        struct operating_system_generic generic;
    } meta_operating_system;

    unsigned long instruction_set_options;
    unsigned long instruction_set_level;
 
    unsigned long options;
};

struct visualiser_raw
{
    struct sexpr_io *stdio;
};

struct visualiser_ice
{
    struct io *out;
};

struct visualiser_descriptor
{
    enum visualiser visualiser;

    void (*on_command) (sexpr);

    union
    {
        struct visualiser_raw raw;
        struct visualiser_ice ice;
    } meta;
};

#define ICEMAKE_OPTION_FREESTANDING    (1 << 0x0)

struct icemake
{
    enum fs_layout filesystem_layout;

    void (*on_error)   (enum icemake_error error, const char *text);
    void (*on_warning) (enum icemake_error error, const char *text);

    sexpr buildtargets;

    /*! \brief S-Expression Standard I/O Port */
    struct sexpr_io *stdio;

    /*! \brief Build Targets
     *
     *  Derived from icemake.sx and command-line arguments.
     */
    struct tree targets;

    struct toolchain_descriptor *toolchain;

    int alive_processes;

    unsigned long options;

    /*! \brief List: Queued tasks */
    sexpr workstack;
};

#define ICEMAKE_PROGRAMME              (1 << 0x0)
#define ICEMAKE_LIBRARY                (1 << 0x1)
#define ICEMAKE_HAVE_CPP               (1 << 0x2)
#define ICEMAKE_HOSTED                 (1 << 0x3)
#define ICEMAKE_USE_CURIE              (1 << 0x4)
#define ICEMAKE_NO_SHARED_LIBRARY      (1 << 0x5)

/*! \brief Icemake Target
 *
 *  This structure contains all the information that is needed to build binaries
 *  and libraries from source code, as well as install all of it with auxiliary
 *  files.
 */
struct target {
    /*!\brief Programme/Library Short Name */
    sexpr name;
    /*!\brief The Libraries to link against */
    sexpr libraries;
    /*!\brief The Libraries to link against */
    sexpr deffile;
    /*!\brief The Libraries that the icemake.sx specifies to link against */
    sexpr olibraries;
    /*!\brief List with all Code Files */
    sexpr code;
    /*!\brief List with all Test Cases */
    sexpr test_cases;
    /*!\brief List with all Test Case Reference Files */
    sexpr test_reference;
    /*!\brief List with all Header Files */
    sexpr headers;
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
    /*!\brief Number: How often this target has been built. */
    sexpr buildnumber;
    /*!\brief Options */
    unsigned int options;
    /*!\brief Base Path for Build Data */
    sexpr buildbase;
    /*!\brief Toolchain Descriptor */
    struct toolchain_descriptor *toolchain;
    /*!\brief Icemake Handle */
    struct icemake *icemake;
};

/*! \brief Effective Operating System Architecture
 *
 *  As reported by uname(), or through the -t flag.
 */
extern const char *uname_arch;

/*! \brief Effective Operating System Vendor Name
 *
 *  As reported by uname(), or through the -t flag.
 */
extern const char *uname_vendor;

/*! \brief Effective Toolchain Code
 *
 *  Derived through the -t flag, or guessed based on the uname data and the
 *  available C compilers.
 */
extern enum toolchain uname_toolchain;

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

/*! \brief Effective Architecture Descriptor
 *
 *  Derived from the effective OS, architecture, vendor and toolchain.
 */
extern const char *archprefix;

/*! \brief Effective Architecture Descriptor (Generated, S-expression)
 *
 *  Derived from the effective OS, architecture, vendor and toolchain.
 */
extern sexpr architecture;

/*! \brief Toolchain Version
 *
 *  Used to find specific toolchain binaries.
 */
extern char *tcversion;

/*! \brief Boolean: Optimise Linking */
extern sexpr i_optimise_linking;

/*! \brief Boolean: Combine C Code Files (for Optimisations) */
extern sexpr i_combine;

/*! \brief Boolean: Create Shared Objects for Libraries */
extern sexpr i_dynamic_libraries;

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
define_symbol (sym_assembly,            "assembly");
/*! \brief Predefined Symbol */
define_symbol (sym_preproc_assembly,    "preprocessed-assembly");
/*! \brief Predefined Symbol */
define_symbol (sym_resource,            "resource");
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
define_symbol (sym_diff,                "diff");
/*! \brief Predefined Symbol */
define_symbol (sym_man,                 "man");
/*! \brief Predefined Symbol */
define_symbol (sym_libc,                "libc");
/*! \brief Predefined Symbol */
define_symbol (sym_libcurie,            "libcurie");
/*! \brief Predefined Symbol */
define_symbol (sym_freestanding,        "freestanding");
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
define_symbol (sym_execute,             "execute");
/*! \brief Predefined Symbol */
define_symbol (sym_targets,             "targets");
/*! \brief Predefined Symbol */
define_symbol (sym_error,               "error");
/*! \brief Predefined Symbol */
define_symbol (sym_warning,             "warning");
/*! \brief Predefined Symbol */
define_symbol (sym_no_shared_library,   "no-shared-library");

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
define_string (str_kernel32,            "kernel32");
/*! \brief Predefined String */
define_string (str_mingw32,             "mingw32");
/*! \brief Predefined String */
define_string (str_coldname,            "coldname");
/*! \brief Predefined String */
define_string (str_mingwex,             "mingwex");
/*! \brief Predefined String */
define_string (str_msvcrt,              "msvcrt");
/*! \brief Predefined String */
define_string (str_dlc,                 "-lc");
/*! \brief Predefined String */
define_string (str_do,                  "-o");
/*! \brief Predefined String */
define_string (str_dc,                  "-c");
/*! \brief Predefined String */
define_string (str_dr,                  "-r");
/*! \brief Predefined String */
define_string (str_ds,                  "-s");
/*! \brief Predefined String */
define_string (str_dw,                  "-w");
/*! \brief Predefined String */
define_string (str_dP,                  "-P");
/*! \brief Predefined String */
define_string (str_dAT,                 "-AT");
/*! \brief Predefined String */
define_string (str_dlGi,                "-lGi");
/*! \brief Predefined String */
define_string (str_plus,                "+");
/*! \brief Predefined String */
define_string (str_dWD,                 "-WD");
/*! \brief Predefined String */
define_string (str_dB,                  "-B");
/*! \brief Predefined String */
define_string (str_dq,                  "-q");
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
define_string (str_snologo,             "/nologo");
/*! \brief Predefined String */
define_string (str_sc,                  "/c");
/*! \brief Predefined String */
define_string (str_sTC,                 "/TC");
/*! \brief Predefined String */
define_string (str_sTP,                 "/TP");
/*! \brief Predefined String */
define_string (str_sEHsc,               "/EHsc");
/*! \brief Predefined String */
define_string (str_slink,               "/link");
/*! \brief Predefined String */
define_string (str_sdll,                "/dll");
/*! \brief Predefined String */
define_string (str_sINCLUDEcumain,      "/INCLUDE:_main");
/*! \brief Predefined String */
define_string (str_sINCLUDEcmain,       "/INCLUDE:main");
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
/*! \brief Predefined String */
define_string (str_version,             "version");
/*! \brief Predefined String */
define_string (str_combined_c_source,   "combined-c-source");
/*! \brief Predefined String */
define_string (str_dfwhole_program,     "-fwhole-program");
/*! \brief Predefined String */
define_string (str_build_slash,         "build/");
/*! \brief Predefined String */
define_string (str_build_backslash,     "build\\");
/*! \brief Predefined String */
define_string (str_slash,               "/");
/*! \brief Predefined String */
define_string (str_backslash,           "\\");
/*! \brief Predefined String */
define_string (str_dot_obj,             ".obj");
/*! \brief Predefined String */
define_string (str_dot_o,               ".o");
/*! \brief Predefined String */
define_string (str_dot_h,               ".h");
/*! \brief Predefined String */
define_string (str_dot_res,             ".res");
/*! \brief Predefined String */
define_string (str_dot_exe,             ".exe");
/*! \brief Predefined String */
define_string (str_dot_pic_o,           ".pic.o");
/*! \brief Predefined String */
define_string (str_dL,                  "-L");
/*! \brief Predefined String */
define_string (str_dI,                  "-I");
/*! \brief Predefined String */
define_string (str_sI,                  "/I");
/*! \brief Predefined String */
define_string (str_dIbuilds,            "-Ibuild/");
/*! \brief Predefined String */
define_string (str_sIbuilds,            "/Ibuild/");
/*! \brief Predefined String */
define_string (str_dl,                  "-l");
/*! \brief Predefined String */
define_string (str_dot_lib,             ".lib");
/*! \brief Predefined String */
define_string (str_sLIBPATHc,           "/LIBPATH:");
/*! \brief Predefined String */
define_string (str_dhighlevel,          "-highlevel");
/*! \brief Predefined String */
define_string (str_dbootstrap,          "-bootstrap");
/*! \brief Predefined String */
define_string (str_dash,                "-");
/*! \brief Predefined String */
define_string (str_test_dash,           "test-");
/*! \brief Predefined String */
define_string (str_sinclude,            "/include");
/*! \brief Predefined String */
define_string (str_sincludes,           "/include/");
/*! \brief Predefined String */
define_string (str_includes,            "include/");
/*! \brief Predefined String */
define_string (str_sversiondh,          "/version.h");
/*! \brief Predefined String */
define_string (str_colon,               ":");
/*! \brief Predefined String */
define_string (str_cl,                  "cl");
/*! \brief Predefined String */
define_string (str_bcc32,               "bcc32");
/*! \brief Predefined String */
define_string (str_slibcuriedsx,        "/libcurie.sx");
/*! \brief Predefined String */
define_string (str_libcuriedsx,         "libcurie.sx");
/*! \brief Predefined String */
define_string (str_susrs,               "/usr/");
/*! \brief Predefined String */
define_string (str_slibs,               "/lib/");
/*! \brief Predefined String */
define_string (str_bslib,               "\\lib");
/*! \brief Predefined String */
define_string (str_bin,                 "bin");
/*! \brief Predefined String */
define_string (str_slib,                "/lib");
/*! \brief Predefined String */
define_string (str_slibslibcuriedsx,    "/lib/libcurie.sx");
/*! \brief Predefined String */
define_string (str_DYLIB_LIBRARY_PATHe, "DYLIB_LIBRARY_PATH=");
/*! \brief Predefined String */
define_string (str_LD_LIBRARY_PATHe,    "LD_LIBRARY_PATH=");
/*! \brief Predefined String */
define_string (str_PATHe,               "PATH=");
/*! \brief Predefined String */
define_string (str_soutc,               "/out:");
/*! \brief Predefined String */
define_string (str_sdefc,               "/def:");
/*! \brief Predefined String */
define_string (str_simplibc,            "/implib:");
/*! \brief Predefined String */
define_string (str_dot_sx,              ".sx");
/*! \brief Predefined String */
define_string (str_dot_a,               ".a");
/*! \brief Predefined String */
define_string (str_dot_dll,             ".dll");
/*! \brief Predefined String */
define_string (str_dot,                 ".");
/*! \brief Predefined String */
define_string (str_dot_so,              ".so");
/*! \brief Predefined String */
define_string (str_dot_so_dot,          ".so.");
/*! \brief Predefined String */
define_string (str_dWlcdsonameclib,     "-Wl,-soname,lib");
/*! \brief Predefined String */
define_string (str_sgenericsdocs,       "/generic/doc/");
/*! \brief Predefined String */
define_string (str_sgenericsmans,       "/generic/man/");
/*! \brief Predefined String */
define_string (str_sgenericsconfigurations, "/generic/configuration/");
/*! \brief Predefined String */
define_string (str_ssharesmans,         "/share/man/");
/*! \brief Predefined String */
define_string (str_ssharesdocs,         "/share/doc/");
/*! \brief Predefined String */
define_string (str_setcs,               "/etc/");
/*! \brief Predefined String */
define_string (str_ddsddsdds,           "../../../");
/*! \brief Predefined String */
define_string (str_dot_pdf,             ".pdf");
/*! \brief Predefined String */
define_string (str_dot_dvi,             ".dvi");
/*! \brief Predefined String */
define_string (str_sFo,                 "/Fo");
/*! \brief Predefined String */
define_string (str_sfo,                 "/fo");
/*! \brief Predefined String */
define_string (str_hdefines,            "#define ");
/*! \brief Predefined String */
define_string (str_sq,                  " \"");
/*! \brief Predefined String */
define_string (str_qnl,                 "\"\n");
/*! \brief Predefined String */
define_string (str_nl,                  "\n");
/*! \brief Predefined String */
define_string (str_uname,               "_name");
/*! \brief Predefined String */
define_string (str_uurl,                "_url");
/*! \brief Predefined String */
define_string (str_uversion,            "_version");
/*! \brief Predefined String */
define_string (str_uversion_s,          "_version_s");
/*! \brief Predefined String */
define_string (str_ubuild_number,       "_build_number");
/*! \brief Predefined String */
define_string (str_ubuild_number_s,     "_build_number_s");
/*! \brief Predefined String */
define_string (str_uversion_long,       "_version_long");
/*! \brief Predefined String */
define_string (str_space,               " ");

/*! \brief Prefix List with Elements from an Environment Variable
 *  \param[in] x   The original list.
 *  \param[in] var The name of the environment variable.
 *  \return x with elements of var prefixed.
 *
 *  This is used (e.g.) when constructing compiler command lines to add the
 *  contents of CFLAGS (and other variables) to the command line. The
 *  environment variable is looked up in the current process environment and
 *  its contents are split by spaces to delimit individual options to the
 *  compiler.
 */
sexpr prepend_flags_from_environment (sexpr x, const char *var);

/*! \brief Prefix with a C String
 *  \param[in] f The C string to add a prefix to.
 *  \param[in] p The prefix to add.
 *  \return The prefixed directory.
 */
sexpr sx_string_dir_prefix_c (const char *f, sexpr p);

/*! \brief Build Targets
 *  \param[in] icemake Icemake handle.
 */
int icemake_build (struct icemake *icemake);

/*! \brief Install Targets
 *  \param[in] icemake Icemake handle.
 */
int icemake_install (struct icemake *);

/*! \brief Run Test Cases
 *  \param[in] icemake Icemake handle.
 */
int icemake_run_tests (struct icemake *);

/*! \brief Link Targets
 *  \param[in] icemake Icemake handle.
 */
int icemake_link (struct icemake *);

/*! \brief Build Documentation
 *  \param[in] icemake Icemake handle.
 */
int icemake_build_documentation (struct icemake *);

/*! \brief Loop over all Processes
 *  \param[in] icemake Icemake handle.
 *
 *  This will go through the workstack and process all of the items. If any of
 *  the programmes fail, icemake terminates.
 */
int icemake_loop_processes ( struct icemake *im );

/*! \brief Path Name Mangling (Borland)
 *  \return b;
 *
 *  BCC and related tools can't handle some characters in path names... in
 *  particular, they can't seem to handle the "+" character too well since it's
 *  used for other things in some of the tools.
 */
char *mangle_path_borland (char *b);

/*! \brief Path Name Mangling (Borland) (S-expressions)
 *  \return b;
 *
 *  BCC and related tools can't handle some characters in path names... in
 *  particular, they can't seem to handle the "+" character too well since it's
 *  used for other things in some of the tools.
 */
sexpr mangle_path_borland_sx (sexpr b);

sexpr lowercase (sexpr s);

sexpr get_build_file   (struct target *t, sexpr file);
sexpr get_install_file (struct target *t, sexpr file);

void mkdir_pi (sexpr path);
void mkdir_p  (sexpr path);

void on_error   (enum icemake_error error, const char *text);
void on_warning (enum icemake_error error, const char *text);

int icemake_prepare_toolchain_generic (struct toolchain_descriptor *td);
int icemake_prepare_toolchain_gcc     (struct toolchain_descriptor *td);
int icemake_prepare_toolchain_borland (struct toolchain_descriptor *td);
int icemake_prepare_toolchain_msvc    (struct toolchain_descriptor *td);
int icemake_prepare_toolchain_latex   (struct toolchain_descriptor *td);
int icemake_prepare_toolchain_doxygen (struct toolchain_descriptor *td);

int icemake_prepare_operating_system_generic (struct toolchain_descriptor *td);

int icemake_prepare_toolchain
    (const char *name,
     int (*with_data)(struct toolchain_descriptor *, void *), void *aux);

int icemake_prepare
    (struct icemake *im, const char *path, struct toolchain_descriptor *td,
     int (*with_data)(struct icemake *, void *), void *aux);

int icemake_default_architecture
    (int (*with_data)(const char *, void *), void *aux);

int icemake
    (struct icemake *im);

/*! \todo remove these functions from the global scope */
sexpr icemake_permutate_paths (struct toolchain_descriptor *td, sexpr p);
sexpr icemake_which (const struct toolchain_descriptor *td, char *programme);

#endif

/*! @} */
