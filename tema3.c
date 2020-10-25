#include "lib.h"
#include "TQueue.h"
#include "fun.h"

char* strdup(char* str)
{
    char* newstr = calloc(strlen(str) + 1, sizeof(char));
    if(!newstr) return NULL;
    strcpy(newstr, str);
    return newstr;
}

/* Functie ce printeaza in fisierul de iesire formatat toate informatiile
dintr-un nod din arbore, adica un tag, formatat cu numarul de taburi necesar */
void format(TArb root, int nrTabs, FILE* output)
{
    //printeaza mai intai tab-urile in fisier
    for(int k = 0; k < nrTabs; k++)
        fprintf(output, "\t");

    //printez tag-ul de deschidere
    fprintf(output, "<%s",root->info->type);

    if(!isEmptyTag(root)) fprintf(output, " ");

    //Parcurg lista de stiluri si afisez fiecare subatribut
    TAttr p = root->info->style;
    if(p != NULL) fprintf(output, "style=\"");
    while(p != NULL)
    {

        int len = strlen(p->value);

        fprintf(output, "%s: ",p->name);

        /*Nu afisez si spatiile albe din campul valorii pentru
        un subatribut de style */
        for(int i = 0; i < len; i++)
            if(p->value[i] != ' ')
                fprintf(output, "%c", p->value[i]);

        fprintf(output,";");

        p = p->next;
        if(p != NULL) fprintf(output, " ");
    }

    if(root->info->style != NULL)fprintf(output, "\"");

    //Parcurg lista de otherAttributes si afisez cu formatul corespunzator
    p = root->info->otherAttributes;
    if(p != NULL && root->info->style != NULL) fprintf(output, " ");
    while(p != NULL)
    {
        fprintf(output,"%s=\"%s\"", p->name, p->value);
        p = p->next;
        if(p != NULL)
            fprintf(output, " ");
    }

    if(root->info->isSelfClosing) fprintf(output, "/");

    fprintf(output, ">");
    fprintf(output, "\n");

    /*Daca sirul de contents nu este gol, afisez continutul tag-ului
    cu un tab in plus */
    if(root->info->contents[0] != '\0')
        printContent(root, nrTabs+1, output);
}

//Functie ce afiseaza recursiv fiecare nod dintr-un arbore multicai, cu format
void printMultiTree(TArb root, int level, FILE* output)
{
    if(root == NULL)
        return;

    //cu crtNode voi itera prin toate nodurile de pe nivelul curent
    TArb crtNode = root;
    while(crtNode != NULL)
    {
        //afisez nodul curent, apoi reapelez recursiv pentru primul copil
        format(crtNode, level, output);
        printMultiTree(crtNode->firstChild, level + 1, output);

        /*daca tag-ul nu este selfClosing, va trebui sa afisez la final si
        inchiderea acestui tag */
        if(!crtNode->info->isSelfClosing)
            printClosingTag(crtNode, level, output);

        crtNode = crtNode->nextSibling;
    }
}

/*Functie ce adauga un nou nod copil cu informatii din tag cu parintele ce
are id-ul corespunzator */
void add(TArb root, char* id, char* tag, FILE* output)
{
    //Caut nodul cu id-ul cerut
    TArb parent = findId(root->firstChild, id, 0);
    if(!parent)
    {
        fprintf(output, "Add tag failed: node with id %s not found!\n", id);
        return;
    }

    ///Dupa ce l-am gasit, construiesc un nou nod cu info din sirul tag
    TArb newNode = buildNode(tag);

    deleteStartSpaces(newNode->info->contents);
    deleteEndSpaces(newNode->info->contents);

    int index = 0;
    TArb lastSon = getLastSon(parent, &index);

    //Construiesc id-ul nodului creat
    char* buffer = calloc(1000, sizeof(char));
    if(!buffer) exit(ERROR);
    sprintf(buffer, "%s.%d", parent->info->id, index);
    strcpy(newNode->info->id,buffer);

    //Daca nodul parinte nu are copii, il adaug ca prim copil
    if(lastSon != NULL)
        lastSon->nextSibling = newNode;
    else parent->firstChild = newNode;
    free(buffer);
}

int main(int argc, char* argv[])
{
    /*preiau numele fisierului de cod, comenzi respectiv output
    si apelez consola */
    if(argc != 4)
        printf("Not enough arguments");

    char* fileName = argv[1];
    char* commands = argv[2];
    char* outFile = argv[3];
    Console(fileName, commands, outFile);

    return 0;
}
