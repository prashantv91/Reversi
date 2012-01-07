all: a.out b.out

a.out: main.cpp gen.cpp player.cpp referee.cpp
	g++ main.cpp -O3

b.out: main.cpp gen.cpp player.cpp referee.cpp
	g++ main.cpp -g -o b.out
