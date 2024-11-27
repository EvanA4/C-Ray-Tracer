all:
	gcc -Wall -Wextra -o kerr main.c args.c -lpthread
clean:
	rm donut