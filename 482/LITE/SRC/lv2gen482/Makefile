CC=gcc
OUTPUT=lv2gen
OBJS=main.o 
CFLAGS=-Wall -I. --std=gnu99
LDFLAGS=-L. -lstdc++

all: $(OUTPUT)

clean:
	rm -f $(OUTPUT) *.o *.exe

$(OUTPUT): $(OBJS)
	$(LINK.c) $(LDFLAGS) -o $@ $^ $(LIBS)
