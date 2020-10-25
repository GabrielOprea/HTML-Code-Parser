#include "lib.h"
#include "TQueue.h"
#include "fun.h"

//Aloca memorie pentru un nod din arbore
TArb alocTree()
{
    TArb new = calloc(1, sizeof(TNodArb));
    if(!new) return NULL;
    new->nextSibling = NULL;
    new->firstChild = NULL;

    new->info = calloc(1, sizeof(TNodInfo));
    if(!new->info)
    {
        free(new);
        return NULL;
    }

    new->info->isSelfClosing = 0;
    new->info->contents = calloc(INFO_SIZE, sizeof(char));
    if(!new->info->contents)
    {
        free(new->info);
        free(new);
        return NULL;
    }
    new->info->type = calloc(INFO_SIZE, sizeof(char));
    if(!new->info->type)
    {
        free(new->info->contents);
        free(new->info);
        free(new);
        return NULL;
    }
    new->info->id = calloc(INFO_SIZE, sizeof(char));
    if(!new->info->id)
    {
        free(new->info->contents);
        free(new->info->type);
        free(new->info);
        free(new);
        return NULL;
    }
    new->info->style = NULL;
    new->info->otherAttributes = NULL;
    return new;
}


/*Functie ce construieste radacina arborelui citind si parsand din fisierul
html doar primul tag */
void buildRoot(TArb root, FILE* html)
{
    TParseState nextState = PARSE_ERROR;
    TParseState crtState = PARSE_CONTENTS;

    char* tempValue = calloc(INFO_SIZE, sizeof(char));
    char* tempName = calloc(INFO_SIZE, sizeof(char));
    if(!tempName || !tempValue) exit(ERROR);

    char c = fgetc(html);
    //parsez pana intalnesc caracterul terminator de tag '>'
    while(c != '>')
    {
        switch (crtState)
        {
        case PARSE_CONTENTS:
            if (c == '<')           {   nextState = PARSE_OPENING_BRACKET;  }
            else                    {   nextState = PARSE_CONTENTS;
                                        strncat(root->info->contents,&c, 1);}
            break;
        case PARSE_OPENING_BRACKET:
            if (isspace(c))         {   nextState = PARSE_OPENING_BRACKET;  }
            else if (c == '>')      {   nextState = PARSE_ERROR;            }
            else if (c == '/')      {   nextState = PARSE_CLOSING_TAG;      }
            else                    {   nextState = PARSE_TAG_TYPE;
                                        if(c != ' ')
                                        strncat(root->info->type, &c, 1);   }
            break;
        case PARSE_TAG_TYPE:
            if (isspace(c))         {   nextState = PARSE_REST_OF_TAG;      }
            else if (c == '>')      {   nextState = PARSE_CONTENTS;         }
            else                    {   nextState = PARSE_TAG_TYPE;
                                        if(c != ' ')
                                        strncat(root->info->type, &c, 1);   }
            break;
        case PARSE_CLOSING_TAG:
            if (c == '>')           {   nextState = PARSE_CONTENTS; }

            else                    {   nextState = PARSE_CLOSING_TAG;      }
            break;
        case PARSE_REST_OF_TAG:
            if (isspace(c))         {   nextState = PARSE_REST_OF_TAG;      }
            else if (c == '>')      {   nextState = PARSE_CONTENTS;         }
            else if (c == '/')      {   nextState = PARSE_SELF_CLOSING;     }
            else                    {   nextState = PARSE_ATTRIBUTE_NAME;
                                        if(c != ' ')
                                        strncat(tempName, &c, 1); }
            break;
        case PARSE_ATTRIBUTE_NAME:
            if (c == '=')           {   nextState = PARSE_ATTRIBUTE_EQ;     }
            else                    {   nextState = PARSE_ATTRIBUTE_NAME;
                                        if(c != ' ')
                                        strncat(tempName, &c, 1);           }
            break;
        case PARSE_ATTRIBUTE_EQ:
            if (c == '\"')          {   nextState = PARSE_ATTRIBUTE_VALUE;  }
            break;
        case PARSE_ATTRIBUTE_VALUE:
            if (c == '\"')          {   nextState = PARSE_REST_OF_TAG;
                                        InsAttr(tempName, tempValue,
                                        &root->info->otherAttributes);      }

            else                    {   nextState = PARSE_ATTRIBUTE_VALUE;
                                        strncat(tempValue, &c, 1);}
            break;
        case PARSE_SELF_CLOSING:
            if (c == '>')           {   nextState = PARSE_CONTENTS;         }

            else                    {   nextState = PARSE_ERROR;            }
            break;
        default:
            break;
        }
        crtState = nextState;
        c = fgetc(html);
    }

    free(tempValue); free(tempName); //eliberez zonele de memorie auxiliare
}

void alocFirstChild(TArb root, char* buffer)
{
    root->firstChild = alocTree();
    strcpy(root->firstChild->info->id, root->info->id);
    if(root->info->id[0] != '\0')
    strncat(root->firstChild->info->id, ".", sizeof(char));
    strncat(root->firstChild->info->id, buffer, strlen(buffer));
}

void alocNextSibling(TArb root, TArb crtSon, char* buffer)
{
    crtSon->nextSibling = alocTree();
    strcpy(crtSon->nextSibling->info->id, root->info->id);
    if(root->info->id[0] != '\0')
        strncat(crtSon->nextSibling->info->id, ".", sizeof(char));
    strncat(crtSon->nextSibling->info->id, buffer, strlen(buffer));
}

/* Functie ce este o varianta modificata a celei de Interpret, ce va construi
arborele citind caracter cu caracter din fisierul html */
void buildTree(TArb root, TParseState crtState, FILE* html, char c,
char* tempValue, char* tempName)
{

    TParseState nextState = PARSE_ERROR;
    if (c == NOT_READ) c = fgetc(html); //in cazul unui caracter inexistent

    while(c != EOF)
    {
        switch (crtState)
        {
        case PARSE_CONTENTS:
            if (c == '<')           {   nextState = PARSE_OPENING_BRACKET;  }
            else                    {   nextState = PARSE_CONTENTS;
                                        if(c!='\n')
                                        strncat(root->info->contents, &c,1);}
            break;
        case PARSE_OPENING_BRACKET:
            if (isspace(c))         {   nextState = PARSE_OPENING_BRACKET;  }
            else if (c == '>')      {   nextState = PARSE_ERROR;            }
            else if (c == '/')      {   nextState = PARSE_CLOSING_TAG;      }
            else                    {//aloc memorie pentru un copil si setez id
                nextState = PARSE_TAG_TYPE;
                char* buffer = calloc(INFO_SIZE, sizeof(char));
                if(!buffer) exit(ERROR);
                int index = 0;
                TArb crtSon = getLastSon(root, &index);
                sprintf(buffer, "%d", index);

                if(crtSon == NULL) {
                    alocFirstChild(root, buffer);
                    buildTree(root->firstChild, nextState, html, c,
                    tempValue, tempName);
                }
                else {
                    alocNextSibling(root, crtSon, buffer);
                    buildTree(crtSon->nextSibling, nextState, html, c,
                    tempValue, tempName);
                }
                nextState = PARSE_CONTENTS;
                free(buffer);
                                    }
            break;
        case PARSE_TAG_TYPE:
            if (isspace(c))         {   nextState = PARSE_REST_OF_TAG;      }
            else if (c == '>')      {   nextState = PARSE_CONTENTS;         }
            else                    {   nextState = PARSE_TAG_TYPE;
                                        if(c!='\n' && c!=' ')
                                            strncat(root->info->type, &c,1);}
            break;
        case PARSE_CLOSING_TAG:
            if (c == '>')           {   nextState = PARSE_CONTENTS;
                                        root->info->isSelfClosing = 0; return;}
            else                    {   nextState = PARSE_CLOSING_TAG;      }
            break;
        case PARSE_REST_OF_TAG:
            if (isspace(c))         {   nextState = PARSE_REST_OF_TAG;      }
            else if (c == '>')      {   nextState = PARSE_CONTENTS;         }
            else if (c == '/')      {   nextState = PARSE_SELF_CLOSING;     }
            else                    {   nextState = PARSE_ATTRIBUTE_NAME;
                                        if(c!='\n')
                                            strncat(tempName, &c, 1);       }
            break;
        case PARSE_ATTRIBUTE_NAME:
            if (c == '=')           {   nextState = PARSE_ATTRIBUTE_EQ;     }
            else                    {   nextState = PARSE_ATTRIBUTE_NAME;
                                        if(c!='\n')
                                            strncat(tempName, &c, 1); }
            break;
        case PARSE_ATTRIBUTE_EQ:
            if (c == '\"')          {   nextState = PARSE_ATTRIBUTE_VALUE;  }
            break;
        case PARSE_ATTRIBUTE_VALUE:
            if (c == '\"')          {
                nextState = PARSE_REST_OF_TAG;
                if(!strcmp(tempName, "style"))
                    InsStyle(tempValue, &root->info->style);
                else
                    InsAttr(tempName, tempValue, &root->info->otherAttributes);
                memset(tempName, '\0', INFO_SIZE * sizeof(char));
                memset(tempValue, '\0', INFO_SIZE * sizeof(char));
                                    }

            else                    {   nextState = PARSE_ATTRIBUTE_VALUE;
                                        strncat(tempValue, &c, 1);          }
            break;
        case PARSE_SELF_CLOSING:
            if (c == '>')           {   nextState = PARSE_CONTENTS;
                                        root->info->isSelfClosing = 1; return;}
            else                    {   nextState = PARSE_ERROR;            }
            break;
        default:
            break;
        }
        crtState = nextState;
        c = fgetc(html);
        while(c == '\n' || c =='\r' || c == '\t') c = fgetc(html);
    }
}

/*Functie ce construieste un nod parsand informatiile din sirul de caractere
primit ca parametru(asemanatoare cu cea de construire a radacinii, doar ca
citirea se face dintr-un sir) */
TArb buildNode(char* tag)
{
    TParseState nextState = PARSE_ERROR;
    TParseState crtState = PARSE_CONTENTS;
    char* tempValue = calloc(INFO_SIZE, sizeof(char));
    if(!tempValue) exit(ERROR);
    char* tempName = calloc(INFO_SIZE, sizeof(char));
    if(!tempName) exit(ERROR);

    TArb root = alocTree();
    if(!root) return NULL;

    char c = tag[0];
    int i = 0;
    while(c != '\0')
    {
        switch (crtState)
        {
        case PARSE_CONTENTS:
            if (c == '<')           {   nextState = PARSE_OPENING_BRACKET;  }
            else                    {   nextState = PARSE_CONTENTS;
                                        strncat(root->info->contents, &c, 1);}
            break;
        case PARSE_OPENING_BRACKET:
            if (isspace(c))         {   nextState = PARSE_OPENING_BRACKET;  }
            else if (c == '>')      {   nextState = PARSE_ERROR;            }
            else if (c == '/')      {   nextState = PARSE_CLOSING_TAG;      }
            else                    {   nextState = PARSE_TAG_TYPE;
                                        strncat(root->info->type, &c, 1);   }
            break;
        case PARSE_TAG_TYPE:
            if (isspace(c))         {   nextState = PARSE_REST_OF_TAG;      }
            else if (c == '>')      {   nextState = PARSE_CONTENTS;         }
            else                    {   nextState = PARSE_TAG_TYPE;
                                        strncat(root->info->type, &c, 1);   }
            break;
        case PARSE_CLOSING_TAG:
            if (c == '>')           {   nextState = PARSE_CONTENTS; }
            else                    {   nextState = PARSE_CLOSING_TAG;      }
            break;
        case PARSE_REST_OF_TAG:
            if (isspace(c))         {   nextState = PARSE_REST_OF_TAG;      }
            else if (c == '>')      {   nextState = PARSE_CONTENTS;         }
            else if (c == '/')      {   nextState = PARSE_SELF_CLOSING;     }
            else                    {   nextState = PARSE_ATTRIBUTE_NAME;
                                        strncat(tempName, &c, 1);           }
            break;
        case PARSE_ATTRIBUTE_NAME:
            if (c == '=')           {   nextState = PARSE_ATTRIBUTE_EQ;     }
            else                    {   nextState = PARSE_ATTRIBUTE_NAME;
                                        strncat(tempName, &c, 1); }
            break;
        case PARSE_ATTRIBUTE_EQ:
            if (c == '\"')          {   nextState = PARSE_ATTRIBUTE_VALUE;  }
            break;
        case PARSE_ATTRIBUTE_VALUE:
            if (c == '\"')          {   nextState = PARSE_REST_OF_TAG;
                                    if(!strcmp(tempName, "style"))
                                        InsStyle(tempValue, &root->info->style);
                                    else
                                        InsAttr(tempName, tempValue,
                                        &root->info->otherAttributes);      }
            else                    {   nextState = PARSE_ATTRIBUTE_VALUE;
                                        strncat(tempValue, &c, 1);}
            break;
        case PARSE_SELF_CLOSING:
            if (c == '>')           {   nextState = PARSE_CONTENTS;         }
            else                    {   nextState = PARSE_ERROR;            }
            break;
        default:
            break;
        }
        crtState = nextState;
        i++;
        c = tag[i]; //obtin urmatorul caracter din sir
    }
    free(tempValue);
    free(tempName);
    return root;
}
/*Elibereaza memorie pentru un nod din arbore si seteaza zonele
eliberate la NULL */
void freeNode(TArb* nodeAdr)
{
    TArb node = *nodeAdr;
    TAttr styleList = node->info->style;
    freeList(&styleList);

    TAttr otherList = node->info->otherAttributes;
    freeList(&otherList);

    free(node->info->contents);
    free(node->info->type);
    free(node->info->id);
    free(node->info);

    node->firstChild = NULL;
    node->nextSibling = NULL;
    free(node);
    *nodeAdr = NULL;
}


/*Elibereaza recursiv memorie pentru toate nodurile pe niveluri mai mari sau
egale cu nodul a carui adresa este rootAdr */
void freeTree(TArb* rootAdr)
{
    TArb root = *rootAdr;
    if(root == NULL)
        return;

    TArb crtNode = root;
    while(crtNode != NULL)
    {
        //Reapelez recursiv pentru copiii nivelului curent
        freeTree(&crtNode->firstChild);
        //Elimin si eliberez nodul curent
        TArb aux = crtNode;
        crtNode = crtNode->nextSibling;
        freeNode(&aux);
    }
}