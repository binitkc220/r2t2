CXX     = g++
CXXFLAGS = -Wpedantic -Wall -Wextra -std=c++17

SRCS = $(shell find * -name "*.cpp")
HEADERS = $(shell find * -name "*.h")
OBJS = $(SRCS:.cpp=.o)
EXE  = rt

DBGDIR = bin/debug
DBGEXE = $(DBGDIR)/$(EXE)
DBGOBJS = $(addprefix $(DBGDIR)/, $(OBJS))
DBGCXXFLAGS = -g -O0 -DDEBUG

RELDIR = bin/release
RELEXE = $(RELDIR)/$(EXE)
RELOBJS = $(addprefix $(RELDIR)/, $(OBJS))
RELCXXFLAGS = -O3 -DNDEBUG -flto

.PHONY: all clean debug prep release remake

all: prep release

debug: $(DBGEXE)

$(DBGEXE): $(DBGOBJS)
	$(CXX) $(CXXFLAGS) $(DBGCXXFLAGS) -o $(DBGEXE) $^

$(DBGDIR)/%.o: %.cpp $(HEADERS)
	$(CXX) -c $(CXXFLAGS) $(DBGCXXFLAGS) -o $@ $<

release: $(RELEXE)

$(RELEXE): $(RELOBJS)
	$(CXX) $(CXXFLAGS) $(RELCXXFLAGS) -o $(RELEXE) $^

$(RELDIR)/%.o: %.cpp $(HEADERS)
	$(CXX) -c $(CXXFLAGS) $(RELCXXFLAGS) -o $@ $<

prep:
	@mkdir -p $(DBGDIR) $(RELDIR)
	@mkdir -p $(DBGDIR)/geometry
	@mkdir -p $(DBGDIR)/renderer
	@mkdir -p $(RELDIR)/geometry
	@mkdir -p $(RELDIR)/renderer

remake: clean all

clean:
	rm -f $(RELEXE) $(RELOBJS) $(DBGEXE) $(DBGOBJS)

image:
	$(RELEXE) > ./asset/gen/image.ppm
	magick ./asset/gen/image.ppm ./asset/gen/image.png