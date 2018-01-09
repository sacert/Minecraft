all: main

run: all
	./main

clean:
	rm *.o

main: main.o shader.o texture.o bitmap.o 
	g++ main.o shader.o texture.o bitmap.o -o main -lGLEW -lglfw -framework OpenGL 

main.o: main.cpp
	g++ -c -o main.o main.cpp

shader.o: shader.cpp shader.hpp
	g++ -c -o shader.o shader.cpp

texture.o: texture.cpp texture.hpp
	g++ -c -o texture.o texture.cpp

bitmap.o: bitmap.cpp bitmap.hpp
	g++ -c -o bitmap.o bitmap.cpp
	
