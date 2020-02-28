#include<stdio.h>
#include<stdlib.h>
#define IN_FILE "input1.txt"
#define OUT_FILE "output1.txt"
#define RULE will_be_alive_torus
#define NO_GENS 5

//structure for holding a universe
struct universe {
    int cols;
    int rows;
    int elems;
    int gen_no;
    int *alive_past;
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

/*     //checking data is all correctly generated into universe structure
    int elements = current -> elems;
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

    printf("no. alive cells in gen %d is %d\n", current -> gen_no, current -> alive_past[0]);
*/
    //evolving whole grid
    int (*rule)(struct universe *u, int column, int row) = RULE;

    for(int h=0; h<NO_GENS; h++){
        evolve(current, rule);
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
    char *array = malloc(513);
    if(array == NULL){
        exit(5);
    }
    fgets(array, 513, infile);

    //calculate no_cols
    int i = 0;
    while(*(array+i) != '\n'){
        i++;
    }
    int no_cols = i+1;

    array = realloc(array, no_cols);
    if(array == NULL){
        exit(5);
    }

    //populating rest of grid with remaining lines from input file
    int no_rows = 1;
    int elements;
    int a = no_cols;
    while(!feof(infile)){
        no_rows++;
        elements = no_rows*no_cols;
        array = realloc(array, elements);
        if(array == NULL){
            exit(5);
        }

        for(int f=0; f<no_cols; f++){
            fscanf(infile, "%c", (array+a));
            a++;
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
    for(int w=0; w<elements-1; w++){
        if(*(array + w) == '*'){
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
    for(int z=0; z<(u -> elems)-1; z++){
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
    //get/set up initial data
    int no_cols = u -> cols;
    int no_rows = u -> rows;
    int no_elems = no_cols * no_rows;
    char *new_grid = calloc(no_elems+1, sizeof(char));
    if(new_grid == NULL){
        exit(5);
    }

    //create new evolved array
    int e = 0;
    int alives = 0;
    for(int r=0; r < no_rows; r++){
        for(int c=0; c < no_cols-1; c++){
            int element = rule(u, c, r);
            if(element == 1){
                *(new_grid + e) = '*';
                alives++;
            } else {
                *(new_grid + e) = '.';
            }
            e++;
        }
        *(new_grid + e) = '\n';
        e++;
    }
    *(new_grid + e - 1) = '\0';

    //update universe
    u -> gen_no += 1;
    u -> alive_past[u -> gen_no] = alives;
    u -> grid = new_grid;
}

void print_statistics(struct universe *u){
    int total_cells = (u -> elems) - (u -> rows);
    int currently_alive = u -> alive_past[u -> gen_no];
    int total_alive = 0;

    double current_percentage = (float)currently_alive/total_cells * 100;
    printf("%.3f%c of cells currently alive\n", current_percentage, '%');

    for(int g=0; g < (u -> gen_no)+2; g++){
        total_alive += u -> alive_past[g];
    }
    printf("gen_no=%d, total_cells=%d, currently_alive=%d, total_alive=%d\n", u->gen_no, total_cells, currently_alive, total_alive);

    double avg_percentage = ((float)total_alive/total_cells * 100)/(u -> gen_no + 1);
    printf("%.3f%c of cells alive on average\n", avg_percentage, '%');

}