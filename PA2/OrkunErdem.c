#include <pthread.h>
#include <time.h>
#include <string.h>
#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>
#include <assert.h>

pthread_mutex_t mutex;
bool GameOver = false;
char **map;
int players = 0;
char mover= 'O';
bool Tie = false;
bool home = false; //Home Turn
bool away = false; //Away Turn

struct value{
	int size;
	char sign;
};

//HORIZONTAL CONTROL 
bool horizontalcontrol(int mat_size){
	int hCounter = 0;
	for (int i = 0; i< mat_size; i++){
		char c = map[i][0];
		for(int k = 0; k < mat_size;k++){
			if(map[i][k] == c && c != '-'){
				hCounter++;
			}
		}
		if(hCounter == mat_size){
			return true;
		}
		else{
			hCounter = 0;
		}
	}
	return false;	
}

//VERTICAL CONTROL
bool verticalcontrol(int mat_size){
	int vCounter = 0;
	for(int a = 0; a<mat_size;a++){
		for(int b = 0; b<mat_size;b++){
			char v = map[0][a];
			if(map[b][a] == v && v != '-'){
				vCounter++;
			}
		}
		if(vCounter == mat_size){
			return true;
		}
		else{
			vCounter = 0;
		}
	}
	return false;
}

//LEFT DIAGONAL CONTROL
bool leftDiagonalcontrol(int mat_size){
	int Diagonal = 0; 
	char a = map[0][0];
	for(int m = 0; m <mat_size;m++){
		for(int n=0; n<mat_size;n++){
			if(m == n){
				if(map[m][n] == a && a != '-'){
					Diagonal++;
				}
			}
		}
	}
	if(Diagonal == mat_size){
		return true;
	}
	return false;	
}

//RIGHT DIAGONAL CONTROL
bool rightDiagonalcontrol(int mat_size){
	int Diagonal2 = 0;
	char b = map[0][mat_size-1];
	for(int n = 0; n<mat_size; n++){
		if(map[n][mat_size-n-1] == b && b!= '-'){
			Diagonal2++;
		}
	}
	if(Diagonal2 == mat_size){
		return true;
	}
	return false;
}


int Win(int mat_size){
	if(horizontalcontrol(mat_size) == true || verticalcontrol(mat_size) == true || leftDiagonalcontrol(mat_size) == true || rightDiagonalcontrol(mat_size) == true){
		GameOver = true;
		return 1; //SOMEONE WINS
	}
	int tieCounter = 0;
	for(int i= 0; i < mat_size; i++){
		for(int m = 0; m<mat_size; m++){
			if(map[i][m] == '-'){
				tieCounter++;
			}
		}
	}
	if(tieCounter == 0){
		GameOver = true;
		return 0; //IT IS TIE
	}
		
	return 2; //THERE IS EMPTY CELL
}

void* game(void* prm){
	int size = ((struct value*)prm)->size;
	while(GameOver == false){
		int x_val, y_val;
		bool process= false;
		pthread_mutex_lock(&mutex);
		if(home == true){
			while(process == false){   
				x_val = rand() % size;
				y_val = rand() % size;
				if(map[x_val][y_val] == '-'){
					map[x_val][y_val] = 'X';
					printf("Player x played on: (%d,%d)\n", x_val, y_val);
					process = true;
					home = false; 
                    //change turn
					players = 0;
					mover = 'X';
				}
				else{
					process = false;
				}
			}
		}
		if(away == true){
			while(process == false){   
				x_val = rand() % size;
				y_val = rand() % size;
				if(map[x_val][y_val] == '-'){
					map[x_val][y_val] = 'O';
					printf("Player o played on: (%d,%d)\n", x_val, y_val);
					process = true;
					away = false; 
                    //change turn
					players = 0;
					mover = 'O';
				}
				else{
					process = false;
				}
			}
		}
			
		pthread_mutex_unlock(&mutex);
	}
}

void displaymap(int mat_size){
	for(int i = 0; i< mat_size;i++){
		for(int m = 0; m<mat_size;m++){
			if(map[i][m] == '-'){
				printf("[ ]");
			}
			else{
				printf("[%c]",map[i][m]);
			}
		}
		printf("\n");
	}
    //DISPLAY GAME
}

int main(int argc, char* argv[]){
	
	int mat_size = atoi(argv[1]);
	srand(time(NULL));
	
	struct value x,o;
	x.sign = 'x';
	x.size = mat_size;
	o.sign = 'o';
	o.size = mat_size;
	
	//generate a map by matrix AxA
	map = (char **)malloc(mat_size * sizeof(char *));
	for(int i = 0; i < mat_size; i++){
		map[i] = (char *)malloc(mat_size * sizeof(char));
	}
	
	
	for(int k = 0; k < mat_size; k++){
		for(int m  = 0; m < mat_size; m++){
			map[k][m] = '-';
		}
	}
	
	printf("Board Size: %sx%s\n", argv[1], argv[1]);
	
	// THREADS
	pthread_t thread_x, thread_o;


	if(pthread_create(&thread_x, NULL, game, &x) != 0){
		fprintf(stderr, "Could not create thread.\n");
		exit(1);
	}
	if(pthread_create(&thread_o, NULL, game, &o) !=0){
		fprintf(stderr, "Could not create thread.\n");
		exit(1);
	}
	while(GameOver == false){
		pthread_mutex_lock(&mutex);
		if(players == 0){
			if(Win(mat_size) == 1){ //WIN SITUATION
				Tie = false;
				break;					
			}
			else if(Win(mat_size) == 0){ //TIE
				Tie = true;
				break;
			}
			else if(Win(mat_size) == 2){ //THERE IS FREE CELL
				if(mover == 'X'){
					away= true;
					players = 2;
					
				}
				else if(mover == 'O'){
					home = true;
					players = 1;	
				}
			}
		}
		pthread_mutex_unlock(&mutex);
	}
	pthread_join(thread_x, NULL);
	pthread_join(thread_o, NULL);
	
	printf("Game end\n");
	
	if(Tie == false){
		printf("Winner is %c\n", mover);
	}
	else if(Tie == true){
		printf("It is a tie\n");
	}
	
	displaymap(mat_size);

	return 0;
}