# taken from https://gist.github.com/MarcosX/4751372 
# don't ask me how it works lol
# Directories
S_DIR=src
B_DIR=bin
H_DIR=headers

# Files
S_FILES=$(S_DIR)/*

# Headers
H_FILES=$(H_DIR)/*

# Output
EXEC=$(B_DIR)/sdl_app

# Build settings
CC=g++ -std=c++11
# SDL options
CC_SDL=-lSDL2 `sdl2-config --cflags --libs`
# doesn't work on new mac so I have do do it this way instead
#CC_SDL= -lSDL2 -I/usr/local/include -L/usr/local/lib

all: Build

Build:
	#$(CC) $(S_FILES) -o $(EXEC) $(CC_SDL)
	$(CC) $(S_FILES) $(CC_SDL) -o $(EXEC) 

build_run:Build
	$(EXEC)

clean:
	rm -rf $(B_DIR)/*

.PHONY: all Build clean
