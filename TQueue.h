/* OPREA-GROZA Gabriel - 313CB */
#include <stdlib.h>
#include <string.h>

#define DIMEQ(a) (((AQ)(a))->dime)
#define IC(a) (((AQ)(a))->ic)
#define SC(a) (((AQ)(a))->sc)
#define VidaQ(a) ((IC(a) == NULL) && (SC(a) == NULL))

/*Pointer la o functie de comparatie intre 2 elemente din coada, o voi folosi
pentru inserarea in coada de prioritati */
typedef int(*TFCmp)(void* s1, void* s2);

//Pointer la o functie de eliberare de memorie
typedef void (*TF)(void*);

//Structura celulei generice de lista
typedef struct cel
{
    struct cel* urm;
    void* info;
}TCelQ, *ACelQ;

typedef struct Queue
{
    size_t dime; //dimensiunea unui element din coada
    ACelQ ic, sc; //adresa de la inceputul, respectiv sfarsitul cozii
}TQueue, *AQ;

//Functie ce aloca memorie pentru o coada cu elemente de dimensiue d
void* InitQ(size_t d);

//Functie ce adauga in coada referinta de la adresa ae
void Enqueue(void* a, void* ae);

/*Functie ce adauga in coada referinta de la adresa ae, in functie
de prioritatea data prin functia de comparatie cmp. Pentru distrugerea
cozii auxiliare utilizate in implementarea acestei functii, se mai foloseste
si un pointer la o functie de eliberare de memorie */
void EnqueuePriority(void* a, void* ae, TFCmp comp, TF free_elem);

/*Functie ce intoarce la adresa ae elementul de la inceputul cozii, pe care
il elimina din lista */
int Dequeue(void* a, void* ae);

/* Functie ce intoarce la adresa ae elementul de la inceputul cozii */
int Peek(void* a, void* ae);

/*Functie ce concateneaza 2 cozi, adaugand coada sursa la sfarsitul cozii
destinatie */
void ConcatQ(void* d, void* s);

/*Functie ce distruge o coada, primind un pointer la o functie care elibereaza
memoria asociata fiecarei celule din lista generica a cozii */
void DistrQ(void** a, TF free_elem);
