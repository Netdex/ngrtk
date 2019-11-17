NAME 		:= ngrtk

SDIR		:= src
ODIR		:= bin
RDIR		:= res

RESOURCES 	:= CWSDPMI.EXE

CXX			:= $(CROSS_COMPILE)g++
CXXFLAGS	:= -Wall -MMD -I$(SDIR) -std=gnu++14

BIN			:= $(ODIR)/$(NAME).exe
SOURCES		:= $(shell find $(SDIR) -name '*.cc')
OBJECTS		:= $(SOURCES:.cc=.o)
DEPENDS		:= $(OBJECTS:.o=.d)

.PHONY: clean all run compdb ide

all: $(BIN) $(ODIR)/$(RESOURCES)

# Main executable and dependencies
$(BIN): $(OBJECTS)
	$(CXX) $(OBJECTS) -o $@

-include $(DEPENDS)

# Non-executable resources
$(ODIR)/$(RESOURCES): $(ODIR)/%: $(RDIR)/%
	cp $< $@

clean:
	find $(SDIR) -type f \( -name '*.d' -o -name '*.o' \) -delete -print
	rm -rf $(ODIR)/*

run: all
	dosbox $(BIN)

compdb:
	make clean
	bear make

ide: compdb run