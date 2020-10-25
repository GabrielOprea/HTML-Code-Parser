/* OPREA-GROZA Gabriel - 313CB */
#include "TQueue.h"

//Functie care distruge lista generica a cozii, utilizand functia free_elem
void DistrugeLGQ(ACelQ* aL, TF free_elem)
{
    //Parcurg lista
    while(*aL != NULL)
    {
        //Adresa celulei de eliminat
        ACelQ aux = *aL;
        if (!aux)
            return;

        free_elem(aux->info);  // eliberez spatiul ocupat de info din celula
        *aL = aux->urm;    // sterg celula din lista
        free(aux);   // distrug celula
    }
}

//Aloca memorie pt o coada
void* InitQ(size_t d)
{

    AQ q;
    q = (AQ)malloc(sizeof(TQueue));
    if(!q) return NULL;

    //Setez campul dime
    q->dime = d;

    //Si pointerii initial vor fi NULL(coada goala)
    q->sc = NULL;
    q->ic = NULL;

    //Returnez coada generica
    return (void*)q;
}

//Adauga informatia de la adresa ae in lista generica, la sfarsitul cozii
void Enqueue(void* a, void* ae)
{

    //Aloc o noua celula
    ACelQ aux = (ACelQ)calloc(1, sizeof(TCelQ));
    if(!aux) return;

    //Copiez elementul in info din celula
    aux->info = calloc(1, DIMEQ(a));
    memcpy(aux->info, ae, DIMEQ(a));
    aux->urm = NULL;

    //Daca inserez in coada vida, setez ambii pointeri
    if(VidaQ(a))
        SC(a) = IC(a) = aux;
    else
    {
        //Altfel, setez doar cel pentru sfarsitul cozii
        SC(a)->urm = aux;
        SC(a) = aux;
    }
}

//Functie ce elimina elementul din inceputul cozii si il returneaza in ae
int Dequeue(void* a, void* ae)
{
    if(VidaQ(a))
        return 0;

    //Copiez elementul la adresa ae
    memcpy(ae, IC(a)->info, DIMEQ(a));

    //Elimin elementul din lista
    ACelQ aux;
    aux = IC(a);
    IC(a) = aux->urm;

    //Eliberez memoria
    free(aux->info);
    free(aux);

    //Verific daca am ramas cu o coada vida
    if(IC(a) == NULL) SC(a) = NULL;
    return 1;
}

//Obtine elementul de la inceputul cozii, fara sa il elimine
int Peek(void* a, void* ae)
{
    if(VidaQ(a))
        return 0;

    memcpy(ae, IC(a)->info, DIMEQ(a));

    return 1;
}

//Suprapune coada sursa peste destinatie
void ConcatQ(void* d, void* s)
{
    //Daca destinatia e vida, atunci doar copiez pointerii
    if(VidaQ(d))
    {
        IC(d) = IC(s);
        SC(d) = SC(s);
        IC(s) = SC(s) = NULL;
    }

    //Daca sursa e vida, nu trebuie sa adaug nimic in destinatie
    if(VidaQ(s))
        return;

    if(!VidaQ(d) && !VidaQ(s))
    {
        //Altfel, alipesc la sfarsitul destinatiei inceputul sursei
        SC(d)->urm = IC(s);
        SC(d) = SC(s);
    }

    //Fac sursa sa fie coada vida
    IC(s) = SC(s) = NULL;
}

//Functie ce adauga in coada pe baza prioritatii
void EnqueuePriority(void* a, void* ae, TFCmp comp, TF free_elem)
{

    //Daca am coada vida, adaug elementul normal
    if(VidaQ(a))
        Enqueue(a, ae);
    else
    {
        //Altfel, folosesc o coada auxiliara cu aceeasi dimensiune
        void* qaux = InitQ(DIMEQ(a));
        if(!qaux) return;

        //In aux voi retine elementul curent din coada initiala
        void* aux = calloc(1, DIMEQ(a));
        if(!aux) return;

        Peek(a, aux);

        /*Cat timp ultimul element din coada initiala nu respecta relatia de
        comparatie cu elementul de la adresa ae, extrag din coada initiala si
        inserez elementele extrase in coada auxiliara */
        while(comp(aux, ae) < 0 && !VidaQ(a))
        {
            Dequeue(a, aux);
            Enqueue(qaux, aux);
            Peek(a, aux);
        }

        //Adaug elementul de adaugat in coada auxiliara
        Enqueue(qaux, ae);

        //Adaug peste coada auxiliara coada initiala
        ConcatQ(qaux, a);

        /*Acest apel este echivalent cu schimbarea referintei cozi
        de la qaux la a, deoarece a este vida */
        ConcatQ(a, qaux);

        //Eliberez memoria pentru elementul auxiliar
        free(aux);

        //Distrug coada auxiliara
        DistrQ(&qaux, free_elem);
    }
}

//Functie ce distruge o coada
void DistrQ(void** a, TF free_elem)
{

    //Dereferentiez adresa cozii
    void* q = *a;

    //Obtin inceputul listei
    ACelQ p = IC(q);
    //Setez inceputul si sf listei la NULL
    IC(q) = NULL;
    SC(q) = NULL;

    //Distrug lista
    DistrugeLGQ(&p, free_elem);

    //Distrug descriptorul pt coada
    free(*a);
    //Setez zona eliberata la NULL
    *a = NULL;
}

