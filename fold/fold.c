/*
    This is a reimplementation of the fold tool, don't look at the code if you
    don't want your eyes to bleed


*/

#include <stdio.h>
#include <stdlib.h>
#include <argp.h>

#include "fold.h"

// Fold method and line/word sizes
// TODO: I don't like this way of keeping track of the data, come back 
// and look it through
struct FoldOptions
{
    // Live length of line.
    int linelength;

    // Standard folding length is 70
    int maxlength;

    /*  
        the separator character
        \n: if the folding type is default
        ' ': if the folding type is fold at spaces
    */
    char separator;
};
struct FoldOptions (*foldoptions);


int main(int argc, char **argv)
{
    foldoptions = (struct FoldOptions*) malloc(sizeof( struct FoldOptions ));
    
    // TODO: parse command line arguments and set the options accordingly
    // using the argp library
    foldoptions->linelength = 0;
    foldoptions->separator = ' ';
    foldoptions->maxlength = 40;

    initialize(argv[1]);

    free(foldoptions);

    return 0;
}

void initialize(char *filename)
{
    FILE *filep = fopen(filename, "r");
    if (filep == NULL)
    {
        // TODO: add support for using stdin
        filep = stdin;
    }


    while (1)
    {
        print_line(filep);

    }

    fclose(filep);
}

int print_line(FILE *filep)
{
    int linesize = count_chars(filep);

    if (linesize > (foldoptions->maxlength))
    {
        switch (foldoptions->separator)
        {
            case ' ':
                print_at_separator(filep, (foldoptions->maxlength));
                break;
            case '\0':
                print_at_size(filep, (foldoptions->maxlength));
                break;
        }
    }
    else 
    {
        print_at_size(filep, linesize);
    }
}

void print_at_size(FILE *filep, int size)
{
    int index, ch;

    for(index = 0; index != size; ++index)
    {
        ch = getc(filep);
        if (ch == EOF) exit(0);

        putchar(ch);
    }
}

void print_at_separator(FILE *filep, int size)
{
    char *line = (char *) malloc(sizeof(char) * (foldoptions->maxlength));
    if (line == NULL) exit(12);

    int ch, index;

    // position of last seen separator
    long sep_pos = 0L;

    // position at which we started iterating the stream
    long start_pos = ftell(filep);

    for (index = 0; index != size; ++index)
    {
        ch = getc(filep);

        // if character is EOF we just break out of the loop, thus
        // preserving the state of ch for later use
        if (ch == EOF) 
        {   
            break;
        }
        else if (ch == (foldoptions->separator))
        {
            sep_pos = ftell(filep);
        }

        if (ch != '\n')
        {
            // we gotta to ignore the newline characters
            line[index] = ch;
        }
        else 
        {
            --index;
        }
    }
    
    if (sep_pos == 0L)
    {
        // just print up to size
        for (index = 0; index != (size - 1); ++index)
        {
            putchar(line[index]);
        }
    }
    else
    {
        // reset the position of the stream to the last seen separator
        fseek(filep, sep_pos, SEEK_SET);

        // the range from 0 up to the last seen separator position
        long last_sep_pos = (sep_pos - start_pos);

        for (index = 0; index != (last_sep_pos); ++index)
        {
            putchar(line[index]);
        }
    }

    putchar('\n');

    if (ch == EOF)
    {
        exit(0);
    }

    free(line);
}

int count_chars(FILE *filep)
{
    int linesize, ch;
    linesize = ch = 0;

    long streampos = ftell(filep);

    do
    {   
        ch = getc(filep);
        ++linesize;

        if (ch == EOF) break;

    } while (ch != '\n');

    fseek(filep, streampos, SEEK_SET);

    return linesize;
}