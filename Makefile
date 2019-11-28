# ngrtk 2019 netdex
# This is probably one of the worst Makefiles I've ever written.

# Top-level organization directories
SDIR := src
ODIR := bin
RDIR := res

# List of default compiler flags
DEPS := -I$(SDIR) -Ideps/
CXX := $(CROSS_COMPILE)g++
CXXFLAGS := -Wall -MMD $(DEPS)

# ngrtk target configuration
BIN.ngrtk := ngrtk.exe
RESOURCES.ngrtk := CWSDPMI.EXE \
                   video.lz4

SOURCES.ngrtk := nagareteku.cc \
                 io/vga_dos.cc \
                 io/video_decoder.cc \
                 third_party/lz4/lz4.c

# framepack target configuration
BIN.framepack := framepack
SOURCES.framepack := framepack.cc \
                     gfx/dither.cc \
                     io/video_encoder.cc \
                     third_party/stb/stb_image.cc \
                     third_party/lz4/lz4.c \
                     third_party/lz4/lz4hc.c

############################## HERE BE DRAGONS ##############################

# Put the all target first
all:;

# Macro for defining a target using a subset of all sources
define define-target
# Enumerate all required files
OBJECTS.$(1) := $$(addprefix $(ODIR)/$(1)/,$$(addsuffix .o,$$(basename $(SOURCES.$(1)))))
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
ngrtk: CXX = $(CROSS_COMPILE)g++
ngrtk: CXXFLAGS += -std=gnu++14 -O2
framepack: CXX=g++
framepack: CXXFLAGS += -std=c++17 -O2

# Copy additional resources for ngrtk target
TARGET_RESOURCES.ngrtk := $(addprefix $(ODIR)/ngrtk/,$(RESOURCES.ngrtk))
ngrtk: $(TARGET_RESOURCES.ngrtk)
$(TARGET_RESOURCES.ngrtk): $(ODIR)/ngrtk/%: $(RDIR)/%
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
