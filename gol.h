struct universe {
/*Put some appropriate things here*/
//holds the universe of cells and any extra data
};

/*Do not modify the next seven lines*/

//reads in file from pointer, stores in universe pointer u (use dynamic memory allocation for u)
//work out how many cells per row using fscanf() with %c (returns EOF when file ends)
void read_in_file(FILE *infile, struct universe *u);

//writes content of universe pointer u to pointer outfile
void write_out_file(FILE *outfile, struct universe *u);

//returns 1 if cell(col, row) is alive, 0 otherwise
int is_alive(struct universe *u, int column, int row);

//returns 1 if cell(col, row) will be alive in next generation, 0 otherwise (assuming cells outside dead)
int will_be_alive(struct universe *u, int column, int row);

//returns 1 if cell(col, row) will be alive in next generation, 0 otherwise (assuming wrap around)
int will_be_alive_torus(struct universe *u,  int column, int row);

//evolves from the current universe to the next (rule points to either wba() or wbat())
void evolve(struct universe *u, int (*rule)(struct universe *u, int column, int row));

//calculates (and prints to screen) % of cells alive now, and average of all pervious generations
void print_statistics(struct universe *u);

/*You can modify after this line again*/


