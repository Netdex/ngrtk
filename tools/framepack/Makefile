BIN			:= framepack

SOURCES		:= framepack.cc

CXX			:= g++
CXXFLAGS	:= -Wall -MMD -std=c++14

OBJECTS		:= $(SOURCES:.cc=.o)
DEPENDS		:= $(OBJECTS:.o=.d)

.PHONY: clean all run compdb ide

all: $(BIN)

# Main executable and dependencies
$(BIN): $(OBJECTS)
	$(CXX) $(OBJECTS) -o $@

-include $(DEPENDS)

clean:
	rm -rf $(BIN) *.o *.d

run: all
	./$(BIN)

compdb:
	make clean
	bear make

ide: compdb run