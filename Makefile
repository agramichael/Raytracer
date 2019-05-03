FILE=main
GLMDIR=../glm/

########
#   Directories
S_DIR=Source
B_DIR=Build

########
#   Output
EXEC=$(B_DIR)/$(FILE)

# default build settings
CC_OPTS=-c -std=c++11 -pipe -Wall -Wno-switch -ggdb -g3 -O3 -Wextra -pedantic -march=native -mtune=native -mfpmath=sse -msse4.2
LN_OPTS=
CC=g++

########
#       SDL options
SDL_CFLAGS := $(shell sdl2-config --cflags)
GLM_CFLAGS := -I$(GLMDIR)
SDL_LDFLAGS := $(shell sdl2-config --libs)

########
#   This is the default action
all:Build


########
#   Object list
#
OBJ = $(B_DIR)/main.o $(B_DIR)/helpers.o

########
#   Objects
# $(B_DIR)/%.o : $(S_DIR)/%.cpp $(S_DIR)/SDLauxiliary.h $(S_DIR)/TestModelH.h
# 	$(CC) $(CC_OPTS) -o $(B_DIR)/$(FILE).o $(S_DIR)/$(FILE).cpp $(SDL_CFLAGS) $(GLM_CFLAGS)
#$(S_DIR)/SDLauxiliary.h $(S_DIR)/TestModel.h
$(B_DIR)/%.o : $(S_DIR)/%.cpp
	$(CC) $(CC_OPTS) -c $< -o $@ $(SDL_CFLAGS) $(GLM_CFLAGS)

########
#   Main build rule
Build : $(OBJ) Makefile
	$(CC) $(LN_OPTS) -o $(EXEC) $(OBJ) $(SDL_LDFLAGS)


clean:
	rm -f $(B_DIR)/*
