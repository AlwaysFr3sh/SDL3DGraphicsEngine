# taken from https://gist.github.com/MarcosX/4751372 
# No fucking clue how line 16 works, just fucked around and added some 2's, and it worked... sometimes my genius... it's almost frightening!
# Directories
S_DIR=src
B_DIR=bin

# Files
S_FILES=$(S_DIR)/main.cpp

# Output
EXEC=$(B_DIR)/sdl_app

# Build settings
CC=g++ -std=c++11
# SDL options
CC_SDL=-lSDL2 `sdl2-config --cflags --libs`

all: Build

Build:
	$(CC) $(S_FILES) -o $(EXEC) $(CC_SDL)

build_run:Build
	$(EXEC)

clean:
	rm -rf $(B_DIR)/*

.PHONY: all Build clean
