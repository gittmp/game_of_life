#include<stdio.h>
#include<stdlib.h>
#define IN_FILE "input3.txt"
#define OUT_FILE "output.txt"
#define MAX_SIZE 513

//structure for holding a universe
struct universe {
    int cols;
    int rows;
    char *grid;
};

//function declarations
void read_in_file(FILE *infile, struct universe *u);
void write_out_file(FILE *outfile, struct universe *u);

int main(){

    //setting up universe structures
    struct universe c_universe;
    struct universe *current = &c_universe;

    //creating input file pointer
    FILE *infile = NULL;

    //reading data from input file into structure
    read_in_file(infile, current);

    //checking data is all correctly generated into universe structure
    int *no_cols = &c_universe.cols;
    int *no_rows = &c_universe.rows;
    int array_size = (*no_cols) * (*no_rows);
    printf("rows=%d, cols=%d, elements=%d\n", *no_rows, *no_cols, array_size);

    int c = 0;
    while(*(c_universe.grid+c) != '\0'){
        printf("%c", *(c_universe.grid+c));
        c++;
    }
    printf("\n");

    //testing file writing out
    FILE *outfile = NULL;
    write_out_file(outfile, current);

    //extracting particular element from universe structure grid
/*     
    int row = 0;
    int col = 0;
    int position_rc = row * (*no_cols) + col;
    printf("Value at position grid[%d][%d] is %c\n", row, col, *(c_universe.grid + position_rc)); 
*/

    return 0;
}

//function for reading data from file into universe structure
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

//function for writing output universe to file
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