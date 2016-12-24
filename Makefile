NAME = one
PATH_NAME = $(NAME)
BIN_DIR = bin
SRC_DIR = src
SRC_EXT = cpp
OBJ_EXT = o
INCL_EXT = h
INCL_DIR = include
DEBUG = yes
CHI_COMMIT = 9199df8d81edebd755de8f791a23d0fb99728216

CC = g++
CFLAGS = -std=c++11
LDFLAGS =
CHI_INCLUDE = dep/libchi/include
DEV = yes


SRC_FILES = $(shell find $(SRC_DIR) -name *.$(SRC_EXT))
OBJ_FILES = $(patsubst %.$(SRC_EXT),%.$(OBJ_EXT),$(SRC_FILES))
HEAD_FILES = $(shell find $(SRC_DIR) -name *.$(INCL_EXT))
INCL_FILES = $(patsubst $(SRC_DIR)/%.h,$(INCL_DIR)/$(PATH_NAME)/%.h,$(HEAD_FILES))

ifeq ($(DEBUG),yes)
CFLAGS += -g -O0 -Wall -Wextra
LDFLAGS += -rdynamic
endif
CFLAGS += -I $(CHI_INCLUDE)


.PHONY: build dep all clean

build: $(BIN_DIR)/$(NAME)

dep: dep/libchi
	git -C $< checkout dev
	git -C $< pull origin dev
	git -C $< checkout $(CHI_COMMIT)

all: dep build
	

clean:
	rm -r -f $(BIN_DIR)
	rm -f $(OBJ_FILES)
	rm -r -f $(INCL_DIR)

warning:
	make build

test: $(BIN_DIR)/$(NAME)
	gdb --args $(BIN_DIR)/$(NAME) test/example.1

$(BIN_DIR)/$(NAME): $(OBJ_FILES) $(BIN_DIR)
	$(CC) $(CFLAGS) $(LDFLAGS) $(OBJ_FILES) -o $(BIN_DIR)/$(NAME)

$(SRC_DIR)/%.$(OBJ_EXT): $(SRC_DIR)/%.$(SRC_EXT)
	$(CC) $(CFLAGS) -c $< -o $@
	
$(INCL_DIR)/$(PATH_NAME)/%.$(INCL_EXT):	$(SRC_DIR)/%.$(INCL_EXT)
	mkdir -p `dirname $@`
	cp $< $@

$(INCL_DIR):
	mkdir $(INCL_DIR)

$(BIN_DIR):
	mkdir $(BIN_DIR)

dep/libchi:
	mkdir -p $@
	git -C $@ init
	git -C $@ remote add origin https://github.com/AnAspectOf1/libchi.git
