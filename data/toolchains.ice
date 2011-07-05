;;
; vim:ft=lisp
;;

; The (patterns ...) specify a set of regular expressions which are matched
; against the given toolchain descriptor to gather metadata on it.

(patterns
; default to gcc on BSDs, Darwin and Linux
  (".*"
    (tc generic "generic")
    (os generic "generic")
    (is generic "generic")
    (vendor "generic"))

; default to gcc on BSDs, Darwin and Linux
  (".*-([Dd]arwin|[Ll]inux|.*[Bb][Ss][Dd]).*"
    (tc gcc "gnu"))

; toolchains
  (".*-(gcc|gnu)"
    (tc gcc "gnu"))
  (".*-borland"
    (tc borland "borland"))
  (".*-msvc"
    (tc msvc "msvc")
    (os windows "windows"))
  (".*-latex"
    (tc latex "latex"))
  (".*-doxygen"
    (tc doxygen "doxygen"))

; operating systems
  (".*-[Dd]arwin.*"
    (os darwin "darwin"))
  (".*-[Ll]inux.*"
    (os linux "linux"))
  (".*-[Ww]indows.*"
    (os windows "windows")
    (vendor "microsoft"))
  (".*-[Ff]ree[Bb][Ss][Dd].*"
    (os freebsd "freebsd"))
  (".*-[Nn]et[Bb][Ss][Dd].*"
    (os netbsd "netbsd"))
  (".*-[Oo]pen[Bb][Ss][Dd].*"
    (os openbsd "openbsd"))
  (".*-[Dd]ragonfly[Bb][Ss][Dd].*"
    (os dragonflybsd "dragonflybsd"))

; CPU architectures
  ("(x86[-/_]64|amd64)-.*"
    (is x86 "x86-64")
    is-64-bit)
  ("(x86[-/_]32|i[3-6]86)-.*"
    (is x86 "x86-32")
    is-32-bit)
  ("([Pp]ower(pc| [Mm]acintosh)|ppc(-?32)?)-.*"
    (is powerpc "ppc-32")
    is-32-bit)
  ("(p(ower)?pc-?64)-.*"
    (is powerpc "ppc-64")
    is-64-bit)
  ("(arm-?(e[bl]|32|v..?)?)-.*"
    (is arm "arm-32")
    is-32-bit)

; specific hardware configurations
  ("psp"
    (tc gcc "gnu")
    (os unknown "psp")
    (is mips "mips-32")
    (vendor "sony")
    is-32-bit))

; (filesystem-layout ...) defines file system locations for build and
; installation files.

(filesystem-layout
  (default fhs)
  (build
    (Object          (base / "build" / uname / target / name))
    (Object-PIC      (base / "build" / uname / target / name))
    (Programme       (base / "build" / uname / target / name))
    (Static-Library  (base / "build" / uname / target / name))
    (Shared-Object   (base / "build" / uname / target / name))
    (C               (base / "build" / uname / target / name))
    (C-Header        (base / "build" / uname / "include" / name)))
  (source
    (Assembly        (base / "src" / name))
    (Preprocessed-Assembly
                     (base / "src" / name))
    (C               (base / "src" / name))
    (C++             (base / "src" / name))
    (Assembly-PIC    (base / "src" / name))
    (Preprocessed-Assembly-PIC
                     (base / "src" / name))
    (C-PIC           (base / "src" / name))
    (C++-PIC         (base / "src" / name))
    (C-Header        (base / "include" / name))
    (Doxyfile        (base / name))
    (LaTeX           (base / "documentation" / name))
    (Man-1           (base / "documentation" / name))
    (Man-2           (base / "documentation" / name))
    (Man-3           (base / "documentation" / name))
    (Man-4           (base / "documentation" / name))
    (Man-5           (base / "documentation" / name))
    (Man-6           (base / "documentation" / name))
    (Man-7           (base / "documentation" / name))
    (Man-8           (base / "documentation" / name))
    (Man-9           (base / "documentation" / name)))
  (fhs
    (Programme       (root / "bin" / name))
    (Static-Library  (root / "lib" / name))
    (Shared-Object   (root / "lib" / name))
    (C-Header        (root / "include" / name)))
  (fhs-32
    (Programme       (root / "bin" / name))
    (Static-Library  (root / "lib-32" / name))
    (Shared-Object   (root / "lib-32" / name))
    (C-Header        (root / "include" / name)))
  (fhs-64
    (Programme       (root / "bin" / name))
    (Static-Library  (root / "lib-64" / name))
    (Shared-Object   (root / "lib-64" / name))
    (C-Header        (root / "include" / name)))
  (afsl
    (Programme       (root / os / architecture / vendor / "bin" / name))
    (Static-Library  (root / os / architecture / vendor / "lib" / name))
    (Shared-Object   (root / os / architecture / vendor / "lib" / name))
    (C-Header        (root / os / architecture / vendor / "include" / target /
                      name)))
  (gcc
    (Object          (name ".o"))
    (Assembly        (name ".s"))
    (Preprocessed-Assembly
                     (name ".S"))
    (C               (name ".c"))
    (C++             (name ".c++") (name ".cpp"))
    (Object-PIC      (name ".pic.o"))
    (Assembly-PIC    (name ".pic.s") (name ".s"))
    (Preprocessed-Assembly-PIC
                     (name ".pic.S") (name ".S"))
    (C-PIC           (name ".c"))
    (C++-PIC         (name ".c++") (name ".cpp"))
    (C-Header        (target / name ".h"))
    (Programme       (name "." version) (name))
    (Shared-Object   ("lib" name ".so." version) ("lib" name ".so"))
    (Static-Library  ("lib" name ".a." version) ("lib" name ".a"))))

; The (specifications ...) are used to create the commands to run in order to
; build whatever is specified in icemake.sx.

(specifications
  (gcc
    (bind
      (gcc "CC" gcc cc)
      (ld  "LD" ld)
      (g++ "GXX" g++)
      (as  "AS" as)
      (ar  "AR" ar)
      (split
        (cflags   "CFLAGS")
        (cxxflags "CXXFLAGS")
        (ldflags  "LDFLAGS")))
    (object-types
      Assembly Preprocessed-Assembly C C++
      Assembly-PIC Preprocessed-Assembly-PIC C-PIC C++-PIC
      C-Header)
    (install
      C-Header Programme Shared-Object Static-Library)
    (metadata
      (C-Header build "version"
"/* this file was generated by icemake */
#define " name "_name \"" name "\"
#define " name "_version " version "
#define " name "_version_s \"" version "\"
#define " name "_build_number " build-number "
#define " name "_build_number_s \"" build-number "\"
#define " name "_version_long \"" name "-" version "." build-number "\""))
    (build
      (Object
        mode:use-first
        input:consume
        (Assembly
          (gcc "-c" source "-o" target))
        (Preprocessed-Assembly
          (gcc (fold includes ("-I" entry))
               "-c" source "-o" target))
        (C
          (gcc (fold includes ("-I" entry))
               "--std=c99"
               cflags "-c" source "-o" target))
        (C++
          (g++ (fold includes ("-I" entry))
               "-fno-exceptions"
               cxxflags "-c" source "-o" target)))
      (Object-PIC
        type:library
        mode:use-first
        input:consume
        (Assembly-PIC
          (gcc "-c" source "-o" target))
        (Preprocessed-Assembly-PIC
          (gcc (fold includes ("-I" entry))
               "-fPIC" "-c" source "-o" target))
        (C-PIC
          (gcc (fold includes ("-I" entry))
               "--std=c99"
               "-fPIC" cflags "-c" source "-o" target))
        (C++-PIC
          (g++ (fold includes ("-I" entry))
               "-fno-exceptions"
               "-fPIC" cxxflags "-c" source "-o" target)))
      (Programme
        type:programme
        mode:merge
        input:consume
        target:common
        (Object
          (gcc (fold library-paths ("-L" entry))
               (fold libraries ("-l" entry))
               (? freestanding ("-ffreestanding"
                                "-nostdlib" "-nodefaultlibs" "-nostartfiles"))
               source "-o" target)))
      (Shared-Object
        type:library
        mode:merge
        input:consume
        target:common
        (Object-PIC
          (gcc (fold library-paths ("-L" entry))
               (fold libraries ("-l" entry))
               (? freestanding ("-ffreestanding"
                                "-nostdlib" "-nodefaultlibs" "-nostartfiles"))
               source "-fPIC" "-o" target)))
      (Static-Library
        type:library
        mode:merge
        input:consume
        target:common
        (Object
          (ar "-r" "-s" "-c" target source))))))
