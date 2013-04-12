#
# Makefile to make the file libclientserver.a,containing
# connection.o and server.o
#
CXX		 = g++
CC		 = g++
CXXFLAGS	 = -ggdb -Wall -W -Werror -pedantic-errors -std=c++0x
CXXFLAGS	+= -Wmissing-braces -Wparentheses
# The following option cannot be used since some of the socket
# macros give warnings on "old-style-cast"
#CXXFLAGS	+= -Wold-style-cast

RES_DIR = bin/
SRC	= $(wildcard src/*.cc)

.PHONY: all clean cleaner

all: src/baseserver src/diskserver src/newsclient
	mkdir $(RES_DIR)
	mv src/baseserver $(RES_DIR)
	mv src/diskserver $(RES_DIR)
	mv src/newsclient $(RES_DIR)
	


# Create the library; ranlib is for Darwin and maybe other systems.
# Doesn't seem to do any damage on other systems.

#libclientserver.a: connection.o server.o article.o newsgroup.o
#	ar rv libclientserver.a \
#	connection.o server.o article.o newsgroup.o
#	ranlib libclientserver.a

src/baseserver: src/baseserver.o src/connection.o src/server.o src/newsgroup.o src/article.o
src/diskserver: src/diskserver.o src/connection.o src/server.o src/newsgroup.o src/article.o

src/newsclient: src/newsclient.o src/connection.o

clean:
	$(RM) *.o

cleaner: clean
	$(RM) libclientserver.a

%.d: %.cc
	@set -e; rm -f $@; \
	 $(CXX) -MM $(CPPFLAGS) $< > $@.$$$$; \
	 sed 's,\($*\)\.o[ :]*,\1.o $@ : ,g' < $@.$$$$ > $@; \
	 rm -f $@.$$$$

include $(SRC:.cc=.d)

