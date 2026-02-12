CC=g++
SOURCES=$(wildcard *.c)
FILES := $(subst .c,,$(SOURCES))
FLAGS=-g -Wno-unused-variable -Wno-unused-but-set-variable

# By default, make runs the first target in the file
all: $(FILES)

% :: %.c
	$(CC) $(FLAGS) $< -o $@ -lreadline

clean:
	rm -rf $(FILES)
