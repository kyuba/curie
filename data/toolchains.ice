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

; The (specifications ...) are used to create the commands to run in order to
; build whatever is specified in icemake.sx.

(specifications
  (gcc
    (bind
      (binaries
        (gcc . "CC")
        (ld  . "LD)
        (g++ . "GXX")
        (as  . "AS")
        (ar  . "AR"))
      (environment
        (split
          (cflags   . "CFLAGS")
          (cxxflags . "CXXFLAGS")
          (ldflags  . "LDFLAGS"))))
    (object-types
      (regular
        (Assembly . ".s")
        (Preprocessed-Assembly . ".S")
        (C . ".c")
        (C++ . ".c++")
        (C++ . ".cpp"))
      (pic
        (Assembly-PIC . ".pic.s")
        (Preprocessed-Assembly-PIC . ".pic.S")
        (Assembly-PIC . ".s")
        (Preprocessed-Assembly-PIC . ".S")
        (C-PIC . ".c")
        (C++-PIC . ".c++")
        (C++-PIC . ".cpp")))
    (build
      (Assembly)
      (Preprocessed-Assembly)
      (C)
      (C++)
      (Assembly-PIC)
      (Preprocessed-Assembly-PIC)
      (C-PIC)
      (C++-PIC))
    (link
      (programme)
      (library))))
