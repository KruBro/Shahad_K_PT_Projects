#include <stdio.h>
#include <string.h>
#include "s2html_event.h"

#define SIZE_OF_SYMBOLS (sizeof(symbols))
#define SIZE_OF_OPERATORS (sizeof(operators))
#define WORD_BUFF_SIZE	100

/********** Internal states and event of parser **********/
typedef enum
{
	PSTATE_IDLE,
	PSTATE_PREPROCESSOR_DIRECTIVE,
	PSTATE_SUB_PREPROCESSOR_MAIN,
	PSTATE_SUB_PREPROCESSOR_RESERVE_KEYWORD,
	PSTATE_SUB_PREPROCESSOR_ASCII_CHAR,
	PSTATE_HEADER_FILE,
	PSTATE_RESERVE_KEYWORD,
	PSTATE_NUMERIC_CONSTANT,
	PSTATE_STRING,
	PSTATE_SINGLE_LINE_COMMENT,
	PSTATE_MULTI_LINE_COMMENT,
	PSTATE_ASCII_CHAR
}pstate_e;

/********** global variables **********/

/* parser state variable */
static pstate_e state = PSTATE_IDLE;

/* sub state is used only in preprocessor state */
static pstate_e state_sub = PSTATE_SUB_PREPROCESSOR_MAIN;

/* event variable to store event and related properties */
static pevent_t pevent_data;
static int event_data_idx=0;

static char word[WORD_BUFF_SIZE];
static int word_idx=0;


static char* res_kwords_data[] = {"const", "volatile", "extern", "auto", "register",
   						   "static", "signed", "unsigned", "short", "long", 
						   "double", "char", "int", "float", "struct", 
						   "union", "enum", "void", "typedef", ""
						  };

static char* res_kwords_non_data[] = {"goto", "return", "continue", "break", 
							   "if", "else", "for", "while", "do", 
							   "switch", "case", "default","sizeof", ""
							  };

static char operators[] = {'/', '+', '*', '-', '%', '=', '<', '>', '~', '&', ',', '!', '^', '|'};
static char symbols[] = {'(', ')', '{', '[', ':'};

/********** state handlers **********/
pevent_t * pstate_idle_handler(FILE *fd, int ch);
pevent_t * pstate_single_line_comment_handler(FILE *fd, int ch);
pevent_t * pstate_multi_line_comment_handler(FILE *fd, int ch);
pevent_t * pstate_numeric_constant_handler(FILE *fd, int ch);
pevent_t * pstate_string_handler(FILE *fd, int ch);
pevent_t * pstate_header_file_handler(FILE *fd, int ch);
pevent_t * pstate_ascii_char_handler(FILE *fd, int ch);
pevent_t * pstate_reserve_keyword_handler(FILE *fd, int ch);
pevent_t * pstate_preprocessor_directive_handler(FILE *fd, int ch);
pevent_t * pstate_sub_preprocessor_main_handler(FILE *fd, int ch);

/********** Utility functions **********/

/* Appends a single character to the current event's data buffer.
 * The buffer is a fixed size array (PEVENT_DATA_SIZE), so the index
 * is checked first and one slot is always reserved for the terminating
 * '\0' that set_parser_event() writes later. Characters beyond the
 * buffer capacity are simply dropped instead of overflowing memory. */
static void store_event_char(int ch)
{
	if(event_data_idx < PEVENT_DATA_SIZE - 1)
		pevent_data.data[event_data_idx++] = ch;
}

/* Appends a single character to the temporary "word" buffer used while
 * an identifier/keyword is being collected. Same fixed-size protection
 * as store_event_char(), applied to the smaller word[] buffer. */
static void store_word_char(int ch)
{
	if(word_idx < WORD_BUFF_SIZE - 1)
		word[word_idx++] = ch;
}

/* function to check if given word is reserved key word */
static int is_reserved_keyword(char *word)
{
	int idx = 0;

	/* search for data type reserved keyword */
	while(*res_kwords_data[idx])
	{
		if(strcmp(res_kwords_data[idx++], word) == 0)
			return RES_KEYWORD_DATA;
	}

	idx = 0; // reset index
	/* search for non data type reserved key word */
	while(*res_kwords_non_data[idx])
	{
		if(strcmp(res_kwords_non_data[idx++], word) == 0)
			return RES_KEYWORD_NON_DATA;
	}

	return 0; // word did not match, return false
}

/* function to check symbols */
static int is_symbol(char c)
{
	size_t idx; // size_t matches the unsigned type returned by sizeof()
	for(idx = 0; idx < SIZE_OF_SYMBOLS; idx++)
	{
		if(symbols[idx] == c)
			return 1;
	}

	return 0;
}

/* function to check operator */
static int is_operator(char c)
{
	size_t idx; // size_t matches the unsigned type returned by sizeof()
	for(idx = 0; idx < SIZE_OF_OPERATORS; idx++)
	{
		if(operators[idx] == c)
			return 1;
	}

	return 0;
}

/* to set parser event */
static void set_parser_event(pstate_e s, pevent_e e)
{
	pevent_data.data[event_data_idx] = '\0';
	pevent_data.length = event_data_idx;
	event_data_idx = 0;
	state = s;
	pevent_data.type = e;
}


/************ Event functions **********/

/* This function parses the source file and generate 
 * event based on parsed characters and string
 */
pevent_t *get_parser_event(FILE *fd)
{
	int ch;
	pevent_t *evptr = NULL;
	/* Read char by char */
	while((ch = fgetc(fd)) != EOF)
	{
#ifdef DEBUG
	//	putchar(ch);
#endif
		switch(state)
		{
			case PSTATE_IDLE :
				if((evptr = pstate_idle_handler(fd, ch)) != NULL)
					return evptr;
				break;
			case PSTATE_SINGLE_LINE_COMMENT :
				if((evptr = pstate_single_line_comment_handler(fd, ch)) != NULL)
					return evptr;
				break;
			case PSTATE_MULTI_LINE_COMMENT :
				if((evptr = pstate_multi_line_comment_handler(fd, ch)) != NULL)
					return evptr;
				break;
			case PSTATE_PREPROCESSOR_DIRECTIVE :
				if((evptr = pstate_preprocessor_directive_handler(fd, ch)) != NULL)
					return evptr;
				break;
			case PSTATE_RESERVE_KEYWORD :
				if((evptr = pstate_reserve_keyword_handler(fd, ch)) != NULL)
					return evptr;
				break;
			case PSTATE_NUMERIC_CONSTANT :
				if((evptr = pstate_numeric_constant_handler(fd, ch)) != NULL)
					return evptr;
				break;
			case PSTATE_STRING :
				if((evptr = pstate_string_handler(fd, ch)) != NULL)
					return evptr;
				break;
			case PSTATE_HEADER_FILE :
				if((evptr = pstate_header_file_handler(fd, ch)) != NULL)
					return evptr;
				break;
			case PSTATE_ASCII_CHAR :
				if((evptr = pstate_ascii_char_handler(fd, ch)) != NULL)
					return evptr;
				break;
			default : 
				printf("unknown state\n");
				state = PSTATE_IDLE;
				break;
		}
	}

	/* end of file is reached, move back to idle state and set EOF event */
	set_parser_event(PSTATE_IDLE, PEVENT_EOF);

	return &pevent_data; // return final event
}


/********** IDLE state Handler **********
 * Idle state handler identifies the start of every token type
 * (comments, strings, numbers, identifiers, preprocessor lines, etc.)
 * and switches the parser into the matching state.
 ****************************************/

pevent_t * pstate_idle_handler(FILE *fd, int ch)
{
	int pre_ch;
	switch(ch)
	{
		case '\'' : // beginning of ASCII char 
			if (event_data_idx) // Flush the regular text first!
			{
				fseek(fd, -1L, SEEK_CUR);
				set_parser_event(PSTATE_IDLE, PEVENT_REGULAR_EXP);
				return &pevent_data;
			}
			state = PSTATE_ASCII_CHAR;
			store_event_char(ch);
			break;

		case '/' :
			pre_ch = ch;
			if((ch = fgetc(fd)) == '*') // multi line comment
			{
				if(event_data_idx) // we have regular exp in buffer first process that
				{
					fseek(fd, -2L, SEEK_CUR); // unget chars
					set_parser_event(PSTATE_IDLE, PEVENT_REGULAR_EXP);
					return &pevent_data;
				}
				else // multi line comment begin 
				{
#ifdef DEBUG	
					printf("Multi line comment Begin : /*\n");
#endif
					state = PSTATE_MULTI_LINE_COMMENT;
					store_event_char(pre_ch);
					store_event_char(ch);
				}
			}
			else if(ch == '/') // single line comment
			{
				if(event_data_idx) // we have regular exp in buffer first process that
				{
					fseek(fd, -2L, SEEK_CUR); // unget chars
					set_parser_event(PSTATE_IDLE, PEVENT_REGULAR_EXP);
					return &pevent_data;
				}
				else // single line comment begin
				{
#ifdef DEBUG	
					printf("Single line comment Begin : //\n");
#endif
					state = PSTATE_SINGLE_LINE_COMMENT;
					store_event_char(pre_ch);
					store_event_char(ch);
				}
			}
			else // it is regular exp
			{
				store_event_char(pre_ch);
				store_event_char(ch);
			}
			break;

		case '#' :
			if (event_data_idx) // Flush the regular text first!
			{
				fseek(fd, -1L, SEEK_CUR);
				set_parser_event(PSTATE_IDLE, PEVENT_REGULAR_EXP);
				return &pevent_data;
			}
			state = PSTATE_PREPROCESSOR_DIRECTIVE;
			store_event_char(ch);
			break;

		case '\"' :
			if (event_data_idx) // Flush the regular text first!
			{
				fseek(fd, -1L, SEEK_CUR);
				set_parser_event(PSTATE_IDLE, PEVENT_REGULAR_EXP);
				return &pevent_data;
			}
			state = PSTATE_STRING;
			store_event_char(ch);
			break;

		case '0' ... '9' : // detect numeric constant
			if(event_data_idx)
			{
				// If it's part of another variable name or etc...
				store_event_char(ch);
			}
			else
			{
				// Buffer is empty, this is the start of a real number
				state = PSTATE_NUMERIC_CONSTANT;
				store_event_char(ch);
			}
			break;

		case 'a' ... 'z' : // could be reserved key word
		case 'A' ... 'Z' :
		case '_':
			if (event_data_idx) // Flush the regular text first!
			{
				fseek(fd, -1L, SEEK_CUR);
				set_parser_event(PSTATE_IDLE, PEVENT_REGULAR_EXP);
				return &pevent_data;
			}
			state = PSTATE_RESERVE_KEYWORD;
			store_word_char(ch);
			break;

		default : // Assuming common text starts by default.
			store_event_char(ch);
			break;
	}

	return NULL;
}

/* Preprocessor lines (#include, #define, etc.) can themselves contain
 * sub-tokens. Right now only the "main" sub-state is reachable; the
 * keyword/ascii-char sub-states are reserved for future use so that
 * preprocessor lines can highlight nested tokens the same way regular
 * code does. */
pevent_t * pstate_preprocessor_directive_handler(FILE *fd, int ch)
{
	switch(state_sub)
	{
		case PSTATE_SUB_PREPROCESSOR_MAIN :
			return pstate_sub_preprocessor_main_handler(fd, ch);
		case PSTATE_SUB_PREPROCESSOR_RESERVE_KEYWORD :
			return pstate_reserve_keyword_handler(fd, ch);
		case PSTATE_SUB_PREPROCESSOR_ASCII_CHAR :
			return pstate_ascii_char_handler(fd, ch);
		default :
				printf("unknown state\n");
				state = PSTATE_IDLE;
	}

	return NULL;
}
pevent_t * pstate_sub_preprocessor_main_handler(FILE *fd, int ch)
{
	if(ch == '\n')
	{
		fseek(fd, -1L, SEEK_CUR);
		set_parser_event(PSTATE_IDLE, PEVENT_PREPROCESSOR_DIRECTIVE);
		return &pevent_data;
	}
	else if(ch == '<')
	{
		set_parser_event(PSTATE_HEADER_FILE, PEVENT_PREPROCESSOR_DIRECTIVE);
		return &pevent_data;
	}
	else if(ch == '"')
	{
		fseek(fd, -1L, SEEK_CUR);
		set_parser_event(PSTATE_HEADER_FILE, PEVENT_PREPROCESSOR_DIRECTIVE);
		return &pevent_data;
	}
	else
	{
		store_event_char(ch);
		return NULL;
	}
}

/* Collects the file name that appears between < > or " " in an #include
 * line and tags it as a standard or user header so the HTML output can
 * style them differently. */
pevent_t * pstate_header_file_handler(FILE *fd, int ch)
{
	(void)fd; // fd is part of the common handler signature but not needed here

	if(ch == '>')
	{
		pevent_data.property = STD_HEADER_FILE;
		set_parser_event(PSTATE_IDLE, PEVENT_HEADER_FILE);
		return &pevent_data;
	}
	else if(ch == '"')
	{
		store_event_char(ch);

		if(event_data_idx > 1)
		{
			pevent_data.property = USER_HEADER_FILE;
			set_parser_event(PSTATE_IDLE, PEVENT_HEADER_FILE);
			return &pevent_data;
		}

		return NULL;
	}
	else
	{
		store_event_char(ch);
		return NULL;
	}
}

/* Collects letters/digits/underscores into the word[] buffer until a
 * non-identifier character is seen, then checks whether the finished
 * word matches one of the reserved keyword lists. */
pevent_t * pstate_reserve_keyword_handler(FILE *fd, int ch)
{
    // Check for letters, numbers, and underscores
    if(isalnum(ch) || ch == '_')
    {
        store_word_char(ch);
        return NULL; // Still accumulating, keep going
    }
    else
    {
        // 1. The Rewind
        fseek(fd, -1L, SEEK_CUR);
        
        // 2. String Formatting
        word[word_idx] = '\0';

        // 3. The Assessment
        int type = is_reserved_keyword(word);
        
        // 4. Memory Transfer
        event_data_idx = word_idx;
        strcpy(pevent_data.data, word);
        word_idx = 0; // Reset for next time

        // 5. The Packaging
        if(type != 0) 
        {
            pevent_data.property = type;
            set_parser_event(PSTATE_IDLE, PEVENT_RESERVE_KEYWORD);
        }
        else
        {
            set_parser_event(PSTATE_IDLE, PEVENT_REGULAR_EXP);
        }

        return &pevent_data; // Payload is ready, send it out!
    }
}

/* Collects digits (and any trailing letters/dots that belong to the
 * literal) until whitespace, an operator, or a symbol marks the end of
 * the numeric constant. */
pevent_t * pstate_numeric_constant_handler(FILE *fd, int ch)
{
	//If it hits the wall that marks the end of the number 
	if(ch == '\t' || ch == ' ' || ch == ';' || ch == '\n' || is_operator(ch) || is_symbol(ch))
	{
		fseek(fd, -1L, SEEK_CUR);
		set_parser_event(PSTATE_IDLE, PEVENT_NUMERIC_CONSTANT);
		return &pevent_data;
	}

	//If it is not the wall, Store the char in to the buffer
	store_event_char(ch);
	return NULL;
}

/* Collects characters of a double-quoted string literal, honoring
 * backslash-escaped characters (including an escaped quote) so the
 * literal isn't ended early. */
pevent_t * pstate_string_handler(FILE *fd, int ch)
{
	switch(ch)
	{
		case '\\':
			store_event_char(ch);
			ch = fgetc(fd);
			if(ch != EOF) // guard against a dangling backslash at end of file
				store_event_char(ch);
			break;
		case '"':
			store_event_char(ch);
			set_parser_event(PSTATE_IDLE, PEVENT_STRING);
			return &pevent_data;
		default:
			store_event_char(ch);
			break;
	}

	return NULL;
}

/* Collects characters of a // comment until the terminating newline. */
pevent_t * pstate_single_line_comment_handler(FILE *fd, int ch)
{
	(void)fd; // fd is part of the common handler signature but not needed here

	switch(ch)
	{
		case '\n' : /* single line comment ends here */
#ifdef DEBUG	
			printf("\nSingle line comment end\n");
#endif
			store_event_char(ch);
			set_parser_event(PSTATE_IDLE, PEVENT_SINGLE_LINE_COMMENT);
			return &pevent_data;
		default :  // collect single line comment chars
			store_event_char(ch);
			break;
	}

	return NULL;
}

/* Collects characters of a /* *\/ comment, looking one character ahead
 * whenever a '*' or '/' is seen to detect the closing "*\/" sequence
 * (which may be split across two separate reads of the parser loop). */
pevent_t * pstate_multi_line_comment_handler(FILE *fd, int ch)
{
	int pre_ch;
	switch(ch)
	{
		case '*' : /* comment might end here */
			store_event_char(ch);
			if((ch = fgetc(fd)) == '/')
			{
#ifdef DEBUG	
				printf("\nMulti line comment End : */\n");
#endif
				store_event_char(ch);
				set_parser_event(PSTATE_IDLE, PEVENT_MULTI_LINE_COMMENT);
				return &pevent_data;
			}
			else // multi line comment string still continued
			{
				store_event_char(ch);
			}
			break;
		case '/' :
			/* go back by two steps and read previous char */
			fseek(fd, -2L, SEEK_CUR); // move two steps back
			pre_ch = fgetc(fd); // read a char
			fgetc(fd); // to come back to current offset

			store_event_char(ch);
			if(pre_ch == '*')
			{
				set_parser_event(PSTATE_IDLE, PEVENT_MULTI_LINE_COMMENT);
				return &pevent_data;
			}
			break;
		default :  // collect multi-line comment chars
			store_event_char(ch);
			break;
	}

	return NULL;
}

/* Collects characters of a 'x' character literal, honoring
 * backslash-escaped characters (including an escaped quote). */
pevent_t * pstate_ascii_char_handler(FILE *fd, int ch)
{
	switch(ch)
	{
		case '\\':
			store_event_char(ch);
			ch = fgetc(fd);
			if(ch != EOF) // guard against a dangling backslash at end of file
				store_event_char(ch);
			break;
		case '\'':
			store_event_char(ch);
			set_parser_event(PSTATE_IDLE, PEVENT_ASCII_CHAR);
			return &pevent_data;
		default:
			store_event_char(ch);
			break;
	}
	
	return NULL;
}
/**** End of file ****/