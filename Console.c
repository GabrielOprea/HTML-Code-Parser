#include "lib.h"
#include "TQueue.h"
#include "fun.h"

//Oprea-Groza Gabriel
//313CB


//Verfifica daca string-ul dat contine un anumit caracter
int containsChar(char* string, char ch)
{
    int len = strlen(string);
    for(int i = 0; i < len; i++)
        if(string[i] == ch)
            return 1;
    return 0;
}

//Functie alternativa lui strtok, pentru un singur caracter
char* splitString(char* str, char ch)
{
    char* newStr = strdup(str);
    int len = strlen(str);
    for(int i = 0 ; i < len; i++)
        if(str[i] == ch)
        {
            newStr[i] = '\0';
            memmove(str, str + i + 1, strlen(str + i + 1) + 1);
            break;
        }
    return newStr;
}

//Functie ce afiseaza un mesaj de eroare, in functie de selector si de mod
void printError(FILE* output, char* mode, char* selector)
{
    if(!strcmp(mode, "deleteRecursively"))
    {
        fprintf(output, "Delete recursively failed: no node found ");
        fprintf(output, "for selector %s!\n", selector);
    }
    else if(!strcmp(mode, "appendStyle"))
    {
        fprintf(output, "Append to style failed: no node found ");
        fprintf(output, "for selector %s!\n", selector );
    }
    else if(!strcmp(mode, "overrideStyle"))
    {
        fprintf(output, "Override style failed: no node found ");
        fprintf(output, "for selector %s!\n", selector);
    }
}

//Functie ce realizeaza operatiile din sirul mode cautand in arbore dupa ID
void byID(char* partCmd, char* mode, TArb root)
{
    int verif = 0;
    char* id = strdup(partCmd + 1);
    if(!id) exit(ERROR);

    //in cazul in care se doreste stergere, apelez fct de stergere recursiva
    if(!strcmp(mode, "deleteRecursively"))
        deleteId(root, id, 0, &verif);
    else
    {
        partCmd = strtok(NULL, " ="); //style
        partCmd = strtok(NULL, "\"");
        //obtin stilul din comanda
        char* style = strdup(partCmd);
        deleteEndSpaces(style); //sterg spatiile nedorite daca exista

        if(!strcmp(mode, "appendStyle") || !strcmp(mode, "overrideStyle"))
            overrideId(root, id, style, 0, mode, &verif);
        free(style);
    }
    free(id);
}

int byClass(char* partCmd, char* mode, TArb root)
{
    int verif = 0;
    char* className = strdup(partCmd + 1);
    if(!className) exit(ERROR);

    if(!strcmp(mode, "deleteRecursively"))
        verif = deleteCond(root, className, checkClass);
    else
    {
        partCmd = strtok(NULL, " ="); //style
        partCmd = strtok(NULL, "\"");
        char* style = strdup(partCmd);
        deleteEndSpaces(style);

        if(!strcmp(mode, "appendStyle") || !strcmp(mode, "overrideStyle"))
            verif = overrideCond(root, className, checkClass, style, mode);
        free(style);
    }
    free(className);
    return verif;
}

int byParent(char* partCmd, char* mode, TArb root)
{

    int verif = 0;
    char* parentType = splitString(partCmd, '>');
    if(!parentType) exit(ERROR);

    char* typeName = strdup(partCmd);
    if(!typeName) exit(ERROR);

    if(!strcmp(mode, "deleteRecursively"))
        verif = deleteParentType(root, parentType, typeName);
    else
    {
        partCmd = strtok(NULL, " ="); //style
        partCmd = strtok(NULL, "\"");
        char* style = strdup(partCmd);
        deleteEndSpaces(style);

        if(!strcmp(mode, "appendStyle") || !strcmp(mode, "overrideStyle"))
            verif = overrideParentType(root, parentType, typeName, style,mode);
        free(style);
    }
    free(parentType); free(typeName);
    return verif;
}

int byAncestor(char* partCmd, char* mode, TArb root)
{
    int verif = 0;
    char* ancestorType = splitString(partCmd, ' ');
    if(!ancestorType) exit(ERROR);

    char* typeName = strdup(partCmd);
    if(!typeName) exit(ERROR);

    if(!strcmp(mode, "deleteRecursively"))
        verif = deleteAncestorType(root, ancestorType, typeName);
    else
    {
        partCmd = strtok(NULL, " ="); //style
        partCmd = strtok(NULL, "\"");
        char* style = strdup(partCmd);
        deleteEndSpaces(style);

        if(!strcmp(mode, "appendStyle") || !strcmp(mode, "overrideStyle"))
            verif = overrideAncestorType(root, ancestorType, typeName,
            style, mode);
        free(style);
    }
    free(ancestorType); free(typeName);
    return verif;
}

int byTypeClass(char* partCmd, char* mode, TArb root)
{
    int verif = 0;
    char* typeName = splitString(partCmd, '.');
    if(!typeName) exit(ERROR);
    char* className = strdup(partCmd);
    if(!className) exit(ERROR);

    if(!strcmp(mode, "deleteRecursively"))
        verif = deleteClassType(root, className, typeName);
    else
    {
        partCmd = strtok(NULL, " ="); //style
        partCmd = strtok(NULL, "\"");
        char* style = strdup(partCmd);
        deleteEndSpaces(style);

        if(!strcmp(mode, "appendStyle") || !strcmp(mode, "overrideStyle"))
            verif = overrideClassType(root, className, typeName, style, mode);
        free(style);
    }
    free(typeName); free(className);

    return verif;
}

int byType(char* partCmd, char* mode, TArb root)
{

    int verif = 0;
    char* typeName = strdup(partCmd);
    if(!typeName) exit(ERROR);

    if(!strcmp(mode, "deleteRecursively"))
        verif = deleteCond(root, typeName, checkType);
    else
    {
        partCmd = strtok(NULL, " ="); //style
        partCmd = strtok(NULL, "\"");
        char* style = strdup(partCmd);
        deleteEndSpaces(style);

        if(!strcmp(mode, "appendStyle") || !strcmp(mode, "overrideStyle"))
            verif = overrideCond(root, typeName, checkType, style, mode);
        free(style);
    }
    free(typeName);
    return verif;
}

void freeAll(TArb* root, FILE** input, FILE** output, FILE** html)
{
    freeTree(root);
    fclose(*input);
    fclose(*output);
    fclose(*html);
}

/*Functie ce va citi din fisierele cu numele aferente si va apela
functiile cerute */
void Console(char* fileName, char* commands, char* outFile)
{

    FILE* html = fopen(fileName, "r");
    if(!html) exit(ERROR);
    FILE* input = fopen(commands, "r");
    if(!input) exit(ERROR);
    FILE* output = fopen(outFile, "w");
    if(!output) exit(ERROR);

    int nrCmd;
    fscanf(input, "%d\n", &nrCmd);

    TArb root = alocTree();
    if(!root) exit(ERROR);
    buildRoot(root, html);
    char c = NOT_READ;
    char* tempName = calloc(INFO_SIZE, sizeof(char));
    if(!tempName) exit(ERROR);
    char* tempValue = calloc(INFO_SIZE, sizeof(char));
    if(!tempValue) exit(ERROR);

    //construiesc arborele citind din fisierul html
    buildTree(root, PARSE_CONTENTS, html, c, tempName, tempValue);

    free(tempName); free(tempValue);

    for(int i = 0; i < nrCmd; i++)
    {
        //citesc nrCmd comenzi din fisier
        char* cmd = calloc(CMD_SIZE, sizeof(char));
        if(!cmd) exit(ERROR);
        fgets(cmd, CMD_SIZE, input);
        cmd[strcspn(cmd, "\n")] = '\0'; //elimin '\n' din comanda

        char* partCmd = strtok(cmd, " =");

        //in functie de primul cuvant din comanda, parsez corespunz
        if(!strcmp(partCmd, "format"))
            printMultiTree(root, 0, output);
        if(!strcmp(partCmd, "add"))
        {
            partCmd = strtok(NULL, " ="); //id
            partCmd = strtok(NULL, " =");
            char* id = strdup(partCmd);
            if(!id) exit(ERROR);
            partCmd = strtok(NULL, " ="); //tag
            partCmd = strtok(NULL, "\"");
            char* tag = strdup(partCmd);
            add(root, id, tag, output);
            free(tag); free(id);
        }
        else if(!strcmp(partCmd, "appendStyle") || !strcmp(partCmd,
        "deleteRecursively") || !strcmp(partCmd, "overrideStyle"))
        {
            char* mode = strdup(partCmd);
            partCmd = strtok(NULL, " ="); //selector
            partCmd = strtok(NULL, "\"");
            char* selector = strdup(partCmd);
            int verif = 1;
            if(partCmd[0] == '#')
                byID(partCmd, mode, root);
            else if(partCmd[0] == '.')
                verif = byClass(partCmd, mode, root);
            else if(containsChar(partCmd, '>'))
                verif = byParent(partCmd, mode, root);
            else if(containsChar(partCmd, ' '))
                verif = byAncestor(partCmd, mode, root);
            else if(containsChar(partCmd, '.'))
                verif = byTypeClass(partCmd, mode, root);
            else //cauta tag-urile dupa tip
                verif = byType(partCmd, mode, root);
            if(!verif)
                printError(output, mode, selector);

            free(selector); free(mode);
        }
        free(cmd);
    }
    freeAll(&root, &input, &output, &html);
}