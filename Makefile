CC=gcc
CPROTO=cproto
CPROTOFLAGS=-q -e  -s
CFLAGS=-c -Wall -Wextra -pedantic -std=c99 -pipe -O3 -fomit-frame-pointer
#CFLAGS= -c -Wall -Wextra -pedantic -std=c99 -pipe -O2 -g -ggdb
LDFLAGS=-lSDL -lSDL_ttf
SOURCES=snake.c
OBJECTS=$(SOURCES:.c=.o)
PROTOTYPES=$(SOURCES:.c=.p)
EXECUTABLE=snake

all: cleanproto proto compile cleanobj

proto: $(SOURCES) $(PROTOTYPES)

compile: $(SOURCES) $(EXECUTABLE)

$(EXECUTABLE): $(OBJECTS)
	$(CC) $(LDFLAGS) $(OBJECTS) -o $@

.c.o:
	$(CC) $(CFLAGS) $(INCLUDES) $< -o $@

.c.p:
	@$(CPROTO) $(CPROTOFLAGS) $(INCLUDES) $< -o $@ &>/dev/null

clean: cleanobj cleanproto

cleanobj:
	rm -rf $(OBJECTS)

cleanproto:
	rm -rf $(PROTOTYPES)
