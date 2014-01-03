comma:=,
space:=
space+=

DESTDIR:=
PREFIX:=/usr/local
LIBDIR:=lib

PWD:=$(shell pwd)

root?=$(sort $(addprefix $(PWD)/,$(dir $(wildcard */targets)) .))

SELF:=$(word $(words $(MAKEFILE_LIST)),$(MAKEFILE_LIST))
CPUS:=unknown generic x86-64
OSS:=unknown generic ansi posix linux
VENDORS:=unknown generic pc
TOOLCHAINS:=unknown
UNAME_OS=$(shell uname)
UNAME_VENDOR=unknown
UNAME_MACHINE=$(shell uname -m)
UNAME_TOOLCHAIN=gcc
HOST=$(shell echo $(subst $(space),-,$(strip $(subst _,-,$(UNAME_MACHINE) $(UNAME_VENDOR) $(UNAME_OS) $(UNAME_TOOLCHAIN)))) | tr A-Z a-z)
BUILD=.build
BUILDD=$(BUILD)/.volatile
ECHO=echo
ECHON=echo -n
ECHOE=printf "%s\n"
CAT=cat
SYMLINK=ln -s
LN=ln
CP=cp
RM=rm
MV=mv
SED=sed
CPIO=cpio
INSTALL=install
BUILDBASE=$(BUILD)/$(basedir)
BUILDBASEMAKE=$(BUILDBASE)/makefile
BASEDIRS=src include data lib bin
MINIMALDOC=README AUTHORS COPYING CREDITS
OPTIMISE_FOR_SIZE:=true

SUFFIXES:=$(strip $(EXTRASUFFIXES) sx mk)
SUFFIXES:=$(strip $(SUFFIXES) pic.s pic.S s S c c++ h cpio)

TARGETS:=$(foreach dir,$(root),$(wildcard $(dir)/targets/*))
TARGETBASES=$(notdir $(TARGETS))
TOOLCHAINSPECS:=$(foreach dir,$(root),$(wildcard $(dir)/toolchains/*))

export TOOLCHAINSPECS_VERBATIM

all: host

include $(TOOLCHAINSPECS) $(BUILD)/toolchains.mk

$(BUILDD):
	mkdir $(BUILD); true
	touch $(BUILDD)

$(BUILD)/toolchains.mk: $(BUILDD) $(SELF) $(TOOLCHAINSPECS) $(TARGETS)
	rm -f $@ $(BUILD)/*/makefile
	for c in $(CPUS); do for v in $(VENDORS); do for o in $(OSS); do for t in $(TOOLCHAINS); do \
	$(ECHO) "\$$(BUILD)/$${c}-$${v}-$${o}-$${t}/makefile: \$$(BUILDD) \$$(SELF)\n\t\$$(MAKE) -f \$$(SELF) cpu=$${c} vendor=$${v} os=$${o} toolchain=$${t} basedir=$${c}-$${v}-$${o}-$${t} root='$(root)' create-build-directory" >> "$@"; \
	done; done; done; done

create-build-directory: $(BUILDDIR)
	rm -rf $(BUILDBASE)
	mkdir $(BUILDBASE); true
	for d in $(BASEDIRS); do \
		mkdir "$(BUILDBASE)/$${d}"; \
		for o in $(os) $(COMPATIBLE_OS) .; do \
			for c in $(cpu) .; do \
				for t in $(toolchain) $(COMPATIBLE_TOOLCHAIN) .; do \
					for v in $(vendor) .; do \
						for s in $(SUFFIXES); do \
							for b in $(TARGETBASES) .; do \
								for r in $(root); do \
									for f in $${r}/$${d}/$${o}/$${c}/$${t}/$${v}/$${b}/*.$${s}; do \
										[ -e "$${f}" -a ! -e "$(BUILDBASE)/$${d}/$${b}/$$(basename $${f})" ] && \
										([ -d "$(BUILDBASE)/$${d}/$${b}" ] || mkdir -p "$(BUILDBASE)/$${d}/$${b}") && \
										$(LN) "$${f}" "$(BUILDBASE)/$${d}/$${b}"; \
									done; \
								done; \
							done; \
						done; \
					done; \
				done; \
			done; \
		done; \
	done; true
	$(ECHO) "all: $(TARGETBASES)" > $(BUILDBASEMAKE)
	$(ECHO) "install-fhs: $(addsuffix _INSTALL_FHS,$(TARGETBASES))" >> $(BUILDBASEMAKE)
	$(ECHO) "uninstall-fhs: $(addsuffix _UNINSTALL_FHS,$(TARGETBASES))" >> $(BUILDBASEMAKE)
	$(ECHO) ".SECONDEXPANSION:" >> $(BUILDBASEMAKE)
	$(ECHO) ".SECONDARY:" >> $(BUILDBASEMAKE)
	$(ECHO) "comma:=," >> $(BUILDBASEMAKE)
	(for o in $(BUILDBASE)/src/*.mk; do [ -e "$$o" ] && $(CAT) $$o; done || true) >> $(BUILDBASEMAKE)
	$(ECHO) "cpu:=$(cpu)" >> $(BUILDBASEMAKE)
	$(ECHO) "vendor:=$(vendor)" >> $(BUILDBASEMAKE)
	$(ECHO) "os:=$(os)" >> $(BUILDBASEMAKE)
	$(ECHO) "toolchain:=$(toolchain)" >> $(BUILDBASEMAKE)
	$(ECHO) "basedir:=$(basedir)" >> $(BUILDBASEMAKE)
	$(ECHO) "SYMLINK:=$(SYMLINK)" >> $(BUILDBASEMAKE)
	$(ECHO) "LN:=$(LN)" >> $(BUILDBASEMAKE)
	$(ECHO) "CPIO:=$(CPIO)" >> $(BUILDBASEMAKE)
	$(ECHO) "INSTALL:=$(INSTALL)" >> $(BUILDBASEMAKE)
	$(ECHO) "ECHO:=$(ECHO)" >> $(BUILDBASEMAKE)
	$(ECHO) "DESTDIR:=$(DESTDIR)" >> $(BUILDBASEMAKE)
	$(ECHO) "PREFIX:=$(PREFIX)" >> $(BUILDBASEMAKE)
	$(ECHO) "LIBDIR:=$(LIBDIR)" >> $(BUILDBASEMAKE)
	$(ECHO) "SHAREDIR:=\$$(subst //,/,/usr\$$(subst /usr/,/,\$$(PREFIX)/share))" >> $(BUILDBASEMAKE)
	$(ECHO) "INCLUDEDIR:=\$$(subst //,/,/usr\$$(subst /usr/,/,\$$(PREFIX)/include))" >> $(BUILDBASEMAKE)
	$(ECHO) "DEST:=\$$(DESTDIR)\$$(PREFIX)" >> $(BUILDBASEMAKE)
	$(ECHO) "DESTSHARE:=\$$(DESTDIR)\$$(SHAREDIR)" >> $(BUILDBASEMAKE)
	$(ECHO) "DESTINCLUDE:=\$$(DESTDIR)\$$(INCLUDEDIR)" >> $(BUILDBASEMAKE)
	$(ECHO) "OPTIMISE_FOR_SIZE:=$(OPTIMISE_FOR_SIZE)" >> $(BUILDBASEMAKE)
	([ -n "$(TOOLCHAINSPECS)" ] && $(CAT) $(TOOLCHAINSPECS) || true) >> $(BUILDBASEMAKE)
	$(ECHOE) "$${TOOLCHAINSPECS_VERBATIM}" >> $(BUILDBASEMAKE)
	for i in $(TARGETS); do \
		PIC=YES; \
		BOOTSTRAP=NO; \
		. $${i}; \
		name="$$(basename $${i})"; \
		ename="$${name}"; \
		enamepic=""; \
		if [ "$${TYPE}" = "library" ]; then \
			ename="lib/lib$${name}.a"; \
			enamepic="lib/lib$${name}.so"; \
			$(ECHO) "HAVE_LIBRARIES += $${name}"; \
		elif [ "$${TYPE}" = "programme" ]; then \
			ename="bin/$${name}"; \
		fi; \
		if [ "$${BOOTSTRAP}" = "YES" ]; then \
			LIBRARIES="$${LIBRARIES} curie-bootstrap"; \
			$(ECHO) "$${name}_LDFLAGS:=\$$(BOOTSTRAP_LDFLAGS)"; \
		fi; \
		$(ECHO) "$${name}_VERSION:=$${VERSION}"; \
		$(ECHO) "VERSIONS:=\$$(VERSIONS) $${VERSION}"; \
		$(ECHON) "$${name}_HEADERS:=$${name}/version.h"; \
		for c in $${HEADERS}; do \
			$(ECHON) " $${name}/$${c}.h"; \
		done; \
		$(ECHO); \
		$(ECHO) "$${name}_LIBRARIES:=$${LIBRARIES}"; \
		$(ECHO) "$${name}_FHS_TARGETS:=\$$($${name}_FHS_TARGETS) \$$(addprefix \$$(DESTINCLUDE)/,\$$($${name}_HEADERS))"; \
		if [ -e "$(BUILDBASE)/include/$${name}.mk" ]; then \
			$(ECHO) "$${name}_FHS_TARGETS:=\$$($${name}_FHS_TARGETS) \$$(addprefix \$$(DESTINCLUDE)/,$${name}.mk)"; \
			if [ -n "$(TOOLCHAINSPECS)" ]; then \
				$(CP) "$(BUILDBASE)/include/$${name}.mk" tmpfile; \
				$(RM) "$(BUILDBASE)/include/$${name}.mk"; \
				$(MV) tmpfile "$(BUILDBASE)/include/$${name}.mk"; \
				$(ECHO) >> "$(BUILDBASE)/include/$${name}.mk"; \
				$(ECHO) define TOOLCHAINSPECSDUMP >> "$(BUILDBASE)/include/$${name}.mk"; \
				$(CAT) $(TOOLCHAINSPECS) >> "$(BUILDBASE)/include/$${name}.mk"; \
				(for o in $(BUILDBASE)/src/*.mk; do [ -e "$$o" ] && $(CAT) $$o; done || true) >> "$(BUILDBASE)/include/$${name}.mk"; \
				$(ECHO) endef >> "$(BUILDBASE)/include/$${name}.mk"; \
				$(ECHO) "TOOLCHAINSPECS_VERBATIM:=\$$(value TOOLCHAINSPECSDUMP)" >> "$(BUILDBASE)/include/$${name}.mk"; \
				$(CAT) $(TOOLCHAINSPECS) >> "$(BUILDBASE)/include/$${name}.mk"; \
				(for o in $(BUILDBASE)/src/*.mk; do [ -e "$$o" ] && $(CAT) $$o; done || true) >> "$(BUILDBASE)/include/$${name}.mk"; \
			fi; \
		fi; \
		if [ -n "$${CODE}" ]; then \
			$(ECHON) "$${name}_OBJECTS="; \
			for c in $${CODE}; do \
				$(ECHON) " $${c}.o"; \
			done; \
			if [ "$${TYPE}" = "programme" ]; then \
				for l in $${LIBRARIES}; do \
					$(ECHON) " \$$($${l}_OBJECTS)"; \
				done; \
			elif [ "$${TYPE}" = "library" -a "$${PIC}" = "YES" ]; then \
				$(ECHO); \
				$(ECHO) "LIBRARY_OBJECTS+=\$$($${name}_OBJECTS)"; \
				$(ECHON) "$${name}_OBJECTS_PIC="; \
				for c in $${CODE}; do \
					$(ECHON) " $${c}.pic.o"; \
				done; \
			elif [ "$${TYPE}" = "library" ]; then \
				$(ECHO); \
				$(ECHON) "LIBRARY_OBJECTS+=\$$($${name}_OBJECTS)"; \
			fi; \
			$(ECHO); \
			for c in $${CODE}; do \
				$(ECHO) " $${c}.o: include/$${name}/version.h include/$${name}/archive.h"; \
			done; \
			if [ "$${name}" != "$${ename}" ]; then \
				$(ECHO) "$${name}_BUILD_TARGETS:=\$$($${name}_BUILD_TARGETS) $${ename}"; \
				$(ECHO) "$${name}_FHS_TARGETS:=\$$($${name}_FHS_TARGETS) \$$(DEST)/$${ename}"; \
				if [ "$${enamepic}" != "" -a "$${PIC}" = "YES" ]; then \
					$(ECHO) "$${name}_BUILD_TARGETS:=\$$($${name}_BUILD_TARGETS) $${enamepic}"; \
					$(ECHO) "$${name}_FHS_TARGETS:=\$$($${name}_FHS_TARGETS) \$$(DEST)/$${enamepic}"; \
				fi; \
			else \
				$(ECHO) "$${name}_BUILD_TARGETS:=\$$($${name}_BUILD_TARGETS) $${name}"; \
				$(ECHO) "$${name}_FHS_TARGETS:=\$$($${name}_FHS_TARGETS) \$$(DEST)/$${name}"; \
			fi; \
			$(ECHO) "$${name}_BUILD: \$$($${name}_BUILD_TARGETS)"; \
			$(ECHO) "$${name}_TARGETS:=\$$($${name}_TARGETS) $${name}_BUILD"; \
		fi; \
		$(ECHO) "$${name}_TARGETS:=\$$($${name}_TARGETS) $${name}-\$$(basedir).cpio"; \
		$(ECHO) "$${name}: \$$($${name}_TARGETS)"; \
		$(ECHO) "$${name}_INSTALL_FHS: \$$($${name}_FHS_TARGETS)"; \
		$(ECHO) "$${name}_UNINSTALL_FHS: \$$(addprefix remove-,\$$($${name}_FHS_TARGETS))"; \
	done >> $(BUILDBASEMAKE)

host: build-$(HOST)
install: install-$(HOST)
uninstall: uninstall-$(HOST)
test: test-$(HOST)

build-%: $(BUILD)/%/makefile
	cd $(BUILD)/$* && $(MAKE) all

install-%: $(BUILD)/%/makefile
	cd $(BUILD)/$* && $(MAKE) install

uninstall-%: $(BUILD)/%/makefile
	cd $(BUILD)/$* && $(MAKE) uninstall

test-%: $(BUILD)/%/makefile
	cd $(BUILD)/$* && $(MAKE) autotest
