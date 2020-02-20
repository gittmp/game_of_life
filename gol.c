#include<stdio.h>
#include<stdlib.h>
#include"gol.h"

#define IN_FILE "input1.txt"
#define OUT_FILE "output1.txt"
#define RULE will_be_alive_torus
#define NO_GENS 5
#define STATS 1

int main(){
    printf("Running GOL on input %s, for %i generations, to output file %s\n", IN_FILE, NO_GENS, OUT_FILE);

    //create universe
    struct universe v;
    struct universe *u= &v;

    //read in input file
    FILE *infile = NULL;
    read_in_file(infile, u);

    //select rule (wba or wbat)
    int (*rule)(struct universe *u, int column, int row) = RULE;

    //evolve universe
    for(int a=0; a<NO_GENS; a++){
        evolve(u, rule);
    }

    //print stats
    if(STATS){
        print_statistics(u);
    }

    //generate output file
    FILE *outfile = NULL;
    write_out_file(outfile, u);

    return 0;
}

//function for reading data from file IN_FILE into universe structure
void read_in_file(FILE *infile, struct universe *u){
    //opening input file
    if((infile = fopen(IN_FILE, "r")) == NULL){
        printf("can't open file\n");
        exit(1);
    }

    //get line 1 of input
    char *array = malloc(513);
    if(array == NULL){
        exit(5);
    }
    fgets(array, 513, infile);

    //calculate no_cols
    int b = 0;
    while(*(array+b) != '\n'){
        b++;
    }
    int no_cols = b+1;

    array = realloc(array, no_cols);
    if(array == NULL){
        exit(5);
    }

    //populating rest of grid with remaining lines from input file
    int no_rows = 1;
    int elements;
    int c = no_cols;
    while(!feof(infile)){
        no_rows++;
        elements = no_rows*no_cols;
        array = realloc(array, elements);
        if(array == NULL){
            exit(5);
        }

        for(int d=0; d<no_cols; d++){
            fscanf(infile, "%c", (array+c));
            c++;
        }
    }

    no_rows -= 1;
    elements = no_cols*no_rows;

    array = realloc(array, elements+1);
    if(array == NULL){
        exit(5);
    }
    *(array+elements-1) = '\n';
    *(array+elements) = '\0';

    fclose(infile);

    //initialising alive_past array
    int alive_cells = 0;
    for(int e=0; e<elements-1; e++){
        if(*(array + e) == '*'){
            alive_cells++;
        }
    }

    int *alive_array = calloc(NO_GENS+1, sizeof(int));
    if(alive_array == NULL){
        exit(5);
    }
    *alive_array = alive_cells;

    //updating universe structure
    u -> grid = array;
    u -> cols = no_cols;
    u -> rows = no_rows;
    u -> elems = elements;
    u -> gen_no = 0;
    u -> alive_past = alive_array;

}

//function for writing output universe to file OUT_FILE
void write_out_file(FILE *outfile, struct universe *u){
    //opening a file to output to
    if((outfile = fopen(OUT_FILE, "w")) == NULL){
        printf("can't generate output file\n");
        exit(1);
    }

    //outputting universe to file
    for(int f=0; f<(u -> elems)-1; f++){
        fprintf(outfile, "%c", *((u -> grid) + f));
    }
    fprintf(outfile, "%c", '\n');
}

//function for checking if a specified cell is alive
int is_alive(struct universe *u, int column, int row){
    //finding element in universe grid
    int position = row * (u -> cols) + column;
    char cell = *((u -> grid) + position);

    if(cell == '*'){
        return 1;
    } else {
        return 0;
    }
}

//function for determining if a cell will be alive in the next generation (cells ouside dead)
int will_be_alive(struct universe *u, int column, int row){
    //construct neighbourhood array
    int neighbourhood[3][3];
    int alive=0;
    int n=0;
    for(int y = row-1; y < row+2; y++){
        int m=0;
        for(int x = column-1; x < column+2; x++){
            if(x < 0 || x > (u -> cols)-2 || y < 0 || y > (u -> rows)-1){
                neighbourhood[n][m] = 0;
            } else {
                neighbourhood[n][m] = is_alive(u, x, y);
                if(neighbourhood[n][m] == 1){
                    alive++;
                }
            }
            m++;
        }
        n++;
    }

    //calculating if cell is alive/dead
    int cell = neighbourhood[1][1];
    alive -= cell;

    //using rules to determine cell's next state
    switch(cell){
        case 0:
            if(alive == 3){
                return 1;
            } else {
                return 0;
            }
        case 1:
            if(alive == 2 || alive == 3){
                return 1;
            } else {
                return 0;
            }
        default:
            printf("ERROR\n");
            return 2;
    }
}

//function for determining if a cell will be alive in the next generation (cells ouside wrap around like torus)
int will_be_alive_torus(struct universe *u,  int column, int row){
    //construct neighbourhood array
    int neighbourhood[3][3];
    int alive=0;
    int n=0;

    for(int y = row-1; y < row+2; y++){
        int m=0;
        for(int x = column-1; x < column+2; x++){
            int temp_x = x;
            int temp_y = y;
            if(y < 0){
                y = (u -> rows)-1;
            } else if(y > (u -> rows)-1){
                y = 0;
            }
            if(x < 0){
                x = (u -> cols)-2;
            } else if(x > (u -> cols)-2){
                x = 0;
            }

            neighbourhood[n][m] = is_alive(u, x, y);
            if(neighbourhood[n][m] == 1){
                alive++;
            }

            x = temp_x;
            y = temp_y;
            m++;
        }
        n++;
    }

    //calculating if cell is alive/dead
    int cell = neighbourhood[1][1];
    alive -= cell;

    //using rules to determine cell's next state
    switch(cell){
        case 0:
            if(alive == 3){
                return 1;
            } else {
                return 0;
            }
        case 1:
            if(alive == 2 || alive == 3){
                return 1;
            } else {
                return 0;
            }
        default:
            printf("ERROR\n");
            return 2;
    }
}

//function for evolving current grid of cells into next generation using rule (wba or wbat)
void evolve(struct universe *u, int (*rule)(struct universe *u, int column, int row)){
    //get/set up initial data
    int no_cols = u -> cols;
    int no_rows = u -> rows;
    int no_elems = no_cols * no_rows;
    char *new_grid = calloc(no_elems+1, sizeof(char));
    if(new_grid == NULL){
        exit(5);
    }

    //create new evolved array
    int g = 0;
    int alives = 0;
    for(int h=0; h<no_rows; h++){
        for(int k=0; k < no_cols-1; k++){
            int element = rule(u, k, h);
            if(element == 1){
                *(new_grid + g) = '*';
                alives++;
            } else {
                *(new_grid + g) = '.';
            }
            g++;
        }
        *(new_grid + g) = '\n';
        g++;
    }
    *(new_grid + g - 1) = '\0';

    //update universe
    u -> gen_no += 1;
    u -> alive_past[u -> gen_no] = alives;
    u -> grid = new_grid;
}

//printing stats about no. alive cells currently and on average
void print_statistics(struct universe *u){
    //get data
    int total_cells = (u -> elems) - (u -> rows);
    int currently_alive = u -> alive_past[u -> gen_no];
    int total_alive = 0;

    //calculate current percentage of alive cells
    double current_percentage = (float)currently_alive/total_cells * 100;
    printf("%.3f%c of cells currently alive\n", current_percentage, '%');

    //calculate percentage of alive cells on average
    for(int l=0; l<(u -> gen_no)+2; l++){
        total_alive += u -> alive_past[l];
    }

    double avg_percentage = ((float)total_alive/total_cells * 100)/(u -> gen_no + 1);
    printf("%.3f%c of cells alive on average\n", avg_percentage, '%');

}