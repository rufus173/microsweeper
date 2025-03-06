#include <ncurses.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <stdio.h>
#include <locale.h>

//==== structs ====
struct board_struct {
	// [ [column],[column],[column] ]
	char **squares; //stores revealed board state (calculated after every move)
	//stores the mine locations
	int (*mine_coords)[2]; //x then y
	int mine_count;

	//ohh wow what could these mean
	int width;
	int height;
};
void render_board(struct board_struct *board,WINDOW *win);
void generate_mines(struct board_struct *board,int count,int origin_x, int origin_y);

int main(int argc, char **argv){
	//====== generate board ======
	printf("loading board...\n");
	
	//TODO: defined by command line arguments
	int width = 20;
	int height = 10;
	int mine_count = 5;
	unsigned int random_seed = 99;
	srandom(random_seed);

	struct board_struct *board = malloc(sizeof(struct board_struct));
	memset(board,0,sizeof(struct board_struct));
	board->width = width;
	board->height = height;
	board->squares = malloc(sizeof(char *)*width);
	for (int i = 0; i < width; i++){
		board->squares[i] = malloc(sizeof(char)*height);
		memset(board->squares[i],0,sizeof(char)*height);
	}

	//==== random mine placement ====
	board->mine_count = mine_count;
	board->mine_coords = malloc(sizeof(int[2])*mine_count);
	for (int i = 0; i < mine_count; i++){
		int x = random() % width;
		int y = random() % height;
		board->mine_coords[i][0] = x;
		board->mine_coords[i][1] = y;
		//printf("mine at (%d,%d)\n",x,y);
	}

	//====== initialise ncurses ======
	setlocale(LC_ALL, "");
	initscr();
	intrflush(stdscr,TRUE);
	keypad(stdscr,TRUE);
	//board display window
	int centre_x = COLS/2;
	int centre_y = LINES/2;
	WINDOW *board_window = newwin(
	//     +2 for the border width
		height+2,width+2,
		centre_y-(height/2)-1,centre_x-(width/2)-1
	);
	refresh();
	box(board_window,0,0);
	wrefresh(board_window);

	//====== mainloop ======
	render_board(board,board_window);
	getch();

	//====== cleanup ======
	endwin();
	for (int i = 0; i < width; i++){
		free(board->squares[i]);
	}
	free(board->mine_coords);
	free(board->squares);
	free(board);
}

void render_board(struct board_struct *board,WINDOW *win){
	int width = board->width;
	int height = board->height;
	for (int x = 0; x < width; x++){
		for (int y = 0; y < height; y++){
			mvwprintw(win,y+1,x+1,"%c",board->squares[x][y]);
		}
	}
	wrefresh(win);
}
