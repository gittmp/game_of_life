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
    int elements;
    char *grid;
};

//function declarations
void read_in_file(FILE *infile, struct universe *u);
void write_out_file(FILE *outfile, struct universe *u);
int is_alive(struct universe *u, int column, int row);
int will_be_alive(struct universe *u, int column, int row);
int will_be_alive_torus(struct universe *u,  int column, int row);
void evolve(struct universe *u, int (*rule)(struct universe *u, int column, int row));
void print_statistics(struct universe *u);

int main(){

    //setting up universe structures
    struct universe c_universe;
    struct universe *current = &c_universe;

    //creating input file pointer
    FILE *infile = NULL;

    //reading data from input file into structure
    read_in_file(infile, current);

    //checking data is all correctly generated into universe structure
    int elements = current -> elements;
    char *array = current -> grid;

    printf("INPUT GRID:\n");
    for(int f=0; f<elements+1; f++){
        if(*(array+f)=='\n'){
            printf("|n\n");
        } else if(*(array+f)=='\0'){
            printf("|0\n");
        } else {
            printf("%c", *(array+f));
        }    
    }

    //evolving whole grid
    int (*rule)(struct universe *u, int column, int row) = will_be_alive_torus;
    evolve(current, rule);

    printf("\nOUTPUT GRID:\n");
    for(int g=0; g<elements+1; g++){
        if(*(array+g)=='\n'){
            printf("|n\n");
        } else if(*(array+g)=='\0'){
            printf("|0\n");
        } else {
            printf("%c", *(array+g));
        }    
    }

    print_statistics(current);

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
    char *array = (char*)malloc(MAX_SIZE);
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

    no_rows -= 1;
    elements = no_cols*no_rows;

    array = realloc(array, elements+1);
    *(array+elements-1) = '\n';
    *(array+elements) = '\0';

    fclose(infile);

    //updating universe structure
    u -> grid = array;
    u -> cols = no_cols;
    u -> rows = no_rows;
    u -> elements = no_rows*no_cols;
}

//function for writing output universe to file OUT_FILE
void write_out_file(FILE *outfile, struct universe *u){
    //opening a file to output to
    if((outfile = fopen(OUT_FILE, "w")) == NULL){
        printf("can't generate output file\n");
        exit(1);
    }

    //outputting universe to file
    for(int z=0; z<(u -> elements)-1; z++){
        fprintf(outfile, "%c", *((u -> grid) + z));
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

void evolve(struct universe *u, int (*rule)(struct universe *u, int column, int row)){
    int no_cols = u -> cols;
    int no_rows = u -> rows;
    int no_elems = no_cols * no_rows;
    char *new_grid = (char*)calloc(no_elems+1, sizeof(char));
    int e = 0;

    for(int r=0; r < no_rows; r++){
        for(int c=0; c < no_cols-1; c++){
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

void print_statistics(struct universe *u){
    int total_cells = (u -> rows) * ((u -> cols)-1);
    int alive_cells = 0;
    for(int w=0; w<(u -> elements)-1; w++){
        if(*((u -> grid) + w) == '*'){
            alive_cells++;
        }
    }
    double percentage = (float)alive_cells/total_cells * 100;
    printf("%d, %d, %.3f percent of cells currently alive\n", alive_cells, total_cells, percentage);
}