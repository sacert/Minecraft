#INCLUDE=-I /Users/fogleman/Workspace/glfw-2.7.8/include
#LIBRARY=-L /Users/fogleman/Workspace/glfw-2.7.8/lib/cocoa
FLAGS=-std=c99 -O3

all: main

run: all
	./main

clean:
	rm *.o

main: main.o
	gcc $(FLAGS) main.o -o main $(LIBRARY) -lGLEW -lglfw -framework OpenGL 

main.o: main.c
	gcc $(FLAGS) $(INCLUDE) -c -o main.o main.c

#test.o: test.c test.h
#	gcc $(FLAGS) $(INCLUDE) -c -o test.o test.c