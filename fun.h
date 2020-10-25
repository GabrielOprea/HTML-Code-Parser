//Oprea-Groza Gabriel
//313CB


//Principalele functii utilizate in acest program:

char* strdup(char* str); //alternativa a fct build-in pentru ISO C99


//Adaugarea si formatarea codului HTML
void add(TArb root, char* id, char* tag, FILE* output);
void format(TArb root, int nrTabs, FILE* output);

/* Functiile de alocare pentru un nod din arbrore si pentru constructia de
noduri din fisiere text, respectiv dintr-un sir de caractere*/
TArb alocTree();
void buildRoot(TArb root, FILE* html);
TArb buildNode(char* tag);
void buildTree(TArb root, TParseState crtState, FILE* html, char c,
char* tempName, char* tempValue);

/* Functii pentru stergerea nodurior ce respecta o anumita conditie */
int deleteAncestorType(TArb root, char* ancestorType, char* typeName);
int deleteParentType(TArb root, char* parentType, char* typeName);
int deleteClassType(TArb root, char* className, char* typeName);
int deleteCond(TArb root, char* Name, FCheck check);
void deleteId(TArb root, char* id, int level, int* verif);

// Verificarea de conditii dpdv al tipului sau clasei
int checkType(TArb node, char* typeName);
int checkClass(TArb node, char* className);

/* Functii helper cum ar fi cele pentru obtinerea nodurilor cu un anumit
id sau cele ce nu au un nextSibling */
TArb findId(TArb root, char* id, int level);
TArb getLastSon(TArb root, int* index);
int isEmptyTag(TArb node);
void fixId(TArb root, int level);
int getCrtIndex(char* id, int level);


//Functii de afisare
void printMultiTree(TArb root, int level, FILE* output);
void printClosingTag(TArb root, int nrTabs, FILE* output);
void printContent(TArb root, int nrTabs, FILE* output);


//De inserare in liste, respectiv de append in liste
void appendStyle(char* value, TAttr* list);
int appendAttr(char* name, char* value, TAttr* list);
void InsStyle(char* value, TAttr* list);
void InsAttr(char* name, char* value, TAttr* list);


//De trunchiere a spatiilor de la inceput, respectiv final
void deleteStartSpaces(char* str);
void deleteEndSpaces(char* str);


//Eliberare de memorie pentru lista, nod, arbore
void freeList(TAttr* listAdr);
void freeTree(TArb* rootAdr);
void freeNode(TArb* nodeAdr);

//Override sau append pentru nodurile ce respecta o anumita conditie
void overrideId(TArb root, char* id, char* newStyle, int level, char* mode,
int* verif);
int overrideCond(TArb root, char* Name, FCheck check, char* newStyle,
char* mode);
int overrideClassType(TArb root, char* className, char* typeName,
char* newStyle, char* mode);
int overrideAncestorType(TArb root, char* ancestorType, char* typeName,
char* newStyle, char* mode);
int overrideParentType(TArb root, char* parentType, char* typeName,
char* newStyle, char* mode);


//functia de baza ce va citi din fisiere si va prelucra datele
void Console();
