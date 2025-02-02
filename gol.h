struct universe {
    char *input_file;
    char *output_file;
    int in_yes;
    int out_yes;
    int no_gens;
    int stats;
    int torus;
    int cols;
    int rows;
    int elems;
    int gen_no;
    int *alive_past;
    char *grid;
};

/*Do not modify the next seven lines*/
void read_in_file(FILE *infile, struct universe *u);
void write_out_file(FILE *outfile, struct universe *u);
int is_alive(struct universe *u, int column, int row);
int will_be_alive(struct universe *u, int column, int row);
int will_be_alive_torus(struct universe *u,  int column, int row);
void evolve(struct universe *u, int (*rule)(struct universe *u, int column, int row));
void print_statistics(struct universe *u);
/*You can modify after this line again*/

