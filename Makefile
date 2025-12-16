CXX        := g++
CXXFLAGS   := -std=c++20 -Iinclude `sdl2-config --cflags`
LDFLAGS    := `sdl2-config --libs` -lSDL2_image -lSDL2_ttf

TARGET     := build/bin/app

SRC_DIRS := src tools/imgui
SRCS := $(shell find $(SRC_DIRS) -name '*.cpp')
OBJS := $(SRCS:%=build/obj/%.o)
DEPS := $(OBJS:.o=.d)

.PHONY: all clean dirs

all: dirs $(TARGET)

dirs:
	@mkdir -p build/bin
	@mkdir -p build/obj/src
	@mkdir -p build/obj/tools/imgui

$(TARGET): $(OBJS)
	$(CXX) $(OBJS) -o $@ $(LDFLAGS)

build/obj/%.cpp.o: %.cpp
	@mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) -MMD -MP -c $< -o $@

clean:
	rm -rf build

-include $(DEPS)

