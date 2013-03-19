#
# Makefile for CPP
#

sources = connection.cc server.cc client.cc mane.cc


.PHONY: all clean

RES_DIR = bin/

all: server client

include $(sources:.cc=.d)

DEBUG = false

# Compiler and compiler options:
CXX       = g++
CC 		  = g++
CXXFLAGS  = -pipe -Wall -W -ansi -pedantic-errors -std=c++0x
CXXFLAGS += -Wmissing-braces -Wparentheses -Wold-style-cast

ifeq ($(DEBUG), true)
CXXFLAGS += -ggdb
LDFLAGS += -ggdb
else
CXXFLAGS += -O2
endif

%.d: %.cc
	@set -e; rm -f $@; \
	$(CC) -M $(CXXFLAGS) $< > $@.$$$$; \
	sed 's,\($*\)\.o[ :]*,\1.o $@ : ,g' < $@.$$$$ > $@; \
	rm -f $@.$$$$

# Linking: TODO: just nu kompileras server & client likadant
server: mane.o connection.o server.o
client: mane.o connection.o client.o
