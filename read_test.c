#include<stdio.h>
#include<stdlib.h>
#define IN_FILE "input2.txt"
#define MAX_SIZE 513

int main(){
    //getting input file
    FILE *fp1;

    if((fp1 = fopen(IN_FILE, "r")) == NULL){
        printf("can't open file\n");
        return 4;
    }

    //get line 1 of input
    char *line = calloc(MAX_SIZE, sizeof(char));
    fgets(line, MAX_SIZE, fp1);

    //calculate no_cols
    int i = 0;
    while(*(line+i) != '\n'){
        i++;
    }
    int no_cols = i+1;

    //moving line 1 into array of full grid
    int a = 0;
    char *grid = calloc(no_cols, sizeof(char));
    for(int b=0; b<no_cols; b++){
        *(grid+a) = *(line+b);
        printf("Moving element from line[%d] to grid[%d] -- element = %c\n", b, a, *(grid+a));
        a++;
    }

    //populating rest of grid with remaining lines from input file
    int no_rows = 1;
    while(!feof(fp1)){
        grid = realloc(grid, no_cols);
        line = calloc(no_cols, sizeof(char));

        for(int f=0; f<no_cols; f++){
            fscanf(fp1, "%c", &line[f]);
        }

        for(int b=0; b<no_cols; b++){
            *(grid+a) = *(line+b);
            printf("Moving element from line[%d] to grid[%d] -- element = %c\n", b, a, *(grid+a));
            a++;
        }

        no_rows++;
        free(line);
        line = NULL;
    }

    printf("rows=%d, cols=%d, elements=%d\n", no_rows, no_cols, no_rows*no_cols);

    fclose(fp1);

    int c = 0;
    while(*(grid+c) != '\0'){
        printf("%c", *(grid+c));
        c++;
    }
    printf("\n");

    return 0;
}