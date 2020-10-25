#include "lib.h"
#include "TQueue.h"
#include "fun.h"


/*Aceste functii pot realiza atat operatiile de overrideStyle, cat si cele de
appendStyle. Variabila care indica ce tip de prelucare va fi facuta este
sirul de caractere mode */

/*Fiecare functie va cauta in arbore dupa un anumit criteriu(ID, clasa, tip...)
si va realiza operatia ceruta . Modul lor de functionare este asemanator cu
cele de stergere*/


/*Prin efect lateral se intoarce verif, care este 1 daca s-a gasit un nod cu ID
cerut, 0 in caz contrar */
void overrideId(TArb root, char* id, char* newStyle, int level, char* mode,
int* verif)
{
    *verif = 0;
    if(root == NULL)
        return;


    TArb crtNode = root->firstChild;

    while(crtNode != NULL)
    {
        char* p = id;
        char* q = crtNode->info->id;
        p = p + getCrtIndex(p, level);
        q = q + getCrtIndex(q, level);
        if(atoi(p) == atoi(q))
        {
            if(!strcmp(crtNode->info->id, id))
            {
                *verif = 1;
                char* styleCopy = strdup(newStyle);
                if(!strcmp(mode, "overrideStyle"))
                {
                    /*daca trebuie sa efectuez override, distrug lista initiala
                    si inserez peste ea una noua alcatuita din stilurle primite
                    ca parametru */
                    freeList(&crtNode->info->style);
                    InsStyle(styleCopy, &crtNode->info->style);
                }
                else if(!strcmp(mode, "appendStyle"))
                    appendStyle(styleCopy, &crtNode->info->style);
                free(styleCopy);
                break;
            }
            //Apelez recursiv pentru copii nodului curent
            if(*verif == 0)
            overrideId(crtNode, id, newStyle, level + 1, mode, verif);
        }
        crtNode = crtNode->nextSibling;
    }
}

/*Functia efectueaza operatia ceruta pentru nodurile cu clasa si tipul
specificate */
int overrideClassType(TArb root, char* className, char* typeName,
char* newStyle, char* mode)
{
    //Parcurg BFS utilizand o coada
    void* queue = InitQ(sizeof(TArb));
    if(!queue) exit(ERROR);
    //Aloc un element al cozii
    TArb* queueEl = calloc(1, sizeof(TArb));
    if(!queueEl) exit(ERROR);
    *queueEl = calloc(1, sizeof(TNodArb));
    if(!*queueEl) exit(ERROR);

    //Fac o copie a valorii alocate pentru a putea da free
    TArb copy = *queueEl;

    //adaug radacina arborelui in coada
    *queueEl = root;
    Enqueue(queue, queueEl);
    int verif = 0;

    while(!VidaQ(queue))
    {
        /*Pentru fiecare copil al fiecarui element din coada, verific ca acesta
        sa respecte cele 2 conditii */
        Dequeue(queue, queueEl);

        TArb p = (*queueEl)->firstChild;

        while(p != NULL)
        {
            //in caz afirmativ, efectuez operatiile si setez verif la 1
            if(checkClass(p, className) && checkType(p, typeName))
            {
                verif = 1;
                char* styleCopy = strdup(newStyle);
                if(!strcmp(mode, "overrideStyle"))
                {
                    freeList(&p->info->style);
                    InsStyle(styleCopy, &p->info->style);
                }
                else if(!strcmp(mode, "appendStyle"))
                    appendStyle(styleCopy, &p->info->style);
                free(styleCopy);

            }
            //setez un nou element al cozii in care adaug nodul curent
            TArb* queueEl2 = calloc(1, sizeof(TArb));
            *queueEl2 = calloc(1, sizeof(TNodArb));
            TArb copy = *queueEl2;
            *queueEl2 = p;

            //adaug nodul curent in coada
            Enqueue(queue, queueEl2);
            free(copy);
            free(queueEl2);
            p = p->nextSibling;
        }
    }
    //Eliberez memoria alocata(prin intermediul copiei initiale)
    (*queueEl)->firstChild= NULL;
    free(copy);
    free(queueEl);
    free(queue);
    return verif;
}

/*Realizeaza override sau append pentru nodurile care au parinte de tip
parentType si copil de tip typeName */
int overrideParentType(TArb root, char* parentType, char* typeName,
char* newStyle, char* mode)
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
            if(checkType((*queueEl), parentType) && checkType(p, typeName))
            {
                verif = 1;
                char* styleCopy = strdup(newStyle);
                if(!strcmp(mode, "overrideStyle"))
                {
                    freeList(&p->info->style);
                    InsStyle(styleCopy, &p->info->style);
                }
                else if(!strcmp(mode, "appendStyle"))
                    appendStyle(styleCopy, &p->info->style);
                free(styleCopy);
            }
            TArb* queueEl2 = calloc(1, sizeof(TArb));
            *queueEl2 = calloc(1, sizeof(TNodArb));
            TArb copy = *queueEl2;
            *queueEl2 = p;

            Enqueue(queue, queueEl2);
            free(copy);
            free(queueEl2);
            p = p->nextSibling;
        }
    }
    (*queueEl)->firstChild= NULL;
    free(copy);
    free(queueEl);
    free(queue);
    return verif;
}

/* Realizeaza override sau append pentru nodurile care respecta conditia
check cu campul Name */
int overrideCond(TArb root, char* Name, FCheck check,
char* newStyle, char* mode)
{
    void* queue = InitQ(sizeof(TArb));
    if(!queue) exit(ERROR);
    TArb* queueEl = calloc(1, sizeof(TArb));
    *queueEl = calloc(1, sizeof(TNodArb));
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
            if(check(p, Name))
            {
                verif = 1;
                char* styleCopy = strdup(newStyle);
                if(!strcmp(mode, "overrideStyle"))
                {
                    freeList(&p->info->style);
                    p->info->style = NULL;
                    InsStyle(styleCopy, &p->info->style);
                }
                else if(!strcmp(mode, "appendStyle"))
                    appendStyle(styleCopy, &p->info->style);
                free(styleCopy);
            }
            TArb* queueEl2 = calloc(1, sizeof(TArb));
            *queueEl2 = calloc(1, sizeof(TNodArb));
            TArb copy = *queueEl2;
            *queueEl2 = p;

            Enqueue(queue, queueEl2);
            free(copy);
            free(queueEl2);
            p = p->nextSibling;
        }
    }
    (*queueEl)->firstChild= NULL;
    free(copy);
    free(queueEl);
    free(queue);
    return verif;
}

/*Prelucreaza nodurile al caror stramos este de tipul ancestorType si care au
typeName */
int overrideAncestorType(TArb root, char* ancestorType, char* typeName,
char* newStyle, char* mode)
{
    void* queue = InitQ(sizeof(TArb));
    if(!queue) exit(ERROR);

    TArb* queueEl = calloc(1, sizeof(TArb));
    *queueEl = calloc(1, sizeof(TNodArb));
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

            if(checkType(p, ancestorType))
            {
                /*Daca am gasit stramosul, aplic override dupa tip pentru el
                pentru a gasi in acest subarbore nodul de sters */
                if(overrideCond(p, typeName, checkType, newStyle, mode))
                    verif = 1;
            }
            TArb* queueEl2 = calloc(1, sizeof(TArb));
            *queueEl2 = calloc(1, sizeof(TNodArb));
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