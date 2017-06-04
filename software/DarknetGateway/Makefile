VERSION = 0.0.1
CC      = g++
CFLAGS  = -Wall -g --std=c++0x -DVERSION=\"$(VERSION)\" -DRPI=1
#LDFLAGS = -lm `pkg-config --cflags gtk+-2.0` `pkg-config --libs gtk+-2.0`
LDFLAGS = -lm -lwiringPi

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


$(BINARY): $(BUILDDIR)/$(OBJECTS)
	$(CC) $(CFLAGS) $(LDFLAGS) -I$(HEADERDIR) -I$(SOURCEDIR) $(OBJECTS) -o $(BINARY) 


$(BUILDDIR)/%.o: $(SOURCEDIR)/%.cpp
	$(CC) $(CFLAGS) $(LDFLAGS) -I$(HEADERDIR) -I$(SOURCEDIR) -c $< -o $@

setup:
	$(MKDIR) -p $(BUILDDIR)

install:
	$(INSTALL) -m 755 -o 0 -g 0 -d $(DESTDIR)/usr/local/bin/
	$(INSTALL) -m 755 -o 0 -g 0 $(BINARY) $(DESTDIR)/usr/local/bin/$(BINARY)
	$(INSTALL) -m 755 -o 0 -g 0 -d $(DESTDIR)/usr/local/$(NAME)/ui/
	$(INSTALL) -m 644 -o 0 -g 0 ./ui/*.ui $(DESTDIR)/usr/local/$(NAME)/ui/
#	$(INSTALL) -m 755 -o 0 -g 0 -d $(DESTDIR)/usr/local/$(NAME)/model/
#	$(INSTALL) -m 644 -o 0 -g 0 ./model/*.model $(DESTDIR)/usr/local/$(NAME)/model/

clean:
	$(RM) $(BINARY) $(OBJECTS)

distclean: clean


help:
	@$(ECHO) "Targets:"
	@$(ECHO) "all     - buildcompile what is necessary"
	@$(ECHO) "clean   - cleanup old .o and .bin"
	@$(ECHO) "install - not yet fully supported"
