#!/bin/bash
CC=i486-mingw32-gcc
CFLAGS="-Wall -Wextra -pedantic -std=c99 -pipe -O2 -fomit-frame-pointer"

FILES=snake.c
EXECUTABLE=snake.exe

LINKSTATIC="-lSDL -lSDL_ttf -lfreetype -lz"
LINKDYNAMIC="-lmingw32 -lSDLmain -lSDL -lwinmm -lgdi32 -ldxguid -mwindows -lm"

$CC -s $FILES -o $EXECUTABLE $CFLAGS -Wl,-Bstatic $LINKSTATIC -lSDL_ttf -Wl,-Bdynamic $LINKDYNAMIC
