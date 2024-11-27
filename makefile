all:
	gcc -Wall -Wextra -o kerr main.c args.c tpool.c -lpthread
clean:
	rm donut