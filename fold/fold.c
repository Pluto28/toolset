/*
    This is a reimplementation of the fold tool, don't look at the code if you
    don't want your eyes to bleed
*/

#include <stdio.h>
#include <stdlib.h>
#include <argp.h>
#include <string.h>

#include "fold.h"

const char * argp_program_version  =     "1.0.0";
const char * argp_program_bug_address =  ":)";
const char * args_doc =                  "FILE";
const char * doc = "reimplementation of the fold utility";

static struct argp_option argp_options[] =
{
    {"spaces", 's', 0, 0, "Fold at spaces"},
    {"width", 'w', "WIDTH", 0, "Use WIDTH columns instead of 80"},
    {0}
};

static struct argp argp =
{
    argp_options,
    parse_opt,
    "FILE",
    "fold lines from a file"
};

// Fold options as specified by the command line arguments
struct FoldArgs
{
    // Width in columns of the folding length limit, default is 80.
    unsigned int maxlength;

    // If 0, print at column, otherwise print at separator
    char separator;

    // pointer to array containing the name of the file we 
    // are gonna read and fold
    char *filename;
};
struct FoldArgs (*foldargs);

// folding data that the folding algorithm is responsible for managing
struct FoldData
{
    // actual size of the buffer
    unsigned buffer_size;

    // the length of the line we are printing, as
    // limited by foldargs->maxlength
    unsigned int line_length;
};
struct FoldData (*folddata);

// global buffer used to store one word at time, word separator
// is usually a space
char *word_buffer;

// argp parser
int parse_opt(int key, char *arg, struct argp_state *state)
{
    switch (key)
    {
    case 's':
        foldargs->separator = ' ';
        break;
    case 'w':
        int width = atoi(arg);
        
        if (width > 0)
        {
            foldargs->maxlength = width;
        }
        else
        {
            fprintf(stderr, "fold: invalid number of columns: ‘%s’", arg);
            exit(EINVAL);
        }
        break;
    case ARGP_KEY_ARG:
        foldargs->filename = arg;
        break;
    default:
        return ARGP_ERR_UNKNOWN;
        break;
    }

    return 0;
}

/*****************************core part of the algorithm**************************
 *                      FROM HERE ON IS WHERE THE MAGIC HAPPENS                  *
 * *******************************************************************************/

//TODO: reallocate buffer size by BUFFER_EXPAND if word is bigger than 160 
// bytes
int read_word(FILE *filep)
{
    int ch, index;
    index = 0;

    do
    {
        ch = getc(filep);
        if (ch == EOF) break;

        word_buffer[index] = ch;
        ++index;

    } while (ch != ' ' && ch != '\n');
    
    word_buffer[index] = '\0';
    ungetc(ch, filep); // for comparison purposes

    return index;   
}

void fold_lines(FILE *filep)
{
    unsigned int word_size, new_size;
    char separator = foldargs->separator;

    while (1)
    {

        word_size = read_word(filep);
        // size line would have after printing word
        new_size = word_size + (folddata->line_length);

        if (new_size > (foldargs->maxlength))
        {
            if (separator == ' ')
            {
                if (word_size > (foldargs->maxlength))
                {
                    // if word is bigger than limit, it will spread through multiple
                    // lines, so we use the print_at_max function to neatly
                    // handle the behavior
                    putchar('\n');
                    print_at_max(word_buffer, 0);
                } else {
                    // if the size of the word is smaller than that of one line, we
                    // can just print it in a new line and go on with our lives
                    putchar('\n');
                    printf("%s", word_buffer);

                    // always remember to update the trackers, the algorithms gets
                    // angry when you don't
                    folddata->line_length = word_size;
                }
            }
            else if (separator == 0)
            {
                print_at_max(word_buffer, (folddata->line_length));
            }

        }
        else
        {
            printf("%s", word_buffer);
            folddata->line_length += word_size;
        }

        // if the last character of our word is a newline character, 
        // then we start a new line and the size of the line is reset
        // to 0
        if (word_buffer[--word_size] == '\n')
        {
            folddata->line_length = 0;
        }

        if (getc(filep) == EOF) exit(EXIT_SUCCESS);
    }
}


void print_at_max(char *buffer, int offset)
{
    int buffer_offset = 0;
    int line_offset = offset;   // line offset, reset every new line

    char ch = buffer[buffer_offset];

    for (; ch != '\0'; ++buffer_offset)
    {
        ch = buffer[buffer_offset];
        if (ch != '\n')
        {
            putchar(ch);
        } else {
            --buffer_offset, --line_offset;
        }

        line_offset++;
        if (line_offset == (foldargs->maxlength))
        {
            line_offset = 0;
            putchar('\n');
        }
    }

    // Because line_offset starts at 
    folddata->line_length = line_offset;
}


void fold_init()
{
    foldargs = (struct FoldArgs*) malloc(sizeof( struct FoldArgs ));
    if (!foldargs) perror("fold");

    folddata = (struct FoldData*) malloc(sizeof( struct FoldData ));
    if (!foldargs) perror("fold");

    word_buffer = (char *) malloc( sizeof(char) * BUFFER_EXPAND );
    if (!word_buffer) perror("fold");

    // set the default values to our structures
    foldargs->maxlength = 80;
    foldargs->separator = (char) 0;
    foldargs->filename = NULL;

    folddata->buffer_size = 0;
    folddata->line_length = 0;
}


int main(int argc, char **argv)
{
    // initialize our structures
    fold_init();


    // parse command line arguments 
    argp_parse(&argp, argc, argv, 0, 0, 0);

    // if filename was not provided, read from stdin
    FILE *filep;
    if ((foldargs->filename) != NULL) 
    {
        filep = fopen(foldargs->filename, "r");
        if (!filep) perror("fold");
    } else {
        filep = stdin;
    }

    fold_lines(filep);

    // close file we're reading and deallocate our structs
    // and our buffer
    fclose(filep);
    free(foldargs);
    free(folddata);
    free(word_buffer);

    return 0;
}