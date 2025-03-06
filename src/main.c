#include <ncurses.h>
#include <stdlib.h>
#include <curses.h>
#include <string.h>
#include <unistd.h>
#include <stdio.h>
#include <locale.h>

//====== constant defs ======
#define ACTION_NULL 0
#define ACTION_QUIT 1
#define ACTION_DIG 2
#define ACTION_FLAG 3

//====== macros ======
#define CLAMP_MAX(val,max) ((val) < max) ? (val) : max
#define CLAMP_MIN(val,min) ((val) > min) ? (val) : min
#define CLAMP(val,min,max) CLAMP_MAX(CLAMP_MIN(val,min),max)

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
int get_action_position(struct board_struct *board,WINDOW *board_window,int *action_x,int *action_y);
void place_flag(struct board_struct *board,int x,int y);

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

	//====== initialise ncurses ======
	setlocale(LC_ALL, "");
	initscr();
	noecho();
	cbreak();
	//intrflush(stdscr,TRUE);
	keypad(stdscr,TRUE);
	//board display window
	int centre_x = COLS/2;
	int centre_y = LINES/2;
	WINDOW *board_window = newwin(
	//     +2 for the border width
		height+2,width+2,
		centre_y-(height/2)-1,centre_x-(width/2)-1
	);
	//keypad(board_window,TRUE);
	refresh();
	box(board_window,0,0);
	wrefresh(board_window);

	//====== mainloop ======
	int action_x = 0;
	int action_y = 0;
	int started = 0; //has the player dug their first square
	for (int action = ACTION_NULL; action != ACTION_QUIT;){
		render_board(board,board_window);
		action = get_action_position(board,board_window,&action_x,&action_y);
		if (action == ACTION_QUIT) break;
		if (action == ACTION_DIG){
			if (started == 0){
				generate_mines(board,mine_count,action_x,action_y);
			}
			started = 1;
		}
		if ((action == ACTION_FLAG) && started){ //dont let them flag if they havent started
			place_flag(board,action_x,action_y);
		}
	}

	//====== cleanup ======
	endwin();
	for (int i = 0; i < width; i++){
		free(board->squares[i]);
	}
	free(board->mine_coords);
	free(board->squares);
	free(board);
	printf("exiting. Bye :)\n");
}

void render_board(struct board_struct *board,WINDOW *win){
	int width = board->width;
	int height = board->height;
	for (int x = 0; x < width; x++){
		for (int y = 0; y < height; y++){
			char character = board->squares[x][y];
			if (character == '\0') character = ' ';
			mvwprintw(win,y+1,x+1,"%c",character);
		}
	}
}

void generate_mines(struct board_struct *board,int count,int origin_x, int origin_y){
	//==== random mine placement ====
	int width = board->width;
	int height = board->height;
	board->mine_count = count;
	board->mine_coords = malloc(sizeof(int[2])*count);
	for (int i = 0; i < count; i++){
		int x = random() % width;
		int y = random() % height;
		if (x == origin_x || y == origin_y){
			//dont start the player on a mine
			i--;
			continue;
		}
		board->mine_coords[i][0] = x;
		board->mine_coords[i][1] = y;
		//printf("mine at (%d,%d)\n",x,y);
	}
}

int get_action_position(struct board_struct *board,WINDOW *board_window,int *action_x,int *action_y){
	int width = board->width;
	int height = board->height;
	int current_x = *action_x;
	int current_y = *action_y;
	for (;;){
		//====== render cursor in correct position ======
		wmove(board_window,current_y+1,current_x+1); //offset from window border
		wrefresh(board_window);

		//====== get and process input ======
		int input = getch();
		switch (input){
			case KEY_LEFT:
				current_x--;
				break;
			case KEY_RIGHT:
				current_x++;
				break;
			case KEY_UP:
				current_y--;
				break;
			case KEY_DOWN:
				current_y++;
				break;
			case '\n':
				return ACTION_DIG;
			case 'e':
				return ACTION_FLAG;
			case 'q':
				return ACTION_QUIT;
			default:
				//printw("unknown key [%c]",input);
		}
		//stop the cursor from exiting the window
		current_x = CLAMP(current_x,0,width-1);
		current_y = CLAMP(current_y,0,height-1);
		*action_x = current_x;
		*action_y = current_y;
	}
}
void place_flag(struct board_struct *board,int x,int y){
	char **squares = board->squares;
	if (squares[x][y] == 'P'){ //flag already there
		squares[x][y] = '\0';
	} else if (squares[x][y] == '\0'){ //not dug up and no flag
		squares[x][y] = 'P';
	}
}
