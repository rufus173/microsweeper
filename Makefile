CC=gcc
LIBS=`pkg-config --libs ncurses` -fsanitize=address
CFLAGS=`pkg-config --cflags ncurses` -Wall -g
microsweeper : src/main.o
	$(CC) $^ -o $@ $(LIBS)
