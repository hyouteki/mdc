#ifndef MDC_H_
#define MDC_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define LINE_CAPACITY 8192
#define BUFFER_CAPACITY 8192

typedef enum {
	Mdc_Token_Heading,
    Mdc_Token_Paragraph,
    Mdc_Token_Bold,
    Mdc_Token_Italic,
    Mdc_Token_ListItem,
    Mdc_Token_Quote,
    Mdc_Token_Hrule,
    Mdc_Token_Text
} Mdc_TokenType;

typedef struct Mdc_TokenList {
    Mdc_TokenType type;
    char *content;
    struct Mdc_TokenList *next;
} Mdc_TokenList;

static void Mdc_TokenList_Add(Mdc_TokenList **, Mdc_TokenType, char *);
static void Mdc_TrimWhiteSpace(char *);

void Mdc_TokenList_Free(Mdc_TokenList *); 
void Mdc_Tokenize(Mdc_TokenList **, const char *);
void Mdc_ConvertToHtml(FILE *, Mdc_TokenList *);
Mdc_TokenList *Mdc_TokenizeFile(char *);

static void Mdc_TokenList_Add(Mdc_TokenList **list, Mdc_TokenType type, char *content) {
	Mdc_TokenList *token = (Mdc_TokenList *)malloc(sizeof(Mdc_TokenList));
	token->type = type;
	token->content = strdup(content);
	token->next = NULL;
	if (*list == NULL) {
		*list = token;
		return;
	}
	Mdc_TokenList *itr = *list;
	while (itr->next) itr = itr->next;
	itr->next = token;
}

static void Mdc_TrimWhiteSpace(char *text) {
    while (isspace((unsigned char)*text)) text++;
	char *end = text + strlen(text) - 1;
    while (end > text && isspace((unsigned char)*end)) end--;
    *(end + 1) = 0;
}

void Mdc_TokenList_Free(Mdc_TokenList *list) {
	while (list) {
		Mdc_TokenList *next = list->next;
		free(list->content);
		list = next;
	}
}

void Mdc_Tokenize(Mdc_TokenList **list, const char *text) {
    const char *ptr = text;
    char buffer[BUFFER_CAPACITY];
    while (*ptr) {
		int len = 0, type = 0;
        if (*ptr == '#') {
			ptr++;
            while (*ptr && *ptr != '\n') buffer[len++] = *ptr++;
			type = Mdc_Token_Heading;
        } else if (*ptr == '*' && *(ptr+1) == '*') {
            ptr += 2;
            while (*ptr && !(*ptr == '*' && *(ptr+1) == '*'))
                buffer[len++] = *ptr++;
            ptr += 2;
			type = Mdc_Token_Bold;
        } else if (*ptr == '*') {
            ptr++;
            while (*ptr && !(*ptr == '*')) buffer[len++] = *ptr++;
            ptr++;
            type = Mdc_Token_Italic;
        } else if (*ptr == '-' && *(ptr+1) == '-' && *(ptr+2) == '-') {
            ptr += 3;
            type = Mdc_Token_Hrule;
        } else if (*ptr == '>') {
            ptr++;
            while (*ptr && *ptr != '\n') buffer[len++] = *ptr++;
			type = Mdc_Token_Quote;
        } else if (*ptr == '*' || *ptr == '-') {
            ptr++;
            while (*ptr && *ptr != '\n') buffer[len++] = *ptr++;
            type = Mdc_Token_ListItem;
        } else {
            while (*ptr && *ptr != '\n') buffer[len++] = *ptr++;
            type = Mdc_Token_Text;
        }		
		buffer[len] = 0;
		Mdc_TrimWhiteSpace(buffer);
		Mdc_TokenList_Add(list, type, buffer);
        if (*ptr == '\n') ptr++;
    }
}

void Mdc_ConvertToHtml(FILE *stream, Mdc_TokenList *list) {
	Mdc_TokenList *itr = list;
    while (itr) {
        switch (itr->type) {
		case Mdc_Token_Heading:
			fprintf(stream, "<h1>%s</h1>\n", itr->content);
			break;
		case Mdc_Token_Paragraph:
			fprintf(stream, "<p>%s</p>\n", itr->content);
			break;
		case Mdc_Token_Bold:
			fprintf(stream, "<strong>%s</strong>\n", itr->content);
			break;
		case Mdc_Token_Italic:
			fprintf(stream, "<em>%s</em>\n", itr->content);
			break;
		case Mdc_Token_ListItem:
			fprintf(stream, "<li>%s</li>\n", itr->content);
			break;
		case Mdc_Token_Quote:
			fprintf(stream, "<blockquote>%s</blockquote>\n", itr->content);
			break;
		case Mdc_Token_Hrule:
			fprintf(stream, "<hr/>\n");
			break;
		case Mdc_Token_Text:
			fprintf(stream, "<p>%s</p>\n", itr->content);
			break;
        }
		itr = itr->next;
    }
}

Mdc_TokenList *Mdc_TokenizeFile(char *filename) {
    FILE *file = fopen(filename, "r");
    if (!file) {
        printf("Error (Mdc): Could not open file `%s`\n", filename);
        exit(1);
    }
    char line[1024];
	char *buffer = (char *)malloc(sizeof(char)*BUFFER_CAPACITY);
	Mdc_TokenList *list = NULL;
    while (fgets(line, sizeof(line), file)) {
        const char *ptr = line;		
		while (*ptr) {
			int len = 0, type = 0;
			if (*ptr == '#') {
				ptr++;
				while (*ptr && *ptr != '\n') buffer[len++] = *ptr++;
				type = Mdc_Token_Heading;
			} else if (*ptr == '*' && *(ptr+1) == '*') {
				ptr += 2;
				while (*ptr && !(*ptr == '*' && *(ptr+1) == '*'))
					buffer[len++] = *ptr++;
				ptr += 2;
				type = Mdc_Token_Bold;
			} else if (*ptr == '*') {
				ptr++;
				while (*ptr && !(*ptr == '*')) buffer[len++] = *ptr++;
				ptr++;
				type = Mdc_Token_Italic;
			} else if (*ptr == '-' && *(ptr+1) == '-' && *(ptr+2) == '-') {
				ptr += 3;
				type = Mdc_Token_Hrule;
			} else if (*ptr == '>') {
				ptr++;
				while (*ptr && *ptr != '\n') buffer[len++] = *ptr++;
				ptr++;
				type = Mdc_Token_Quote;
			} else if (*ptr == '-') {
				ptr++;
				while (*ptr && *ptr != '\n') buffer[len++] = *ptr++;
				ptr++;
				type = Mdc_Token_ListItem;
			} else {
				while (*ptr && *ptr != '\n') buffer[len++] = *ptr++;
				ptr++;
				type = Mdc_Token_Text;
			}
			if (len > 0) {
				buffer[len] = 0;
				Mdc_TrimWhiteSpace(buffer);
				Mdc_TokenList_Add(&list, type, buffer);
			}
			if (*ptr == '\n') ptr++;
		}
    }
    fclose(file);
	return list;
}

#endif // MDC_H_
