#include<stdio.h>
#include<stdlib.h>
#include"gol.h"

int main(int argc, char *argv[]){

  //create universe
  struct universe v;
  struct universe *u= &v;

  //handling command-line inputs
  u -> in_yes = 0;
  u -> out_yes = 0;
  u -> no_gens = 5;
  u -> stats = 0;
  u -> torus = 0;
  
  char current;
  for(int a=1; a<argc; a++){
    current = argv[a][1];
    switch(current){
      case 'i':
        a++;
        sprintf(u -> input_file, "%s", argv[a]);
        u -> in_yes = 1;
        break;
      case 'o':
        a++;
        sprintf(u -> output_file, "%s", argv[a]);
        u -> out_yes = 1;
        break;
      case 'g':
        a++;
        if(*argv[a] == '0'){
          printf("%s\n", argv[a]);
          u -> no_gens = 0;
        } else {
          u -> no_gens = atoi(argv[a]);
          if(u -> no_gens == 0){
            exit(1);
          }
        }
        break;
      case 's':
        u -> stats = 1;
        break;
      case 't':
        u -> torus = 1;
        break;
      default:
        printf("error\n");
        exit(1);
        break;
    }
  }

  //read in input file
  FILE *infile = NULL;
  if(u -> in_yes){
    if((infile = fopen(u -> input_file, "r")) == NULL){
      printf("can't open file\n");
      exit(2);
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

  //print stats
  if(u -> stats){
      print_statistics(u);
  }

  //generate output file
  FILE *outfile = NULL;
  if(u -> out_yes){
    if((outfile = fopen(u -> output_file, "w")) == NULL){
      printf("can't generate output file\n");
      exit(2);
    }
  } else {
    outfile = stdout;
  }
  write_out_file(outfile, u);

  u = NULL;
  infile = NULL;
  outfile = NULL;

  return 0;
}
