#include <stdio.h>
#include <argp.h>

// amount to which expand the buffer
#define BUFFER_EXPAND 160

// read a word to the global buffer and return its size
int read_word(FILE *filep);

// print the word in the buffer according to the limitations imposed by
// the command line options
void fold_lines(FILE *filep);

// initialize the structures that are gonna interface with the data 
// parsed by argp and with the data that is gonna be used by the 
// folding algorithm
void fold_init();

// Print the word in the buffer until we reach the NULL character at the 
// end of the buffer, always respecting the maximum line limitation. 
// If offset is set to something other than 0, it's assumed that the line
// is offset characters in size already
void print_at_max(char *buffer, int offset);



int parse_opt(int key, char *arg, struct argp_state *state);