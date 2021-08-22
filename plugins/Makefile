TARGET := lib
srcfiles = $(shell find ./ -maxdepth 1 -name "*.c")
objfiles = $(srcfiles:.c=.o)
dynlibs = $(objfiles:.o=.so)
CC := gcc
CFLAGS = -Wall -fPIC#-pthread
LDFLAGS =-shared#-lpthread

#$(info VAR="$(dynlibs)")

.PHONY: clean

$(TARGET):$(dynlibs)

%.so : %.o
		$(CC) $(LDFLAGS) $< -o $(TARGET)$@

$(objfiles): $(srcfiles)
		$(CC) $(CFLAGS) $^ -c

clean:
		rm -rf *.so *.o
