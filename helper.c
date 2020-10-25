#include "lib.h"
#include "TQueue.h"
#include "fun.h"

//Oprea-Groza Gabriel
//313CB


//Functie ce reface id-urile dupa o stergere
void fixId(TArb root, int level)
{
    if(root == NULL)
        return;

    TArb crtNode = root->firstChild;
    int index = 1;
    while(crtNode != NULL)
    {
        /*Pentru fiecare nod, pun intr-un buffer numarul sau de
        ordine pe acest nivel */
        char* buffer = calloc(1000, sizeof(char));
        sprintf(buffer, "%d", index);

        /*Copiez id-ul nodului parinte, apoi alipesc un '.' si nr de ordine
        al nodului curent */
        strcpy(crtNode->info->id, root->info->id);
        strcat(crtNode->info->id, ".");
        strcat(crtNode->info->id, buffer);
        free(buffer);

        //Apelez recursiv pentru nivelul inferior
        fixId(crtNode, level + 1);
        crtNode = crtNode->nextSibling;
        index++;
    }
}

/*Functe ce insereaza un atribut cu numele si valoarea dati ca parametru
in lista destinatie list */
void InsAttr(char* name, char* value, TAttr* list)
{

    //Aloc o celula auxiliara
    TAttr new = calloc(1, sizeof(TNodAttr));
    if(!new) exit(ERROR);

    //Copiez in aceasta numele si valoarea
    new->name = strdup(name);
    new->value = strdup(value);
    if(!new->name) exit(ERROR);
    if(!new->value) exit(ERROR);

    //sterg toate spatiile albe nedorite, daca exista
    deleteStartSpaces(new->name);
    deleteStartSpaces(new->value);
    deleteEndSpaces(new->name);
    deleteEndSpaces(new->value);


    TAttr p = *list;
    TAttr ant = NULL;

    /*Daca lista e vida inserez in capul listei, altfel parcurg lista
    si inserez la sfarsit */
    if(p == NULL)
        *list = new;
    else
    {
        while(p != NULL)
        {
            ant = p;
            p = p->next;
        }
        ant->next = new;
    }
}

/*Functie ce insereaza in lista un stil ce are toate subatributele
concatenate in sirul value. */
void InsStyle(char* value, TAttr* list)
{
    //Obtin numele si valoarea primului subatribut
    char* styleName = strtok(value, ":;");
    char* styleValue = strtok(NULL, ":;");

    //Inserez la sfarsitul listei de atribute
    InsAttr(styleName, styleValue, list);

    //cat timp sirul value mai are subatribuite, le prelucrez pe fiecare
    while(styleName != NULL && styleValue != NULL)
    {

        styleName = strtok(NULL, ";:");
        if(styleName == NULL) break;
        styleValue = strtok(NULL, ";:");
        InsAttr(styleName, styleValue, list);
    }
}

/*Functie ce intoarce pointer la nodul din arbore la care se gaseste id-ul
dat ca parametru */
TArb findId(TArb root, char* id, int level)
{
    if(root == NULL)
        return NULL;

    TArb crtNode = root;
    while(crtNode != NULL)
    {
        char* p = id;
        char* q = crtNode->info->id;

        p = p + getCrtIndex(p, level);
        q = q + getCrtIndex(q, level);

        if(atoi(p) == atoi(q))
        {
            if(!strcmp(crtNode->info->id, id))
                return crtNode;
            TArb parent = findId(crtNode->firstChild, id, level + 1);
            if(parent != NULL) return parent;
        }
        crtNode = crtNode->nextSibling;
    }
    return NULL;
}


/*Primeste un ID si intoarce indicele la care se gaseste valoarea ce
determina pe a cata pozitie se afla un nod pe nivelul level */
int getCrtIndex(char* id, int level)
{
    char* p = id;
    for(int i = 0; i < level; i++)
        p = strchr(id, '.');

    //returnez indicele cerut prin aritmetica de pointeri
    return p - id + 1;
}



/*Functie ce inlocuieste valoarea atributului cu numele name cu valoarea
data ca parametru value. In cazul in care atributul cu acel nume nu exista
se intoarce 0 */
int appendAttr(char* name, char* value, TAttr* list)
{
    TAttr p = *list;
    //Formatez numele atributului stergand spatiile inutile
    char* nameCopy = strdup(name);
    deleteStartSpaces(nameCopy);
    deleteEndSpaces(nameCopy);

    //Caut numele dorit
    while(p != NULL)
    {
        if(!strcmp(p->name, nameCopy))
            break;
        p = p->next;
    }
    free(nameCopy);
    if(p == NULL) return 0; //nu s-a gasit numele

    //Daca s-a gasit, distrug valoarea initiala si o inlocuiesc
    free(p->value);
    p->value = strdup(value);
    return 1;
}

/* Functia primeste un stil dat ca subatribute concatenate intr-un singur sir
de caractere si realieaza operatia de append: in cazul in care un subatribut
exista in sir, ii inlocuieste valoarea, iar in caz contrar adauga la sfarsit
unul nou */
void appendStyle(char* value, TAttr* list)
{
    char* styleName = strtok(value, ":;");
    char* styleValue = strtok(NULL, ":;");
    int verif = appendAttr(styleName, styleValue, list);
    if(verif == 0) InsAttr(styleName, styleValue, list);

    //se obtine fiecare subatribut din sir
    while(styleName != NULL && styleValue != NULL)
    {
        styleName = strtok(NULL, ";:");
        if(styleName == NULL) break;
        styleValue = strtok(NULL, ";:");

        //se verifica daca exista deja stilul cu numele styleName
        int verif = appendAttr(styleName, styleValue, list);
        if(verif == 0)
            InsAttr(styleName, styleValue, list); /*in caz contrar adaug la
            sfarsit de lista */
    }
}


//Functie ce determina daca un tag este gol(nu are atribute de orice fel)
int isEmptyTag(TArb node)
{
    if(node->info->style != NULL)
        return 0;
    if(node->info->otherAttributes != NULL)
        return 0;
    return 1;
}

/*Functie ce returneaza fiul cel mai din dreapta al nodului root si intoarce
prin efect lateral indicele la care acesta se gaseste */
TArb getLastSon(TArb root, int* index)
{
    int i = 1;
    //Parcurgerea se realizeaza asemanator cu a unei liste simplu inlantuite
    TArb crt = root->firstChild;
    TArb ant = NULL;
    while(crt != NULL)
    {
        ant = crt;
        crt = crt->nextSibling;
        i++;
    }
    *index = i;
    return ant;
}

/*Functie ce printeaza continutul nodului root formatat cu nrTabs
la stanga */
void printContent(TArb root, int nrTabs, FILE* output)
{
    for(int k = 0; k < nrTabs; k++)
        fprintf(output, "\t");

    if(root->info->contents)
    {
        fprintf(output, "%s\n", root->info->contents);
    }
}

/*Functie ce printeaza cu formatul de nrTabs la stanga un tag de inchidere,
pentru un tag care nu este self-closing */
void printClosingTag(TArb root, int nrTabs, FILE* output)
{
    for(int k = 0; k < nrTabs; k++)
        fprintf(output, "\t");

    if(root->info->type) fprintf(output, "</%s>\n", root->info->type);
}

void freeList(TAttr* listAdr)
{
    TAttr list = *listAdr;
    while(list != NULL)
    {
        TAttr aux = list;
        list = list->next;
        free(aux->name);
        free(aux->value);
        free(aux);
    }
    *listAdr = NULL;
}


//Functie ce verifica daca nodul curent are clasa cu numele dat ca parametru
int checkClass(TArb node, char* className)
{
    //parcurg lista pana intalnesc campul clasa
    TAttr p = node->info->otherAttributes;
    while(p != NULL)
    {
        if(!strcmp(p->name,"class"))
            break;
        p = p->next;
    }
    //daca nu am gasit clasa, intorc 0
    if(p == NULL) return 0;

    //daca am gasit clasa, intorc 1
    if(!strcmp(p->value, className))
        return 1;
    return 0;
}

//Functie ce verifica daca nodul curent are tipul cu numele dat ca parametru
int checkType(TArb node, char* typeName)
{
    if(!strcmp(node->info->type, typeName))
        return 1;
    return 0;
}

//Functie ce verifica daca nodul are id-ul din parametru
int checkID(TArb node, char* id)
{
    if(!strcmp(node->info->id, id))
        return 1;
    return 0;
}