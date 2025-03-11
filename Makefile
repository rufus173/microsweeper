#CC=gcc
LDFLAGS=`pkg-config --libs ncursesw`
CFLAGS=`pkg-config --cflags ncursesw` -Wall -g

microsweeper : src/main.o
	$(CC) $^ -o $@ $(LDFLAGS)
install : microsweeper
	./install
