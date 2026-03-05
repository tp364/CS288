#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define N 4
#define NxN (N*N)
#define TRUE 1
#define FALSE 0

struct state {
	int tiles[N][N];
	int f, g, h;
	short zero_row, zero_column;	/* location (row and column) of blank tile 0 */
	struct state *next;
	struct state *parent;			/* used to trace back the solution */
};

int goal_rows[NxN];
int goal_columns[NxN];

/* Swaps two tiles in a state */
void swap(int row1,int column1,int row2,int column2, struct state * pstate){
	int tile = pstate->tiles[row1][column1];
	pstate->tiles[row1][column1]=pstate->tiles[row2][column2];
	pstate->tiles[row2][column2]=tile;
}

/* Move blank tile (0) down by one row */
void move_down(struct state * pstate){
	swap(pstate->zero_row, pstate->zero_column, pstate->zero_row+1, pstate->zero_column, pstate); 
	pstate->zero_row++;
}

/* Move blank tile (0) right by one column */
void move_right(struct state * pstate){
	swap(pstate->zero_row, pstate->zero_column, pstate->zero_row, pstate->zero_column+1, pstate); 
	pstate->zero_column++;
}

/* Move blank tile (0) up by one row */
void move_up(struct state * pstate){
	swap(pstate->zero_row, pstate->zero_column, pstate->zero_row-1, pstate->zero_column, pstate); 
	pstate->zero_row--;
}

/* Move blank tile (0) left by one column */
void move_left(struct state * pstate){
	swap(pstate->zero_row, pstate->zero_column, pstate->zero_row, pstate->zero_column-1, pstate); 
	pstate->zero_column--;
}

/* Prints a state in 4x4 format (format=0) or single line (format=1) */
void print_a_state(struct state *pstate, int format) {
	int i,j;
	for (i=0;i<N;i++) {
		for (j=0;j<N;j++) printf("%2d ",pstate->tiles[i][j]);
		if(format==0)
			printf("\n");
	}
	printf("\n");
}

int main(int argc,char **argv) {
	int i,j,k,index,count;
	struct state *pstate;

	/* Command line argument check */
	if(argc!=2 || sscanf(argv[1], "%d", &count)!=1) {
		printf("Usage: %s #steps\n", argv[0]);
		exit(1);
	}

	pstate=(struct state *) malloc(sizeof(struct state));
	
	/* Set up the goal state configuration (1-15, 0 in bottom right) */
	goal_rows[0]=3;
	goal_columns[0]=3;

	for(index=1; index<NxN; index++){
		j=(index-1)/N;
		k=(index-1)%N;
		goal_rows[index]=j;
		goal_columns[index]=k;
		pstate->tiles[j][k]=index;
	}
	pstate->tiles[N-1][N-1]=0;	/* empty tile=0 */
	pstate->zero_row = N-1;
	pstate->zero_column = N-1;
	pstate->f=0;
	pstate->g=0;
	pstate->h=0;
	pstate->next=NULL;
	
	printf("goal state\n"); 
	print_a_state(pstate, 0);

	/* Randomize the state by making 'count' random, valid moves from the goal state */
	srand(time(NULL));
	i=0;
	do{
		j = rand();
		k = j % 4; // Select a random direction (0: Up, 1: Down, 2: Left, 3: Right)
		
		if( k == 0 && pstate->zero_row>0){
			move_up(pstate);
			i++;
		}
		if( k == 1 && pstate->zero_row<N-1){
			move_down(pstate);
			i++;
		}
		if( k == 2 && pstate->zero_column>0){
			move_left(pstate);
			i++;
		}
		if( k == 3 && pstate->zero_column<N-1){
			move_right(pstate);
			i++;
		}
	}while(i<count);

	/* Print the generated initial state */
	printf("initial state:\n");
	print_a_state(pstate, 0);

	printf("initial state in one line:\n");
	print_a_state(pstate, 1);

	/* Clean up memory */
	free(pstate);

	return 0;
}
