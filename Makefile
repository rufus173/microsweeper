#CC=gcc
LIBS=`pkg-config --libs ncursesw` -fsanitize=address
CFLAGS=`pkg-config --cflags ncursesw` -Wall -g

install : microsweeper
	./install
microsweeper : src/main.o
	$(CC) $^ -o $@ $(LIBS)
