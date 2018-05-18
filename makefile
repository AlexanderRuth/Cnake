make:	main.c
	gcc -Wall -Wextra -Werror -O3 -lcurses -lpthread main.c -o cnake
