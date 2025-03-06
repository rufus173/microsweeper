#include <ncurses.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <stdio.h>
#include <locale.h>

//==== structs ====
struct board_struct {
	// [ [column],[column],[column] ]
	char **squares;
	int width;
	int height;
};
void render_board(struct board_struct *board,WINDOW *win);

int main(int argc, char **argv){
	//====== generate board ======
	printf("loading board...\n");
	int width = 20;
	int height = 20;
	struct board_struct *board = malloc(sizeof(struct board_struct));
	memset(board,0,sizeof(struct board_struct));
	board->width = width;
	board->height = height;
	board->squares = malloc(sizeof(char *)*width);
	for (int i = 0; i < width; i++){
		board->squares[i] = malloc(sizeof(char)*height);
		memset(board->squares[i],'0',sizeof(char)*height);
	}

	//====== initialise ncurses ======
	setlocale(LC_ALL, "");
	initscr();
	intrflush(stdscr,TRUE);
	keypad(stdscr,TRUE);

	//====== mainloop ======
	render_board(board,stdscr);
	sleep(2);

	//====== cleanup ======
	endwin();
	for (int i = 0; i < width; i++){
		free(board->squares[i]);
	}
	free(board->squares);
	free(board);
}

void render_board(struct board_struct *board,WINDOW *win){
	int width = board->width;
	int height = board->height;
	for (int x = 0; x < width; x++){
		for (int y = 0; y < height; y++){
			mvwprintw(win,y,x,"%c",board->squares[x][y]);
		}
	}
}
