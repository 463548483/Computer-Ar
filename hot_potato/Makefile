TARGETS=player ringmaster function
CFLAGS=-std=gnu++11 -pedantic -Wall -Werror -ggdb3

all: $(TARGETS)

player: player.o function.o
	g++ -o $@ $^

player.o: player.cpp function.h
	g++ $(CFLAGS) -c  $<

ringmaster: ringmaster.o function.o
	g++ -o $@ $^

ringmaster.o: ringmaster.cpp function.h
	g++ $(CFLAGS) -c  $<

function.o: function.cpp function.h
	g++ $(CFLAGS) -c  $<

clean:
	rm -f $(TARGETS)
