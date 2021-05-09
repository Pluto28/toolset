#include <stdio.h>

// count the number of characters in the word we are iterating, with space
// being the separator of the words. Newline characters are ignored
int count_chars(FILE *filep);

// print at separator that is given with a struct, with
// size specifying the limit of characters that can be printed
void print_at_separator(FILE *filep, int size);

// print exactly size characters
void print_at_size(FILE *filep, int size);

// print a folded line
int print_line(FILE *filep);

// set things up before starting the program itself
void initialize(char *filename);