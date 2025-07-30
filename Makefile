CC := gcc
SDL_ARGS := $(shell sdl2-config --cflags --libs)
CARGS := -Wall -Wextra -Werror=implicit-function-declaration -Iinclude -Llib -lm -lGL $(SDL_ARGS)

SOURCE_FILES := $(wildcard src/*.c)
SOURCE_NAMES := $(notdir $(basename $(SOURCE_FILES)))
OBJ_FILES := $(addprefix obj/, $(addsuffix .o, $(SOURCE_NAMES)))

TARGET := snake

.PHONY: all
all: release
	cp snake bin/snake -f
	cp assets/ bin/ -rf

.PHONY: run
run: all
	(cd bin ; ./$(TARGET))

.PHONY: clean
clean:
	mkdir bin/ -p
	mkdir obj/ -p
	mkdir assets/ -p
	rm bin/* -rf
	rm obj/* -rf
	rm snake -rf

debug: CARGS += -O0 -ggdb
debug: $(TARGET)

release: CARGS += -O3
release: $(TARGET)

$(TARGET): $(OBJ_FILES)
	$(CC) $^ $(CARGS) -o $@

obj/%.o: src/%.c
	$(CC) -c $< $(CARGS) -o $@

