hashtable_dir = ../hashtable/
holdall_dir = ../holdall/
options_dir = ../options/
readfiles_dir = ../readfiles/
CC = gcc
CFLAGS = -std=c18 \
  -Wall -Wconversion -Werror -Wextra -Wfatal-errors -Wpedantic -Wwrite-strings \
  -O2 -g\
  -I$(hashtable_dir) -I$(holdall_dir) -DHOLDALL_INSERT_TAIL -I$(options_dir) \
  -I$(readfiles_dir)
LDFLAGS =
vpath %.c $(hashtable_dir):$(holdall_dir):$(options_dir):$(readfiles_dir)
vpath %.h $(hashtable_dir):$(holdall_dir):$(options_dir):$(readfiles_dir)
objects = hashtable.o holdall.o options.o readfiles.o main.o
executable = ws

all: $(executable)

clean:
	$(RM) $(objects) $(executable)

$(executable): $(objects)
	$(CC) $(objects) $(LDFLAGS) -o $(executable)

hashtable.o: hashtable.c hashtable.h
holdall.o: holdall.c holdall.h
options.o: options.c options.h
readfiles.o: readfiles.c readfiles.h
main.o: main.c hashtable.h holdall.h options.h readfiles.h
