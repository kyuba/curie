space:=
space+=

PWD=$(shell pwd)

root:=$(PWD)

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
CAT=cat
SYMLINK=ln -s
LN=ln
BUILDBASE=$(BUILD)/$(basedir)
BUILDBASEMAKE=$(BUILDBASE)/makefile
BASEDIRS=src include data lib bin
MINIMALDOC=README AUTHORS COPYING CREDITS

SUFFIXES:=sx mk

SUFFIXES:=$(strip $(SUFFIXES) pic.s pic.S s S c c++ h)

TARGETS:=$(foreach dir,$(root),$(wildcard $(dir)/targets/*))
TARGETBASES=$(notdir $(TARGETS))
TOOLCHAINSPECS:=$(foreach dir,$(root),$(wildcard $(dir)/toolchains/*))

all: host

include $(TOOLCHAINSPECS) $(BUILD)/toolchains.mk

$(BUILDD):
	mkdir $(BUILD); true
	touch $(BUILDD)

$(BUILD)/toolchains.mk: $(BUILDD) $(SELF) $(TOOLCHAINSPECS) $(TARGETS)
	rm -f $@ $(BUILD)/*/makefile
	for c in $(CPUS); do for v in $(VENDORS); do for o in $(OSS); do for t in $(TOOLCHAINS); do \
	$(ECHO) "\$$(BUILD)/$${c}-$${v}-$${o}-$${t}/makefile: \$$(BUILDD) \$$(SELF)\n\t\$$(MAKE) -f \$$(SELF) cpu=$${c} vendor=$${v} os=$${o} toolchain=$${t} basedir=$${c}-$${v}-$${o}-$${t} create-build-directory" >> "$@"; \
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
	rm -f $(BUILDBASEMAKE)
	$(ECHO) "all: $(TARGETBASES)" >> $(BUILDBASEMAKE)
	$(ECHO) ".SECONDEXPANSION:" >> $(BUILDBASEMAKE)
	$(ECHO) ".INTERMEDIATE:" >> $(BUILDBASEMAKE)
	$(ECHO) "include src/options.mk" >> $(BUILDBASEMAKE)
	$(ECHO) "cpu:=$(cpu)" >> $(BUILDBASEMAKE)
	$(ECHO) "vendor:=$(vendor)" >> $(BUILDBASEMAKE)
	$(ECHO) "os:=$(os)" >> $(BUILDBASEMAKE)
	$(ECHO) "toolchain:=$(toolchain)" >> $(BUILDBASEMAKE)
	$(ECHO) "basedir:=$(basedir)" >> $(BUILDBASEMAKE)
	$(ECHO) "SYMLINK:=$(SYMLINK)" >> $(BUILDBASEMAKE)
	$(ECHO) "LN:=$(LN)" >> $(BUILDBASEMAKE)
	$(ECHO) "ECHO:=$(ECHO)" >> $(BUILDBASEMAKE)
#	$(CAT) "toolchains/$(toolchain)" >> $(BUILDBASEMAKE)
	$(CAT) $(TOOLCHAINSPECS) >> $(BUILDBASEMAKE)
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
		elif [ "$${TYPE}" = "programme" ]; then \
			ename="bin/$${name}"; \
		fi; \
		if [ "$${BOOTSTRAP}" = "YES" ]; then \
			LIBRARIES="$${LIBRARIES} curie-bootstrap"; \
			$(ECHO) "$${name}_LDFLAGS:=\$$(BOOTSTRAP_LDFLAGS)"; \
		fi; \
		$(ECHO) "$${name}_VERSION:=$${VERSION}"; \
		$(ECHO) "VERSIONS:=\$$(VERSIONS) $${VERSION}"; \
		if [ -n "$${CODE}" ]; then \
			$(ECHO) -n "$${name}_OBJECTS:="; \
			for c in $${CODE}; do \
				$(ECHO) -n " $${c}.o"; \
			done; \
			if [ "$${TYPE}" = "programme" ]; then \
				for l in $${LIBRARIES}; do \
					$(ECHO) -n " \$$($${l}_OBJECTS)"; \
				done; \
			elif [ "$${TYPE}" = "library" -a "$${PIC}" = "YES" ]; then \
				$(ECHO); \
				$(ECHO) -n "$${name}_OBJECTS_PIC:="; \
				for c in $${CODE}; do \
					$(ECHO) -n " $${c}.pic.o"; \
				done; \
			fi; \
			$(ECHO);\
			if [ "$${name}" != "$${ename}" ]; then \
				$(ECHO) "$${name}_BUILD_TARGETS:=\$$($${name}_BUILD_TARGETS) $${ename}"; \
				if [ "$${enamepic}" != "" -a "$${PIC}" = "YES" ]; then \
					$(ECHO) "$${name}_BUILD_TARGETS:=\$$($${name}_BUILD_TARGETS) $${enamepic}"; \
				fi; \
			else \
				$(ECHO) "$${name}_BUILD_TARGETS:=\$$($${name}_BUILD_TARGETS) $${name}"; \
			fi; \
			$(ECHO) "$${name}_BUILD: \$$($${name}_BUILD_TARGETS)"; \
			$(ECHO) "$${name}_TARGETS:=\$$($${name}_TARGETS) $${name}_BUILD"; \
		fi; \
		$(ECHO) "$${name}: \$$($${name}_TARGETS)"; \
	done >> $(BUILDBASEMAKE)

host: $(BUILD)/$(HOST)/makefile
	cd $(BUILD)/$(HOST) && $(MAKE)

