all: main

run: all
	./main

clean:
	rm *.o

main: main.o shader.o texture.o bitmap.o camera.o skybox.o frustum.o chunk.o libraries/FastNoise.o
	g++ main.o shader.o texture.o bitmap.o camera.o skybox.o frustum.o chunk.o libraries/FastNoise.o -o main -lGLEW -lglfw -framework OpenGL 

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

chunk.o: chunk.cpp chunk.h
	g++ -c -o chunk.o chunk.cpp

FastNoise.o: libraries/FastNoise.cpp libraries/FastNoise.h
	g++ -c -o libraries/FastNoise.o libraries/FastNoise.cpp
	
