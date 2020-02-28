#include<stdio.h>
#include<stdlib.h>

int main(){
    struct universe{
        int no_cols;
        int no_rows;
        int *array;
    } c_universe, *current;

    current = &c_universe;
    current -> no_cols = 3;
    current -> no_rows = 3;
    current -> array = calloc(9, sizeof(int));
    if(c_universe.array == NULL){
        exit(1);
    }

    int a[3][3] = {{9,8,7},{6,5,4},{3,2,1}};
    current -> array = &a[0][0];

    int row = 0;
    int col = 0;
    int position_rc = row * c_universe.no_cols + col;

    printf("%d\n", *(c_universe.array+position_rc));
   
    return 0;
}