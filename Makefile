all: main

run: all
	./main

clean:
	rm *.o

main: main.o shader.o texture.o bitmap.o camera.o skybox.o frustum.o block.o chunk_manager.o chunk.o util.o libraries/FastNoise.o
	g++ main.o shader.o texture.o bitmap.o camera.o skybox.o frustum.o block.o chunk_manager.o chunk.o util.o libraries/FastNoise.o -o main -lGLEW -lglfw -framework OpenGL 

main.o: main.cpp
	g++ -c -o main.o main.cpp

shader.o: shader.cpp shader.h
	g++ -c -o shader.o shader.cpp

texture.o: texture.cpp texture.h
	g++ -c -o texture.o texture.cpp

bitmap.o: bitmap.cpp bitmap.h
	g++ -c -o bitmap.o bitmap.cpp

camera.o: camera.cpp camera.h
	g++ -c -o camera.o camera.cpp

skybox.o: skybox.cpp skybox.h
	g++ -c -o skybox.o skybox.cpp

frustum.o: frustum.cpp frustum.h
	g++ -c -o frustum.o frustum.cpp

block.o: block.cpp block.h
	g++ -c -o block.o block.cpp

chunk.o: chunk.cpp chunk.h
	g++ -c -o chunk.o chunk.cpp

chunk.o: chunk_manager.cpp chunk_manager.h
	g++ -c -o chunk_manager.o chunk_manager.cpp

util.o: util.cpp util.h
	g++ -c -o util.o util.cpp

FastNoise.o: libraries/FastNoise.cpp libraries/FastNoise.h
	g++ -c -o libraries/FastNoise.o libraries/FastNoise.cpp
	
