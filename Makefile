launch: main.o block.o level.o
	g++ main.o block.o level.o -o launch

main.o: main.cpp
	g++ -c main.cpp -o main.o -std=c++0x

block.o: block.cpp block.hpp
	g++ -c block.cpp -o block.o -std=c++0x

level.o: level.cpp level.hpp block.hpp
	g++ -c level.cpp -o level.o -std=c++0x

clean:
	rm -rf *.o launch
