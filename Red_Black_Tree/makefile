# Makefile — Emertxe Red-Black Tree
#
# Targets:
#   make / make all  — build interactive binary ./rbt
#   make test        — build unit test binary ./test_rbt
#   make clean       — remove all build artifacts
#
# To add a new module: add a .o rule below and include it in SHARED_OBJS.

CC     = gcc
CFLAGS = -Wall -Wextra -g

# Modules shared by both binaries (no main() in any of these).
SHARED_OBJS = insertion.o delete.o rotation.o search.o \
              find_min.o find_max.o display.o tree_utils.o

.DEFAULT_GOAL := all

all: $(SHARED_OBJS) main.o
	$(CC) $(CFLAGS) -o rbt $(SHARED_OBJS) main.o

# main.c is excluded from test build — test.c provides its own main().
test: $(SHARED_OBJS) test.o
	$(CC) $(CFLAGS) -o test_rbt $(SHARED_OBJS) test.o

main.o:       main.c       tree.h color.h
	$(CC) $(CFLAGS) -c main.c
test.o:       test.c       tree.h
	$(CC) $(CFLAGS) -c test.c
insertion.o:  insertion.c  tree.h
	$(CC) $(CFLAGS) -c insertion.c
delete.o:     delete.c     tree.h
	$(CC) $(CFLAGS) -c delete.c
rotation.o:   rotation.c   tree.h
	$(CC) $(CFLAGS) -c rotation.c
search.o:     search.c     tree.h
	$(CC) $(CFLAGS) -c search.c
find_min.o:   find_min.c   tree.h
	$(CC) $(CFLAGS) -c find_min.c
find_max.o:   find_max.c   tree.h
	$(CC) $(CFLAGS) -c find_max.c
display.o:    display.c    tree.h color.h
	$(CC) $(CFLAGS) -c display.c
tree_utils.o: tree_utils.c tree.h
	$(CC) $(CFLAGS) -c tree_utils.c

clean:
	rm -f *.o rbt test_rbt

.PHONY: all test clean