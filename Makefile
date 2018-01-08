all: main

run: all
	./main

clean:
	rm *.o

main: main.o shader.o
	g++ main.o shader.o -o main -lGLEW -lglfw -framework OpenGL 

main.o: main.cpp
	g++ -c -o main.o main.cpp

shader.o: shader.cpp shader.hpp
	g++ -c -o shader.o shader.cpp
