#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include"gol.h"

int main(int argc, char *argv[]){

  //create universe
  struct universe v;
  struct universe *u= &v;

  //initialise universe values to defaults
  u -> in_yes = 0;
  u -> out_yes = 0;
  u -> input_file = calloc(50, sizeof(char));
  u -> output_file = calloc(50, sizeof(char));
  u -> stats = 0;
  u -> torus = 0;
  u -> no_gens = 5;
  
  //handling specified command-line inputs 
  char current;
  for(int a=1; a<argc; a++){
    current = argv[a][1];
    switch(current){
      case 'i':
        a++;

        //handling conflicting -i options
        if(u -> in_yes && !(strcmp(argv[a], u -> input_file) == 0)){
          fprintf(stderr, "Error: conflicting command-line options given\n");
          exit(1);
        }

        int in_res = sprintf(u -> input_file, "%s", argv[a]);

        //handling failure of sprintf function, too large file names, and unspecified inputs
        if(in_res < 0 || in_res > 50 || strcmp(u -> input_file, "(null)") == 0){
          fprintf(stderr, "Error: failed to read input file name\n");
          exit(1);
        }

        u -> in_yes = 1;
        break;

      case 'o':
        a++;

        //handling conflicting -o options
        if(u -> out_yes && !(strcmp(argv[a], u -> output_file) == 0)){
          fprintf(stderr, "Error: conflicting command-line options given\n");
          exit(1);
        }

        int out_res = sprintf(u -> output_file, "%s", argv[a]);

        //handling failure of sprintf function, too large file names, and unspecified inputs (incl. specifying other options after)
        if(out_res < 0 || out_res > 50 || strcmp(u -> output_file, "(null)") == 0 || u->output_file[0] == '-'){
          fprintf(stderr, "Error: failed to read output file name\n");
          exit(1);
        }

        u -> out_yes = 1;
        break;

      case 'g':
        a++;
        int g;
        
        //handling failure of atoi function
        if(strcmp(argv[a], "0") == 0){
          g = 0;
        } else {
          g = atoi(argv[a]);
          if(g == 0){
            fprintf(stderr, "Error: function atoi failed\n");
            exit(1);
          }
        }

        //handling conflicting -g options
        if(u -> no_gens != 5 && g != u -> no_gens){
          fprintf(stderr, "Error: conflicting command-line options given\n");
          exit(1);
        }

        u -> no_gens = g;
        break;

      case 's':
        u -> stats = 1;
        break;

      case 't':
        u -> torus = 1;
        break;

      default:
        fprintf(stderr, "Error: invalid command-line arguments supplied\n");
        exit(1);
    }
  }

  //read in input file
  FILE *infile = NULL;
  if(u -> in_yes){
    if((infile = fopen(u -> input_file, "r")) == NULL){
      fprintf(stderr, "Error: failed to open input file\n");
      exit(1);
    }
  } else {
    infile = stdin;
  }
  read_in_file(infile, u);

  //select rule
  int (*rule)(struct universe *u, int column, int row);
  if(u -> torus){
    rule = will_be_alive_torus;
  } else {
    rule = will_be_alive;
  }

  //evolve universe
  for(int b=0; b<(u -> no_gens); b++){
      evolve(u, rule);
  }

  //generate output file
  FILE *outfile = NULL;
  if(u -> out_yes){
    if((outfile = fopen(u -> output_file, "w")) == NULL){
      fprintf(stderr, "Error: failed to open/generate output file\n");
      exit(1);
    }
  } else {
    outfile = stdout;
  }
  write_out_file(outfile, u);

  //print stats
  if(u -> stats){
      print_statistics(u);
  }

  u = NULL;
  infile = NULL;
  outfile = NULL;

  return 0;
}
