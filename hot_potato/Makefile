TARGETS=player ringmaster potato
CFLAGS=-std=gnu++11 -pedantic -Wall -Werror -ggdb3

all: $(TARGETS)

player: player.o potato.o
	g++ -o $@ $^

player.o: player.cpp potato.h
	g++ $(CFLAGS) -c  $<

ringmaster: ringmaster.o potato.o
	g++ -o $@ $^

ringmaster.o: ringmaster.cpp potato.h
	g++ $(CFLAGS) -c  $<

potato.o: potato.cpp potato.h
	g++ $(CFLAGS) -c  $<

clean:
	rm -f $(TARGETS) *.o *~
