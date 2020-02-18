#include<stdio.h>
#include<stdlib.h>
#define MAX_SIZE 513
#define IN_FILE "output.txt"
#define OUT_FILE "output.txt"
#define TEST_COL 7
#define TEST_ROW 2

//structure for holding a universe
struct universe {
    int cols;
    int rows;
    char *grid;
};

//function declarations
void read_in_file(FILE *infile, struct universe *u);
void write_out_file(FILE *outfile, struct universe *u);
int is_alive(struct universe *u, int column, int row);
int will_be_alive(struct universe *u, int column, int row);
int will_be_alive_torus(struct universe *u,  int column, int row);
void evolve(struct universe *u, int (*rule)(struct universe *u, int column, int row));

int main(){

    //setting up universe structures
    struct universe c_universe;
    struct universe *current = &c_universe;

    //creating input file pointer
    FILE *infile = NULL;

    //reading data from input file into structure
    read_in_file(infile, current);

    //checking data is all correctly generated into universe structure
    int no_cols = c_universe.cols;
    int no_rows = c_universe.rows;
    int array_size = no_cols * no_rows;
    printf("INFO:\nrows=%d, cols=%d, elements=%d\n", no_rows, no_cols, array_size);

    printf("\nINPUT GRID:\n");
    for(int z=0; z<array_size; z++){
        printf("%c", *((current -> grid) +z));
    }
    printf("\n");

    //evolving whole grid
    int (*rule)(struct universe *u, int column, int row) = will_be_alive_torus;
    evolve(current, rule);

    printf("\nOUTPUT GRID:\n");
    for(int z=0; z<array_size; z++){
        printf("%c", *((current -> grid) +z));
    }
    printf("\n");

    //writing out grid in universe *current to OUT_FILE
    FILE *outfile = NULL;
    write_out_file(outfile, current);

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
    char *array = (char*)calloc(MAX_SIZE, sizeof(char));
    fgets(array, MAX_SIZE, infile);

    //calculate no_cols
    int i = 0;
    while(*(array+i) != '\n'){
        i++;
    }
    int no_cols = i+1;

    array = realloc(array, no_cols);

    //populating rest of grid with remaining lines from input file
    int no_rows = 1;
    int elements;
    int a = no_cols;
    while(!feof(infile)){
        no_rows++;
        elements = no_rows*no_cols;
        array = realloc(array, elements);

        for(int f=0; f<no_cols; f++){
            fscanf(infile, "%c", (array+a));
            a++;
        }    
    }

    fclose(infile);

    //updating universe structure
    u -> cols = no_cols;
    u -> rows = no_rows;
    u -> grid = array;

}

//function for writing output universe to file OUT_FILE
void write_out_file(FILE *outfile, struct universe *u){
    //opening a file to output to
    if((outfile = fopen(OUT_FILE, "w")) == NULL){
        printf("can't generate output file\n");
        exit(1);
    }

    //outputting universe to file
    int z=0;
    while(*((u -> grid) + z) != '\0'){
        fprintf(outfile, "%c", *((u -> grid) + z));
        z++;
    }
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

void evolve(struct universe *u, int (*rule)(struct universe *u, int column, int row)){
    int no_elems = (u -> cols) * (u -> rows);
    char *new_grid = (char*)calloc(no_elems, sizeof(char));
    int e = 0;

    for(int r=0; r < (u -> rows); r++){
        for(int c=0; c < (u -> cols)-1; c++){
            int element = rule(u, c, r);
            if(element == 1){
                *(new_grid + e) = '*';
            } else {
                *(new_grid + e) = '.';
            }
            e++;
        }
        *(new_grid + e) = '\n';
        e++;
    }
    *(new_grid + e - 1) = '\0';

    u -> grid = new_grid;
}