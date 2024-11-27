all:
	gcc -Wall -Wextra -o kerr main.c args.c tpool.c tga.c -lpthread
clean:
	rm donut