VERSION = 0.0.1
CC      = g++
CFLAGS  = -Wall -g --std=c++0x -DVERSION=\"$(VERSION)\" -DRPI=1
#LDFLAGS = -lm `pkg-config --cflags gtk+-2.0` `pkg-config --libs gtk+-2.0`
LDFLAGS = -lm -lcurl -L src/tbb/build/linux_armv7_gcc_cc4.9.2_libc2.19_kernel4.9.28_release -l tbb

BUILDDIR = build
SOURCEDIR = src
HEADERDIR = src

SOURCES = $(wildcard $(SOURCEDIR)/*.cpp)
OBJECTS = $(patsubst $(SOURCEDIR)/%.cpp, $(BUILDDIR)/%.o, $(SOURCES))

NAME = darknet-gateway
BINARY = $(NAME).bin

ECHO = echo
RM = rm -rf
MKDIR = mkdir
INSTALL = install

.PHONY: all clean setup

all: $(BINARY)


$(BINARY): tbb $(BUILDDIR)/$(OBJECTS)
	$(CC) $(CFLAGS) $(LDFLAGS) -I$(HEADERDIR) -I$(SOURCEDIR) $(OBJECTS) /usr/local/lib/libbcm2835.a  -o $(BINARY) 


$(BUILDDIR)/%.o: $(SOURCEDIR)/%.cpp
	$(CC) $(CFLAGS) $(LDFLAGS) -I$(HEADERDIR) -I$(SOURCEDIR) -c $< -o $@

setup:
	$(MKDIR) -p $(BUILDDIR)

clean:
	$(RM) $(BINARY) $(OBJECTS)
	make -C src/tbb clean

distclean: clean

tbb:
	make -C src/tbb

run:
	export LD_LIBRARY_PATH=$(LD_LIBRARY_PATH):src/tbb/build/linux_armv7_gcc_cc4.9.2_libc2.19_kernel4.9.28_release;sudo ./$(BINARY)

help:
	@$(ECHO) "Targets:"
	@$(ECHO) "all     - buildcompile what is necessary"
	@$(ECHO) "clean   - cleanup old .o and .bin"
	@$(ECHO) "install - not yet fully supported"
