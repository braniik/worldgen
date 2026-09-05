CC       ?= cc
CXX      ?= c++
CFLAGS   ?= -std=c11 -O2 -Wall -Wextra -Wpedantic
CXXFLAGS ?= -std=c++17 -O2 -Wall -Wextra -Wpedantic
CPPFLAGS += -Iinclude
LDLIBS   +=

BUILD := build
BIN   := $(BUILD)/worldgen

C_SRCS   := $(wildcard src/*.c src/*/*.c)
CXX_SRCS := $(wildcard src/*.cpp src/*/*.cpp)
OBJS     := $(C_SRCS:%.c=$(BUILD)/%.o) $(CXX_SRCS:%.cpp=$(BUILD)/%.o)
DEPS     := $(OBJS:.o=.d)

# SDL only touches src/platform/sdl3.c so core stays dependency-free.
CPPFLAGS += $(shell pkg-config --cflags sdl3)
LDLIBS   += $(shell pkg-config --libs sdl3)

LINK := $(if $(strip $(CXX_SRCS)),$(CXX),$(CC))

.PHONY: all run clean debug

all: $(BIN)

$(BIN): $(OBJS)
	@mkdir -p $(@D)
	$(LINK) $(LDFLAGS) $^ $(LDLIBS) -o $@

$(BUILD)/%.o: %.c
	@mkdir -p $(@D)
	$(CC) $(CPPFLAGS) $(CFLAGS) -MMD -MP -c $< -o $@

$(BUILD)/%.o: %.cpp
	@mkdir -p $(@D)
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -MMD -MP -c $< -o $@

run: $(BIN)
	./$(BIN) $(ARGS)

debug:
	$(MAKE) CFLAGS="-std=c11 -O0 -g -Wall -Wextra -Wpedantic -fsanitize=address,undefined" \
	        LDFLAGS="-fsanitize=address,undefined"

clean:
	rm -rf $(BUILD)

-include $(DEPS)
