CC ?= cc
CXX ?= c++
CPPFLAGS += -Iinclude
LDLIBS += -lm

MODE ?= release
ifeq ($(MODE),debug)
  CFLAGS   ?= -std=c11 -O0 -g -Wall -Wextra -Wpedantic -fsanitize=address,undefined
  CXXFLAGS ?= -std=c++17 -O0 -g -Wall -Wextra -Wpedantic -fsanitize=address,undefined
  LDFLAGS  += -fsanitize=address,undefined
else
  CFLAGS   ?= -std=c11 -O2 -Wall -Wextra -Wpedantic
  CXXFLAGS ?= -std=c++17 -O2 -Wall -Wextra -Wpedantic
endif

WG_FLAGS := -ffp-contract=off

BUILD := build/$(MODE)
BIN   := $(BUILD)/worldgen

C_SRCS := $(wildcard src/*.c src/*/*.c)
CXX_SRCS := $(wildcard src/*.cpp src/*/*.cpp)
OBJS := $(C_SRCS:%.c=$(BUILD)/%.o) $(CXX_SRCS:%.cpp=$(BUILD)/%.o)
DEPS := $(OBJS:.o=.d)

# SDL only touches src/platform/sdl3.c so core stays dependency-free.
CPPFLAGS += $(shell pkg-config --cflags sdl3)
SDL_LIBS := $(shell pkg-config --libs sdl3)

LINK := $(if $(strip $(CXX_SRCS)),$(CXX),$(CC))

CORE_OBJS := $(filter $(BUILD)/src/core/%,$(OBJS))
CHECK_BIN := $(BUILD)/check
DEPS += $(BUILD)/tools/check.d

.PHONY: all run clean debug check

all: $(BIN)

$(BIN): $(OBJS)
	@mkdir -p $(@D)
	$(LINK) $(LDFLAGS) $^ $(SDL_LIBS) $(LDLIBS) -o $@

$(BUILD)/%.o: %.c
	@mkdir -p $(@D)
	$(CC) $(CPPFLAGS) $(CFLAGS) $(WG_FLAGS) -MMD -MP -c $< -o $@

$(BUILD)/%.o: %.cpp
	@mkdir -p $(@D)
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) $(WG_FLAGS) -MMD -MP -c $< -o $@

run: $(BIN)
	./$(BIN) $(ARGS)

debug:
	$(MAKE) MODE=debug

$(CHECK_BIN): $(BUILD)/tools/check.o $(CORE_OBJS)
	@mkdir -p $(@D)
	$(CC) $(LDFLAGS) $^ $(LDLIBS) -o $@

check: $(CHECK_BIN)
	./$(CHECK_BIN) $(ARGS)

clean:
	rm -rf build

-include $(DEPS)
