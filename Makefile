# ngrtk 2019 netdex
# This is probably one of the worst Makefiles I've ever written.

# Top-level organization directories
SDIR := src
ODIR := bin
RDIR := res

# List of default compiler flags
DEPS := -I$(SDIR) -Ideps/
CXX := $(CROSS_COMPILE)g++
CXXFLAGS := -Wall -MMD -std=gnu++14 $(DEPS)

# ngrtk target configuration
BIN.ngrtk := ngrtk.exe
RESOURCES.ngrtk := CWSDPMI.EXE
SOURCES.ngrtk := nagareteku.cc \
				 io/vga_dos.cc

# framepack target configuration
BIN.framepack := framepack
SOURCES.framepack := framepack.cc \
                     util/stb_image.cc

############################## HERE BE DRAGONS ##############################

# Put the all target first
all:;

# Macro for defining a target using a subset of all sources
define define-target
# Enumerate all required files
OBJECTS.$(1) := $$(addprefix $(ODIR)/$(1)/,$$(patsubst %.cc,%.o,$(SOURCES.$(1))))
DEPENDS.$(1) := $$(OBJECTS.$(1):.o=.d)

# Enumerate build artifacts for make clean
ARTIFACTS += $$(OBJECTS.$(1)) $$(DEPENDS.$(1)) $(ODIR)/$(1)/$(BIN.$(1))

# Add this target to the all target
all: $(1)

# Create friendly name target for binary
$(1): $(ODIR)/$(1)/$(BIN.$(1))
.PHONY:  $(1)

# Compile rules for target
$(ODIR)/$(1)/$(BIN.$(1)): $$(OBJECTS.$(1))
	$$(CXX) $$(OBJECTS.$(1)) -o $$@

# Compile objects into build directory with depends
$$(OBJECTS.$(1)): $(ODIR)/$(1)/%.o: $(SDIR)/%.cc
	@mkdir -p $$(dir $$@)
	$$(CXX) $$(CXXFLAGS) -c $$< -o $$@

-include $$(DEPENDS.$(1))
endef

$(eval $(call define-target,ngrtk))
$(eval $(call define-target,framepack))

# Target specific overrides
ngrtk: CXX=$(CROSS_COMPILE)g++
framepack: CXX=g++

# Copy additional resources for ngrtk target
TARGET_RESOURCES.ngrtk := $(addprefix $(ODIR)/ngrtk/,$(RESOURCES.ngrtk))
ngrtk: $(TARGET_RESOURCES)
$(TARGET_RESOURCES): $(ODIR)/ngrtk/%: $(RDIR)/%
	cp $< $@

# General targets
clean:
	rm -rf $(ARTIFACTS)

run: ngrtk
	dosbox $(ODIR)/ngrtk/$(BIN.ngrtk)

# Generate compilation database with bear (for my IDE)
compdb:
	make clean
	bear make all

# Regenerate compilation database then run ngrtk target
ide: compdb run

.PHONY: clean all run compdb ide
