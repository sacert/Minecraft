all: main

run: all
	./main

clean:
	rm *.o

main: main.o shader.o texture.o bitmap.o camera.o skybox.o frustum.o block.o chunk_manager.o chunk.o util.o gui.o libraries/FastNoise.o
	g++ main.o shader.o texture.o bitmap.o camera.o skybox.o frustum.o block.o chunk_manager.o chunk.o util.o gui.o libraries/FastNoise.o -o main -lGLEW -lglfw -framework OpenGL 

main.o: main.cpp
	g++ -std=c++11 -c -o main.o main.cpp

shader.o: shader.cpp shader.h
	g++ -std=c++11 -c -o shader.o shader.cpp

texture.o: texture.cpp texture.h
	g++ -std=c++11 -c -o texture.o texture.cpp

bitmap.o: bitmap.cpp bitmap.h
	g++ -std=c++11 -c -o bitmap.o bitmap.cpp

camera.o: camera.cpp camera.h
	g++ -std=c++11 -c -o camera.o camera.cpp

skybox.o: skybox.cpp skybox.h
	g++ -std=c++11 -c -o skybox.o skybox.cpp

frustum.o: frustum.cpp frustum.h
	g++ -std=c++11 -c -o frustum.o frustum.cpp

block.o: block.cpp block.h
	g++ -c -o block.o block.cpp

chunk.o: chunk.cpp chunk.h
	g++ -std=c++11 -c -o chunk.o chunk.cpp

chunk_manager.o: chunk_manager.cpp chunk_manager.h
	g++ -std=c++11 -c -o chunk_manager.o chunk_manager.cpp

util.o: util.cpp util.h
	g++ -std=c++11 -c -o util.o util.cpp

gui.o: gui.cpp gui.h
	g++ -std=c++11 -c -o gui.o gui.cpp

FastNoise.o: libraries/FastNoise.cpp libraries/FastNoise.h
	g++ -std=c++11 -c -o libraries/FastNoise.o libraries/FastNoise.cpp
	
