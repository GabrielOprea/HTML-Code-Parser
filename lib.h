#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

//Oprea-Groza Gabriel
//313CB

#define ERROR 1
#define INFO_SIZE 1000
#define CMD_SIZE 1024
#define NOT_READ -1

typedef struct TNodAttr {
    char *name;
    char *value;
    struct TNodAttr *next;
} TNodAttr, *TAttr;

typedef struct TNodInfo {
    char *type;
    char *id;
    TAttr style;
    TAttr otherAttributes;
    int isSelfClosing;
    char *contents;
} TNodInfo, *TInfo;

typedef struct TNodArb {
    TInfo info;
    struct TNodArb *nextSibling;
    struct TNodArb *firstChild;
} TNodArb, *TArb;

typedef int (*FCheck)(TArb, char*);


typedef enum
{
    PARSE_ERROR             = 0,
    PARSE_CONTENTS          = 1,
    PARSE_OPENING_BRACKET   = 2,
    PARSE_TAG_TYPE          = 3,
    PARSE_CLOSING_TAG       = 4,
    PARSE_REST_OF_TAG       = 5,
    PARSE_ATTRIBUTE_NAME    = 6,
    PARSE_ATTRIBUTE_EQ      = 7,
    PARSE_ATTRIBUTE_VALUE   = 8,
    PARSE_SELF_CLOSING      = 9,
} TParseState;
