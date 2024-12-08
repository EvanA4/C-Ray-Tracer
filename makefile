all:
	mkdir -p bin
	gcc -Wall -Wextra -o bin/kerr src/main.c src/args.c src/tpool.c src/tga.c src/render.c -lpthread
clean:
	rm donut