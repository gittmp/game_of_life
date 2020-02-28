#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include"gol.h"

//function for reading data from file IN_FILE into universe structure
void read_in_file(FILE *infile, struct universe *u){

    //get line 1 of input
    char *array = calloc(513, sizeof(char));
    if(array == NULL){
        fprintf(stderr, "Error: failed to assign the pointer array\n");
        exit(1);
    }
    fgets(array, 513, infile); //need to handle error for files with over 512 columns
    if(*array == '\n'){
        fprintf(stderr, "Error: input is malformed\n");
        exit(1);
    }

    //calculate no_cols
    int b = 0;

    while(*(array+b) != '\n'){
        b++;
    }
    int no_cols = b+1;

    array = realloc(array, no_cols);
    if(array == NULL){
        fprintf(stderr, "Error: failed to reallocate memory for the pointer array\n");
        exit(1);
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
            fprintf(stderr, "Error: failed to reallocate memory for the pointer array\n");
            exit(1);
        }

        for(int d=0; d<no_cols; d++){
            fscanf(infile, "%c", (array+c));
            if(*(array+c) == '\n' && d != no_cols-1){
                fprintf(stderr, "Error: input is malformed\n");
                exit(1);
            }
            c++;
        }
    }

    no_rows -= 1;
    elements = no_cols*no_rows;

    //specifying end of universe
    array = realloc(array, elements+1);
    if(array == NULL){
        fprintf(stderr, "Error: failed to reallocate memory for the pointer array\n");
        exit(1);
    }
    *(array+elements-1) = '\n';
    *(array+elements) = '\0';

    fclose(infile);

    //initialising alive_past array
    int alive_cells = 0;
    for(int e=0; e<elements; e++){
        if(*(array + e) == '*'){
            alive_cells++;
        }
    }

    int *alive_array = calloc((u -> no_gens)+1, sizeof(int));
    if(alive_array == NULL){
        fprintf(stderr, "Error: failed to allocate memory for the pointer alive_array\n");
        exit(1);
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

    //outputting universe to file
    for(int f=0; f<(u -> elems)-1; f++){
        int print_res = fprintf(outfile, "%c", *((u -> grid) + f));
        if(print_res < 0){
            fprintf(stderr, "Error: failed to print to output file\n");
            exit(1);
        }
    }
    int print_res = fprintf(outfile, "%c", '\n');
    if(print_res < 0){
        fprintf(stderr, "Error: failed to print to output file\n");
        exit(1);
    }
}

//function for checking if a specified cell is alive
int is_alive(struct universe *u, int column, int row){
    //finding element in universe grid
    int position = row * (u -> cols) + column;
    char cell = *((u -> grid) + position);

    //determingng if cell is alive or dead
    if(cell == '*'){
        return 1;
    } else if (cell == '.'){
        return 0;
    } else {
        fprintf(stderr, "Error: input universe contains unidentified symbols\n");
        exit(1);
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
            fprintf(stderr, "Error: failed to determine next state of cell\n");
            exit(1);
    }
}

//function for determining if a cell will be alive in the next generation (cells ouside wrap around like torus)
int will_be_alive_torus(struct universe *u,  int column, int row){
    //construct neighbourhood array
    int neighbourhood[3][3];
    int alive=0;
    int n=0;

    //populate cell neighbourhood
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

    //compute no. alive cells
    int cell = neighbourhood[1][1];
    alive -= cell;

    //use given rules to determine cell's next state
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
            fprintf(stderr, "Error: failed to determine next state of cell\n");
            exit(1);
    }
}

//function for evolving current grid of cells into next generation using rule (wba or wbat)
void evolve(struct universe *u, int (*rule)(struct universe *u, int column, int row)){
    //get + set up initial data
    int no_cols = u -> cols;
    int no_rows = u -> rows;
    int no_elems = no_cols * no_rows;
    char *new_grid = calloc(no_elems+1, sizeof(char));
    if(new_grid == NULL){
        fprintf(stderr, "Error: failed to allocate memory for the pointer new_grid\n");
        exit(1);
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
            } else if(element == 0){
                *(new_grid + g) = '.';
            } else {
                fprintf(stderr, "Error: rule failed to return appropriate value\n");
                exit(1);
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
    double current_percentage = (double)currently_alive/total_cells * 100;
    printf("%.3lf%c of cells currently alive\n", current_percentage, '%');

    //calculate percentage of alive cells on average
    for(int l=0; l<(u -> no_gens) + 1; l++){
        total_alive += (u -> alive_past[l]);
    }

    double avg_percentage = ((double)total_alive/total_cells * 100)/(u -> gen_no + 1);
    printf("%.3lf%c of cells alive on average\n", avg_percentage, '%');

}
