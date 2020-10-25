#include "lib.h"
#include "TQueue.h"
#include "fun.h"

//Oprea-Groza Gabriel
//313CB


//Functie ce sterge toate spatiile albe de la inceputul sirului str
void deleteStartSpaces(char* str)
{
    int len = strlen(str);
    int i;
    //parcurg pana la primul non - spatiu
    for(i = 0 ; i < len ; i++)
        if(str[i] != ' ') break;

    //trunchiez zona cu spatii albe
    memmove(str, str + i, strlen(str + i) + 1);
}

//Functie ce sterge spatiile albe de la sfarsit de sir
void deleteEndSpaces(char* str)
{
    int len = strlen(str);
    int i;
    for(i = len - 1; i >= 0; i--)
        if(str[i] != ' ') break;

    str[i + 1] = '\0';
}

//Functie ce sterge nodul care are id-ul dat ca parametru
/*Prin efect lateral intoarce prin parametrul verif daca s-a gasit un astfel
de nod */
void deleteId(TArb root, char* id, int level, int* verif)
{
    *verif = 0;
    if(root == NULL)
        return;

    TArb ant = NULL;
    TArb crtNode = root->firstChild;

    while(crtNode != NULL)
    {

        /*Obtin subsirurile ce indica partea din ID corespunzatoare
        nivelului curent */
        char* p = crtNode->info->id;
        char* q = id;
        p = p + getCrtIndex(p, level);
        q = q + getCrtIndex(q, level);

        //Daca valorile sunt comune, apelez recursiv
        if(atoi(p) == atoi(q))
        {
            //Daca am gasit nodul,il sterg la fel ca la liste simplu inlantuite
            if(!strcmp(crtNode->info->id, id))
            {
                *verif = 1;
                if(ant != NULL)
                    ant->nextSibling = crtNode->nextSibling;
                else
                    root->firstChild = root->firstChild->nextSibling;

                freeTree(&crtNode->firstChild);
                freeNode(&crtNode);

                //Refac id-urile incepand de la nodul parinte
                fixId(root, level + 1);
                break;
            }
            if(*verif == 0)
                deleteId(crtNode, id, level + 1, verif);
        }
        crtNode = crtNode->nextSibling;
    }
}

/*Functie ce primeste radacina arborelui, un nume, si o functie de verificare
si verifica daca se verifica conditia impusa de functia de verificare, in
raport cu nodul curent si numele */
int deleteCond(TArb root, char* Name, FCheck check)
{
    //Pentru a parcurge BFS, folosesc o coada
    void* queue = InitQ(sizeof(TArb));
    if(!queue) exit(ERROR);

    //Aloc memorie pentru un element din coada
    TArb* queueEl = calloc(1, sizeof(TArb));
    if(!queueEl) exit(ERROR);
    *queueEl = calloc(1, sizeof(TNodArb));
    if(!*queueEl) exit(ERROR);

    //Fac o copie a adresei alocate
    TArb copy = *queueEl;

    //Pun radacina in coada
    *queueEl = root;
    Enqueue(queue, queueEl);

    int verif = 0;

    while(!VidaQ(queue))
    {
        Dequeue(queue, queueEl);

        //Pentru fiecare element din coada, parcurg nodurile copil
        TArb ant = NULL;
        TArb p = (*queueEl)->firstChild;

        while(p != NULL)
        {
            //daca se verifica conditia, elimin nodul curent din arbore
            if(check(p, Name))
            {
                verif = 1;
                //tratez separat cazul in care elementul este primul copil
                if(p == (*queueEl)->firstChild)
                {
                    TArb aux = p;
                    (*queueEl)->firstChild=(*queueEl)->firstChild->nextSibling;
                    p = p->nextSibling;
                    freeTree(&aux);
                    continue;
                }
                else
                {
                    TArb aux = p;
                    ant->nextSibling = p->nextSibling;
                    p = p->nextSibling;
                    freeTree(&aux);
                    continue;
                }
            }
            /*Aloc inca un element al cozii a carui valoare o setez
            la nodul curent */
            TArb* queueEl2 = calloc(1, sizeof(TArb));
            if(!queueEl2) exit(ERROR);
            *queueEl2 = calloc(1, sizeof(TNodArb));
            if(!*queueEl2) exit(ERROR);

            TArb copy = *queueEl2;
            *queueEl2 = p;

            //Pun nodul curent in coada
            Enqueue(queue, queueEl2);
            free(copy);
            free(queueEl2);
            ant = p;
            p = p->nextSibling;
        }
    }
    /*Eliberez memoria pentru copia facuta anterior si
    pentru elementele alocate */
    (*queueEl)->firstChild= NULL;
    free(copy);
    free(queueEl);
    free(queue);
    return verif;
}

/*Functie ce sterge din arbore nodurile care au numele si tipul date
ca parametru */
int deleteClassType(TArb root, char* className, char* typeName)
{
    void* queue = InitQ(sizeof(TArb));
    if(!queue) exit(ERROR);

    TArb* queueEl = calloc(1, sizeof(TArb));
    if(!queueEl) exit(ERROR);
    *queueEl = calloc(1, sizeof(TNodArb));
    if(!*queueEl) exit(ERROR);

    *queueEl = root;
    TArb copy = *queueEl;

    Enqueue(queue, queueEl);
    int verif = 0;

    while(!VidaQ(queue))
    {
        Dequeue(queue, queueEl);

        TArb ant = NULL;
        TArb p = (*queueEl)->firstChild;

        while(p != NULL)
        {
            //Verific atat conditiile de clasa, cat si de tip
            if(checkClass(p, className) && checkType(p, typeName))
            {
                verif = 1;
                if(p == (*queueEl)->firstChild)
                {
                    TArb aux = p;
                    (*queueEl)->firstChild=(*queueEl)->firstChild->nextSibling;
                    p = p->nextSibling;
                    freeTree(&aux);
                    continue;
                }
                else
                {
                    TArb aux = p;
                    ant->nextSibling = p->nextSibling;
                    p = p->nextSibling;
                    freeTree(&aux);
                    continue;
                }
            }
            TArb* queueEl2 = calloc(1, sizeof(TArb));
            if(!queueEl2) exit(ERROR);
            *queueEl2 = calloc(1, sizeof(TNodArb));
            if(!*queueEl2) exit(ERROR);

            TArb copy = *queueEl2;
            *queueEl2 = p;

            Enqueue(queue, queueEl2);
            free(copy);
            free(queueEl2);
            ant = p;
            p = p->nextSibling;
        }
    }
    (*queueEl)->firstChild= NULL;
    free(copy);
    free(queueEl);
    free(queue);
    return verif;
}

/*Functie care sterge nodurile care au tipul typeName si tipul parintelor
parentType */
int deleteParentType(TArb root, char* parentType, char* typeName)
{
    void* queue = InitQ(sizeof(TArb));
    if(!queue) exit(ERROR);

    TArb* queueEl = calloc(1, sizeof(TArb));
    if(!queueEl) exit(ERROR);
    *queueEl = calloc(1, sizeof(TNodArb));
    if(!*queueEl) exit(ERROR);
    TArb copy = *queueEl;
    *queueEl = root;


    Enqueue(queue, queueEl);

    int verif = 0;

    while(!VidaQ(queue))
    {
        Dequeue(queue, queueEl);

        TArb ant = NULL;
        TArb p = (*queueEl)->firstChild;

        while(p != NULL)
        {
            //Verific ca tipul este corect pentru parinte respectiv nod curent
            if(checkType((*queueEl), parentType) && checkType(p, typeName))
            {
                verif = 1;
                if(p == (*queueEl)->firstChild)
                {
                    TArb aux = p;
                    (*queueEl)->firstChild=(*queueEl)->firstChild->nextSibling;
                    p = p->nextSibling;
                    freeTree(&aux);
                    continue;
                }
                else
                {
                    TArb aux = p;
                    ant->nextSibling = p->nextSibling;
                    p = p->nextSibling;
                    freeTree(&aux);
                    continue;
                }
            }
            TArb* queueEl2 = calloc(1, sizeof(TArb));
            if(!queueEl2) exit(ERROR);
            *queueEl2 = calloc(1, sizeof(TNodArb));
            if(!*queueEl2) exit(ERROR);
            TArb copy = *queueEl2;
            *queueEl2 = p;

            Enqueue(queue, queueEl2);
            free(copy);
            free(queueEl2);
            ant = p;
            p = p->nextSibling;
        }
    }
    free(copy);
    free(queueEl);
    free(queue);
    return verif;
}

/*Functie ce sterge toate nodurile ce au tipul typeName, ce au un stramos
ancestorType */
int deleteAncestorType(TArb root, char* ancestorType, char* typeName)
{
    void* queue = InitQ(sizeof(TArb));
    if(!queue) exit(ERROR);

    TArb* queueEl = calloc(1, sizeof(TArb));
    if(!queueEl) exit(ERROR);
    *queueEl = calloc(1, sizeof(TNodArb));
    if(!*queueEl) exit(ERROR);

    TArb copy = *queueEl;

    *queueEl = root;
    Enqueue(queue, queueEl);

    int verif = 0;

    while(!VidaQ(queue))
    {

        Dequeue(queue, queueEl);

        TArb p = (*queueEl)->firstChild;

        while(p != NULL)
        {
            //Verific daca am gasit stramosul
            if(checkType(p, ancestorType))
            {
                /*Caut si sterg nodurile cu tipul typename incepand de la nodul
                stramos */
                if(deleteCond(p, typeName, checkType))
                    verif = 1;
            }
            TArb* queueEl2 = calloc(1, sizeof(TArb));
            if(!queueEl2) exit(ERROR);
            *queueEl2 = calloc(1, sizeof(TNodArb));
            if(!*queueEl2) exit(ERROR);

            TArb copy = *queueEl2;
            *queueEl2 = p;

            Enqueue(queue, queueEl2);
            free(copy);
            free(queueEl2);

            p = p->nextSibling;
        }
    }
    free(copy);
    free(queueEl);
    free(queue);
    return verif;
}
