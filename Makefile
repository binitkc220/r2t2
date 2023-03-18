CXX     = g++
CXXFLAGS = -Wpedantic -Wall -Wextra -std=c++17 `sdl2-config --cflags` `sdl2-config --libs` -lGL

IMGUI_DIR = external/imgui

SRCS = main.cpp
SRCS += $(IMGUI_DIR)/backends/imgui_impl_sdl2.cpp
SRCS += $(IMGUI_DIR)/backends/imgui_impl_sdlrenderer.cpp
SRCS += $(IMGUI_DIR)/imgui.cpp
SRCS += $(IMGUI_DIR)/imgui_draw.cpp
SRCS += $(IMGUI_DIR)/imgui_widgets.cpp
SRCS += $(IMGUI_DIR)/imgui_tables.cpp
HEADERS=$(shell find * -name "*.h")
INCLUDES = $(IMGUI_DIR) $(IMGUI_DIR)/backends
OBJS = $(SRCS:.cpp=.o)
EXE  = rt

DBGDIR = bin/debug
DBGEXE = $(DBGDIR)/$(EXE)
DBGOBJS = $(addprefix $(DBGDIR)/, $(OBJS))
DBGCXXFLAGS = -g -O0 -DDEBUG

RELDIR = bin/release
RELEXE = $(RELDIR)/$(EXE)
RELOBJS = $(addprefix $(RELDIR)/, $(OBJS))
RELCXXFLAGS = -Ofast -DNDEBUG -flto

.PHONY: all clean debug prep release remake

all: prep release

debug: $(DBGEXE)

$(DBGEXE): $(DBGOBJS)
	$(CXX) $(CXXFLAGS) $(DBGCXXFLAGS) -o $(DBGEXE) $^

$(DBGDIR)/%.o: %.cpp $(HEADERS)
	$(CXX) -c $(CXXFLAGS) $(DBGCXXFLAGS) -I $(INCLUDES) -o $@ $<

release: $(RELEXE)

$(RELEXE): $(RELOBJS)
	$(CXX) $(CXXFLAGS) $(RELCXXFLAGS) -o $(RELEXE) $^

$(RELDIR)/%.o: %.cpp $(HEADERS)
	$(CXX) -c $(CXXFLAGS) $(RELCXXFLAGS) -I $(INCLUDES) -o $@ $<

prep:
	@mkdir -p $(DBGDIR) $(RELDIR)
	@mkdir -p $(DBGDIR)/$(IMGUI_DIR)/backends
	@mkdir -p $(RELDIR)/$(IMGUI_DIR)/backends

remake: clean all

clean:
	rm -f $(RELEXE) $(RELOBJS) $(DBGEXE) $(DBGOBJS)

image:
	$(RELEXE) > ./asset/gen/image.ppm
	convert ./asset/gen/image.ppm ./asset/gen/image.png
