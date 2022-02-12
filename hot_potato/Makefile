TARGETS=client ringmaster function
CFLAGS=-std=gnu++11 -pedantic -Wall -Werror -ggdb3

all: $(TARGETS)

client: client.cpp
	g++ -g -o $@ $^

ringmaster: ringmaster.o function.o
	g++ -o $@ $^

ringmaster.o: ringmaster.cpp function.h
	g++ $(CFLAGS) -c  $<

function.o: function.cpp function.h
	g++ $(CFLAGS) -c  $<

clean:
	rm -f $(TARGETS)
