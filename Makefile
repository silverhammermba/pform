launch: main.o entity.o block.o level.o
	g++ main.o entity.o block.o level.o -o launch -lsfml-graphics -lsfml-window -lsfml-system

main.o: main.cpp
	g++ -c main.cpp -o main.o -std=c++0x

entity.o: entity.cpp pform.hpp
	g++ -c entity.cpp -o entity.o -std=c++0x

block.o: block.cpp pform.hpp
	g++ -c block.cpp -o block.o -std=c++0x

level.o: level.cpp pform.hpp
	g++ -c level.cpp -o level.o -std=c++0x

clean:
	rm -rf *.o launch
