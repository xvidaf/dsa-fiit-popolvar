#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>

//Node pre zoznam susednosti
struct Node
{
    int stlpec;
    int riadok;
    int id;
    int cost;
    struct Node* next;
};
//Vrchol v ktorom je zoznam susednosti
struct Vrchol
{
    int stlpec;
    int riadok;
    struct Node *head;
};
// Node v heape
struct heapNode
{
    int id;
    int cost;
};
//Vytvorime novy node
struct heapNode* newNode(int id, int cost)
{
    struct heapNode* newNode = malloc(sizeof(struct heapNode));
    newNode -> id = id;
    newNode -> cost = cost;
    return newNode;
}
//Uprava heapu po inserte noveho prvku, zdola hore
void afterInsert(struct heapNode **minHeap, int pocet)
{
    int swapCost;
    int swapId;
    int i = pocet;
    while(i >= 2)
    {
        //Vymenime ich
        if(minHeap[i/2] -> cost > minHeap[i] -> cost)
        {
            swapCost = minHeap[i/2] -> cost;
            swapId = minHeap[i/2] -> id;

            minHeap[i/2] -> cost = minHeap[i] -> cost;
            minHeap[i/2] -> id = minHeap[i] -> id;

            minHeap[i] -> cost = swapCost;
            minHeap[i] -> id = swapId;

        }
        i = i/2;
    }
}
//Uprava heapu po zmeneni existujuce prvku
void afterModify(struct heapNode **minHeap, int pocet, int id)
{
    int swapCost;
    int swapId;
    for(int i = id; i >= 2 ; --i)
    {
        //Vymenime ich
        if(minHeap[i/2] -> cost > minHeap[i] -> cost)
        {
            swapCost = minHeap[i/2] -> cost;
            swapId = minHeap[i/2] -> id;

            minHeap[i/2] -> cost = minHeap[i] -> cost;
            minHeap[i/2] -> id = minHeap[i] -> id;

            minHeap[i] -> cost = swapCost;
            minHeap[i] -> id = swapId;
        }
    }
}
//Heapify, uprava zhora dole
void heapify(struct heapNode **minHeap, int pocetPrvkov, int pozicia)
{
    int smol = pozicia;
    int lavy = 2*pozicia ;
    int pravy = 2*pozicia + 1;

    if (lavy <= pocetPrvkov && minHeap[lavy] -> cost < minHeap[smol] -> cost) {
        smol = lavy;
    }

    if (pravy <= pocetPrvkov && minHeap[pravy] -> cost < minHeap[smol] -> cost) {
        smol = pravy;
    }

    if (minHeap[smol] -> cost != minHeap[pozicia] -> cost)
    {
        int swapCost = minHeap[smol] -> cost;
        int swapId = minHeap[smol] -> id;

        minHeap[smol] -> cost = minHeap[pozicia] -> cost;
        minHeap[smol] -> id = minHeap[pozicia] -> id;

        minHeap[pozicia] -> cost = swapCost;
        minHeap[pozicia] -> id = swapId;

        heapify(minHeap, pocetPrvkov, smol);
    }
}
//Odstranenie rootu z heapu
void removeRoot(struct heapNode **minHeap, int pocetPrvkov)
{
    if(pocetPrvkov != 1) {
        //Vymenime prvy a posledny
        int swapCost;
        int swapId;

        minHeap[1]->cost = minHeap[pocetPrvkov]->cost;
        minHeap[1]->id = minHeap[pocetPrvkov]->id;

        minHeap[pocetPrvkov] = NULL;
    }
    else{
        minHeap[1] = NULL;
    }
}
//Test ci heap je validny
void testHeap(struct heapNode **minHeap, int pocetPrvkov )
{
    for(int i = pocetPrvkov; i >= 2; --i)
    {
        if(minHeap[i] -> cost < minHeap[i/2] -> cost )
        {
            printf("%d je vecsie ako %d v heape, i je %d", minHeap[i/2], minHeap[i], i);
            break;
        }
    }
}
//Vytvorenie permutacii typu int, zo vstupneho pola
void permutacie(int polePrincezien[], int aktualnaPozicia, int dlzka, int VsetkyPermutacie[], int *counterPrePermutacie)
{
    int dummy;
    if (aktualnaPozicia == dlzka)
    {
        for(int i = 0; i < dlzka; ++i)
        {
            //printf("%d " ,polePrincezien[i]);
            VsetkyPermutacie[*counterPrePermutacie] = polePrincezien[i];
            *counterPrePermutacie = *counterPrePermutacie + 1;
        }
        //printf("\n");
    }
    else
    {
        for (int i = aktualnaPozicia; i < dlzka; i++)
        {

            dummy = polePrincezien[aktualnaPozicia];
            polePrincezien[aktualnaPozicia] = polePrincezien[i];
            polePrincezien[i] = dummy;

            permutacie(polePrincezien, aktualnaPozicia+1, dlzka, VsetkyPermutacie,counterPrePermutacie);

            dummy = polePrincezien[aktualnaPozicia];
            polePrincezien[aktualnaPozicia] = polePrincezien[i];
            polePrincezien[i] = dummy ;
        }
    }
}

//Djikstra
//Tabulka do ktorej zapisujeme pocas djikstru
struct tabulkaPrejdenia
{
    int found;
    int cost;
    int visitedBy;
};
//Djikstrov algoritmus
int findPath(int zaciatokStlpec, int zaciatokRiadok, int cielId, struct Vrchol **array, char **mapa, int pocetValidnychPolicok,int *dlzka_cesty, int draha[], int jeToDrak)
{
    if(zaciatokRiadok == array[cielId] ->riadok && zaciatokStlpec == array[cielId] -> stlpec )
    {
        if(*dlzka_cesty == 0)
        {
            *dlzka_cesty = 1;
            draha[0] = 0;
            draha[1] = 0;
        }
        return 0;
    }
    //Inicializacija minHeap
    struct heapNode **minHeap = malloc((pocetValidnychPolicok + 1) * sizeof(struct heapNode));
    //struct heapNode minHeap[pocetValidnychPolicok];
    int pocetPrvkov = 0;
    for(int i = 0; i < pocetValidnychPolicok ; ++i)
    {
        minHeap[i] = NULL;
    }
    //Inicializacia tabulky
    struct tabulkaPrejdenia **tabulkaDjikstra = malloc(pocetValidnychPolicok *sizeof(struct tabulkaPrejdenia));
    for(int i = 0; i < pocetValidnychPolicok; ++i)
    {
        tabulkaDjikstra[i] = malloc(sizeof(struct tabulkaPrejdenia));
        tabulkaDjikstra[i] -> visitedBy = -1;
        tabulkaDjikstra[i] -> cost = -1;
        tabulkaDjikstra[i] -> found = -1;
    }
    int costPrveho;
    if(mapa[zaciatokStlpec][zaciatokRiadok] == 'H')
    {
        costPrveho = 2;
    }
    else if(mapa[zaciatokStlpec][zaciatokRiadok] == 'C' || mapa[zaciatokStlpec][zaciatokRiadok] == 'P'||mapa[zaciatokStlpec][zaciatokRiadok] == 'D')
    {
        costPrveho = 1;
    }
    else{
        printf("Neplatne prve policko \n");
        return -500000;
    }
    tabulkaDjikstra[0] -> cost = costPrveho;
    //Djisktra
    //Zistenie id zaciatku
    int pociatocneId;
    for(int i = 0; i < pocetValidnychPolicok; ++i)
    {
        if(array[i] -> stlpec == zaciatokStlpec && array[i] -> riadok == zaciatokRiadok)
        {
            pociatocneId = i;
            break;
        }
    }
    minHeap[1] = newNode(pociatocneId,costPrveho);
    //printf("5%d5",minHeap[1]->cost);
    ++pocetPrvkov;
    //Pomocne premenne pre cyklus
    int founder;
    struct Node *helper = malloc(sizeof(struct Node));

    while(pocetPrvkov != 0)
    {
        if(pocetPrvkov == 0)
        {
            break;
        }
        //Id vymazaneho prvku z heapu(navstivujeme ho)
        founder = minHeap[1] -> id;
        //Removneme ho z heapu
        removeRoot(minHeap, pocetPrvkov);
        //Nastavime ze sme ho navstivili
        tabulkaDjikstra[founder] -> found = 1;
        --pocetPrvkov;
        if(pocetPrvkov != 0) {
            heapify(minHeap, pocetPrvkov, 1);
            //testHeap(minHeap, pocetPrvkov);
            //printf("PRI HEAPIFY\n");
        }
        //Prejdeme vsetkych jeho susedov
        helper = array[founder] -> head;
        while(helper != NULL)
        {
            //Ak este nebol nikdy navstiveny tak ho pridame
            if(tabulkaDjikstra[helper -> id] -> visitedBy == -1 && tabulkaDjikstra[helper -> id] -> found != 1) {
                tabulkaDjikstra[helper->id]->visitedBy = founder;
                tabulkaDjikstra[helper->id]->cost = helper -> cost + tabulkaDjikstra[founder] -> cost;
                minHeap[pocetPrvkov + 1] = newNode(helper -> id ,helper -> cost + tabulkaDjikstra[founder] -> cost);
                ++pocetPrvkov;
                afterInsert(minHeap,pocetPrvkov);
                //testHeap(minHeap, pocetPrvkov);
                //printf("PRI INSERTE\n");
            }
            //Ak je cesta kratsia
            else if(tabulkaDjikstra[helper->id]->found != 1 && tabulkaDjikstra[helper->id]->cost > (helper -> cost + tabulkaDjikstra[founder] -> cost ) )
            {
                //printf("prepisujeme");
                tabulkaDjikstra[helper->id]->cost = helper -> cost + tabulkaDjikstra[founder] -> cost;
                tabulkaDjikstra[helper->id]->visitedBy = founder;
                for(int i =0; i < pocetPrvkov; ++i)
                {
                    if(minHeap[i] -> id == helper -> id)
                    {
                        minHeap[i] -> cost = helper -> cost + tabulkaDjikstra[founder] -> cost;
                        afterModify(minHeap, pocetPrvkov,i );
                        //testHeap(minHeap, pocetPrvkov);
                        //printf("PRI UPRAVE\n");
                        break;
                    }
                }
            }
            helper = helper -> next;
        }

    }
    //Skontrolujeme ci sme sa dostali k princezne/drakovi
    if(tabulkaDjikstra[cielId] -> visitedBy == -1)
    {
        printf("Nevedeli sme sa dostat k cielu\n");
        return -500000;
    }
    //Zapiseme cestu
    int startId;
    for (int i = 0; i < pocetValidnychPolicok; ++i) {
        if (array[i]->stlpec == zaciatokStlpec && array[i]->riadok == zaciatokRiadok) {
            startId = i;
            break;
        }
    }
    int fakeCielId = cielId;
    int pocetKrokov = 0;
    while (fakeCielId != startId) {
        ++pocetKrokov;
        fakeCielId = tabulkaDjikstra[fakeCielId]->visitedBy;
    }
    ++pocetKrokov;
    //Vratime pocetkrokov a upravime tabulku prechodu
    if(jeToDrak == 1) {
        fakeCielId = cielId;
        int polePreKroky[pocetKrokov * 2];
        int i = 0;
        while (fakeCielId != startId) {
            polePreKroky[i * 2] = array[fakeCielId]->stlpec;
            polePreKroky[i * 2 + 1] = array[fakeCielId]->riadok;
            i++;
            fakeCielId = tabulkaDjikstra[fakeCielId]->visitedBy;
        }
        if(array[startId] -> stlpec == 0 && array[startId] -> riadok == 0 && *dlzka_cesty == 0)
        {
            polePreKroky[i * 2] = array[fakeCielId]->stlpec;
            polePreKroky[i * 2 + 1] = array[fakeCielId]->riadok;
        }
        else{
            pocetKrokov = pocetKrokov - 1;
        }
        int polePreKrokyOtocene[pocetKrokov * 2];
        int originalnaDlzka = 0;
        if(*dlzka_cesty != 0)
        {
            originalnaDlzka = *dlzka_cesty * 2 - 1;
        }

            for (int l = 0; l < pocetKrokov * 2; ++l) {
                if (originalnaDlzka == 0) {
                    draha[originalnaDlzka + pocetKrokov * 2 - 1 - l] = polePreKroky[l];
                } else {
                    draha[originalnaDlzka + pocetKrokov * 2 - l] = polePreKroky[l];
                }
            }


    }

    if(jeToDrak == 1)
        return pocetKrokov;
    //Vratime pocetkrokov ale neupravime tabulku prechodov
    else{
        pocetKrokov = 0;
        fakeCielId = cielId;
        while (fakeCielId != startId) {
            if(mapa[array[fakeCielId] -> stlpec][array[fakeCielId] -> riadok] == 'P' || mapa[array[fakeCielId] -> stlpec][array[fakeCielId] -> riadok] == 'D' || mapa[array[fakeCielId] -> stlpec][array[fakeCielId] -> riadok] == 'C')
            {
                ++pocetKrokov;
            }
            else if(mapa[array[fakeCielId] -> stlpec][array[fakeCielId] -> riadok] == 'H')
            {
                pocetKrokov = pocetKrokov + 2 ;
            }
            fakeCielId = tabulkaDjikstra[fakeCielId]->visitedBy;
        }
        return pocetKrokov;
    };

    //printf("%d",minHeap[9]->cost);
}

//Pomocna funkcia pre vypisanie susednosti
void vypisSusednost(struct Vrchol **array, int pocetValidnychPolicok)
{
    struct Node *helper = malloc(sizeof(struct Node));
    for(int i = 0; i < pocetValidnychPolicok; ++i)
    {
        printf("Vrchol : [%d][%d]", array[i] -> stlpec, array[i] -> riadok);
        printf("Spojenia :");
        if(array[i] -> head != NULL)
        {
            helper = array[i] -> head;
            while(helper != NULL)
            {
                printf("[%d][%d] ", helper -> stlpec, helper -> riadok);
                printf("Cena %d ID %d",helper -> cost, helper -> id);
                helper = helper -> next;
            }

        }
        printf("\n");
    }
}
int *zachran_princezne(char **mapa, int n, int m, int t, int *dlzka_cesty)
{
    //Zistime pocet prechodnych bodov
    int pocetValidnychPolicok = 0;
    int pocetPrincezien = 0;
    int dostupnaPrinceznaCounter = 0;
    int drakId;
    int nasielSaDrak = -1;
    //Zistime kolko je princezien/drakov a zistime pocet prechodnych policok
    for(int stlpec = 0; stlpec < n; ++stlpec) {
        for (int riadok = 0; riadok < m; ++riadok)
        {
            if(mapa[stlpec][riadok] == 'C' || mapa[stlpec][riadok] == 'D' || mapa[stlpec][riadok] == 'P' || mapa[stlpec][riadok] == 'H' )
            {
                //Skontrolovat ci je princezna alebo drak dostupne
                if(mapa[stlpec][riadok] == 'D' || mapa[stlpec][riadok] == 'P')
                {
                    //Hore
                    if(stlpec != n - 1 && (mapa[stlpec + 1][riadok] == 'C' || mapa[stlpec + 1][riadok] == 'D' || mapa[stlpec + 1][riadok] == 'P' || mapa[stlpec + 1][riadok] == 'H' ))
                    {
                        ++dostupnaPrinceznaCounter;
                    }
                    //Dole
                    if(stlpec != 0 && (mapa[stlpec - 1][riadok] == 'C' || mapa[stlpec - 1][riadok] == 'D' || mapa[stlpec - 1][riadok] == 'P' || mapa[stlpec - 1][riadok] == 'H' ))
                    {
                        ++dostupnaPrinceznaCounter;
                    }
                    //Vlavo
                    if(riadok != 0 && (mapa[stlpec][riadok - 1] == 'C' || mapa[stlpec][riadok - 1] == 'D' || mapa[stlpec][riadok - 1] == 'P' || mapa[stlpec][riadok - 1] == 'H' ))
                    {
                        ++dostupnaPrinceznaCounter;
                    }
                    //Vpravo
                    if(riadok != m-1 && (mapa[stlpec][riadok + 1] == 'C' || mapa[stlpec][riadok + 1] == 'D' || mapa[stlpec][riadok + 1] == 'P' || mapa[stlpec][riadok + 1] == 'H') )
                    {
                        ++dostupnaPrinceznaCounter;
                    }
                    if(dostupnaPrinceznaCounter == 0)
                    {
                        printf("Princezna alebo drak nedostupne");
                        *dlzka_cesty = 0;
                        return NULL;
                    }
                    dostupnaPrinceznaCounter = 0;
                }
                ++pocetValidnychPolicok;

            }if(mapa[stlpec][riadok] == 'P')
            {
                ++pocetPrincezien;
            }
            if(mapa[stlpec][riadok] == 'D')
            {
               nasielSaDrak = 1;
            }
        }
    }
    //printf("Pocet princezien : %d\n",pocetPrincezien);
    //printf("Pocet validnych policok : %d\n",pocetValidnychPolicok);

    if(pocetPrincezien == 0 || nasielSaDrak == -1)
    {
        printf("Na mape niesu princezne");
        *dlzka_cesty = 0;
        return NULL;
    }


    //free(matica);
    //Experimental
    //Inicializujeme array
    struct Vrchol **array = malloc(pocetValidnychPolicok * sizeof(struct Vrchol));
    for (int i = 0; i < pocetValidnychPolicok; ++i) {
        array[i] = NULL;
    }
    int aktualnyIndex = 0;
    struct Node *helper = malloc(sizeof(struct Node));
    struct Node *helper2 = malloc(sizeof(struct Node));
    //Spravime zoznam susednosti
    for(int stlpec = 0; stlpec < n; ++stlpec)
    {
        for(int riadok = 0; riadok < m; ++riadok)
        {
            if(mapa[stlpec][riadok] == 'C' || mapa[stlpec][riadok] == 'D' || mapa[stlpec][riadok] == 'P' || mapa[stlpec][riadok] == 'H')
            {
                if(mapa[stlpec][riadok] == 'D')
                {
                    drakId = aktualnyIndex;
                }
                array[aktualnyIndex] = malloc(sizeof(struct Vrchol));
                array[aktualnyIndex] -> stlpec = stlpec;
                array[aktualnyIndex] -> riadok = riadok;
                array[aktualnyIndex] -> head = NULL;
                //Hore
                if(stlpec != n - 1 && (mapa[stlpec + 1][riadok] == 'C' || mapa[stlpec + 1][riadok] == 'D' || mapa[stlpec + 1][riadok] == 'P' || mapa[stlpec + 1][riadok] == 'H' ))
                {
                    if(array[aktualnyIndex] -> head == NULL)
                    {
                        array[aktualnyIndex] -> head = malloc(sizeof(struct Node));
                        array[aktualnyIndex] -> head -> stlpec = stlpec + 1;
                        array[aktualnyIndex] -> head -> riadok =  riadok;
                        array[aktualnyIndex] -> head -> next = NULL;
                        if(mapa[stlpec + 1][riadok] == 'H')
                        {
                            array[aktualnyIndex] -> head -> cost = 2;
                        }
                        else{
                            array[aktualnyIndex] -> head -> cost = 1;
                        }
                    } else{
                        helper = array[aktualnyIndex] -> head;
                        while(helper != NULL) {
                            helper2 = helper;
                            helper = helper->next;
                        }
                        helper  = malloc(sizeof(struct Node));
                        helper -> stlpec = stlpec + 1;
                        helper -> riadok =  riadok;
                        helper -> next = NULL;
                        if(mapa[stlpec + 1][riadok] == 'H')
                        {
                           helper -> cost = 2;
                        }
                        else{
                            helper  -> cost = 1;
                        }
                        helper2 -> next = helper;

                    }
                }
                //Dole
                if(stlpec != 0 && (mapa[stlpec - 1][riadok] == 'C' || mapa[stlpec - 1][riadok] == 'D' || mapa[stlpec - 1][riadok] == 'P' || mapa[stlpec - 1][riadok] == 'H' ))
                {
                    if(array[aktualnyIndex] -> head == NULL)
                    {
                        array[aktualnyIndex] -> head = malloc(sizeof(struct Node));
                        array[aktualnyIndex] -> head -> stlpec = stlpec - 1;
                        array[aktualnyIndex] -> head -> riadok =  riadok;
                        array[aktualnyIndex] -> head -> next = NULL;
                        if(mapa[stlpec - 1][riadok] == 'H')
                        {
                            array[aktualnyIndex] -> head -> cost = 2;
                        }
                        else{
                            array[aktualnyIndex] -> head -> cost = 1;
                        }
                    } else{
                        helper = array[aktualnyIndex] -> head;
                        while(helper != NULL) {
                            helper2 = helper;
                            helper = helper->next;
                        }
                        helper  = malloc(sizeof(struct Node));
                        helper  -> stlpec = stlpec - 1;
                        helper  -> riadok =  riadok;
                        helper  -> next = NULL;
                        if(mapa[stlpec - 1][riadok] == 'H')
                        {
                            helper -> cost = 2;
                        }
                        else{
                            helper -> cost = 1;
                        }
                        helper2 -> next = helper;
                    }
                }
                //Vlavo
                if(riadok != 0 && (mapa[stlpec][riadok - 1] == 'C' || mapa[stlpec][riadok - 1] == 'D' || mapa[stlpec][riadok - 1] == 'P' || mapa[stlpec][riadok - 1] == 'H' ))
                {
                    if(array[aktualnyIndex] -> head == NULL)
                    {
                        array[aktualnyIndex] -> head = malloc(sizeof(struct Node));
                        array[aktualnyIndex] -> head -> stlpec = stlpec;
                        array[aktualnyIndex] -> head -> riadok =  riadok - 1;
                        array[aktualnyIndex] -> head -> next = NULL;
                        if(mapa[stlpec][riadok - 1] == 'H')
                        {
                            array[aktualnyIndex] -> head -> cost = 2;
                        }
                        else{
                            array[aktualnyIndex] -> head -> cost = 1;
                        }
                    } else{
                        helper = array[aktualnyIndex] -> head;
                        while(helper != NULL) {
                            helper2 = helper;
                            helper = helper->next;
                        }
                        helper = malloc(sizeof(struct Node));
                        helper -> stlpec = stlpec;
                        helper -> riadok =  riadok - 1;
                        helper -> next = NULL;
                        if(mapa[stlpec][riadok - 1] == 'H')
                        {
                            helper -> cost = 2;
                        }
                        else{
                            helper -> cost = 1;
                        }
                        helper2 -> next = helper;
                    }
                }
                //Vpravo
                if(riadok != m-1 && (mapa[stlpec][riadok + 1] == 'C' || mapa[stlpec][riadok + 1] == 'D' || mapa[stlpec][riadok + 1] == 'P' || mapa[stlpec][riadok + 1] == 'H') )
                {
                    if(array[aktualnyIndex] -> head == NULL)
                    {
                        array[aktualnyIndex] -> head = malloc(sizeof(struct Node));
                        array[aktualnyIndex] -> head -> stlpec = stlpec;
                        array[aktualnyIndex] -> head -> riadok =  riadok + 1;
                        array[aktualnyIndex] -> head -> next = NULL;
                        if(mapa[stlpec][riadok + 1] == 'H')
                        {
                            array[aktualnyIndex] -> head -> cost = 2;
                        }
                        else{
                            array[aktualnyIndex] -> head -> cost = 1;
                        }
                    } else{
                        helper = array[aktualnyIndex] -> head;
                        while(helper != NULL) {
                            helper2 = helper;
                            helper = helper->next;
                        }
                        helper  = malloc(sizeof(struct Node));
                        helper  -> stlpec = stlpec;
                        helper  -> riadok =  riadok + 1;
                        helper  -> next = NULL;
                        if(mapa[stlpec][riadok + 1] == 'H')
                        {
                            helper  -> cost = 2;
                        }
                        else{
                            helper  -> cost = 1;
                        }
                        helper2 -> next = helper;
                    }
                }
                ++aktualnyIndex;
            }
        }
    }
    for(int i = 0; i < pocetValidnychPolicok; ++i)
    {
        helper = array[i] -> head;
        while(helper != NULL)
        {
            for(int j = 0; i < pocetValidnychPolicok; ++j)
            {
                if(array[j] -> stlpec == helper -> stlpec && array[j] -> riadok == helper -> riadok)
                {
                    helper -> id = j;
                    break;
                }
            }
            helper = helper -> next;
        }
    }
    //vypisSusednost(array, pocetValidnychPolicok);
    //najdeme draka
    int *cesta;
    int *draha = malloc((pocetValidnychPolicok * pocetValidnychPolicok) * sizeof(int));
    *dlzka_cesty = 0;
    *dlzka_cesty += findPath(0,0,drakId,array,mapa,pocetValidnychPolicok, dlzka_cesty, draha,1);
    //Nedostupny ciel
    if(*dlzka_cesty < 0)
    {
        *dlzka_cesty = 0;
        return  NULL;
    }
    //Hladame princezne
    int startY = array[drakId] -> stlpec;
    int startX = array[drakId] -> riadok;
    //Zapiseme princezne do pola
    int polePrincezien[pocetPrincezien];
    int counter = 0;


    for(int i =0; i < pocetValidnychPolicok; ++i)
    {
        if(mapa[array[i] -> stlpec][array[i] -> riadok] == 'P')
        {
            polePrincezien[counter] = i;
            ++counter;
        }
    }
    int pocetPermutacii = 1;
    for(int l = pocetPrincezien; l>0; --l)
    {
        pocetPermutacii = pocetPermutacii * l;
    }
    int vsetkyPermutacia[pocetPermutacii*pocetPrincezien];
    int counterPrePermutacie = 0;
    //Vytvorime vsetky moznosti
    permutacie(polePrincezien, 0, pocetPrincezien, vsetkyPermutacia, &counterPrePermutacie);

    /*for( int i = 0; i < counterPrePermutacie; i = i + pocetPrincezien)
    {
        for( int j = i; j < pocetPrincezien + i; ++j)
            printf("%d ", vsetkyPermutacia[j]);
        printf("\n");
    }*/
    int konecnyCost = INT_MAX;
    int aktualnyCost = 0;
    int vybrataPermutacia;
    //Vybereme najkratsiu postupnost
    for( int i = 0; i < counterPrePermutacie; i = i + pocetPrincezien)
    {
        startY = array[drakId] -> stlpec;
        startX = array[drakId] -> riadok;
        for( int j = i; j < pocetPrincezien + i; ++j)
        {
            aktualnyCost += findPath(startY, startX, vsetkyPermutacia[j], array, mapa, pocetValidnychPolicok,dlzka_cesty, draha, 0);
            if(aktualnyCost < 0)
            {
                *dlzka_cesty = 0;
                return  NULL;
            }
            startY = array[vsetkyPermutacia[j]] -> stlpec;
            startX = array[vsetkyPermutacia[j]] -> riadok;
        }
        //printf("%d%d %d%d %d%d je cost %d\n", array[vsetkyPermutacia[i]] -> riadok,array[vsetkyPermutacia[i]] -> stlpec,array[vsetkyPermutacia[i+1]] -> riadok,array[vsetkyPermutacia[i+1]] -> stlpec, array[vsetkyPermutacia[i+2]] -> riadok,array[vsetkyPermutacia[i+2]] -> stlpec, aktualnyCost);
        if(aktualnyCost < konecnyCost)
        {
            vybrataPermutacia = i;
            konecnyCost = aktualnyCost;
        }
        aktualnyCost = 0;
    }
    //printf("Vybrali sme postupnost: ");
    startY = array[drakId] -> stlpec;
    startX = array[drakId] -> riadok;
    //printf("Cost na zaciatku je %d\n", *dlzka_cesty);
    //Prejdeme vybratu cestu, aj so zapisom do pola cesty
    for( int i = vybrataPermutacia; i < vybrataPermutacia + pocetPrincezien; i++)
    {
        *dlzka_cesty += findPath(startY, startX,vsetkyPermutacia[i],array,mapa,pocetValidnychPolicok, dlzka_cesty, draha,1);
        startY = array[vsetkyPermutacia[i]] -> stlpec;
        startX = array[vsetkyPermutacia[i]] -> riadok;
        //printf("Posielame %d%d cost je zatial %d\n",array[vsetkyPermutacia[i]] -> riadok,array[vsetkyPermutacia[i]] -> stlpec, *dlzka_cesty);
    }
    //printf("\n");
    /*
    while(pocetPrincezien > 0)
    {
        for(int i =0; i < counter; ++i)
        {
            if(polePrincezien[i] != -1) {
                dlzkaKPrinceznej = findPath(startY, startX, polePrincezien[i], array, mapa, pocetValidnychPolicok,dlzka_cesty, draha, 0);
                printf("Princezna %d %d, je od %d %d vzdialena %d\n", startY, startX, array[polePrincezien[i]] -> stlpec, array[polePrincezien[i]] -> riadok, dlzkaKPrinceznej);
                if (dlzkaKPrinceznej < najblizsiaPrinceznaVzdialenost) {
                    najblizsiaPrinceznaVzdialenost = dlzkaKPrinceznej;
                    najblizsiaPrinceznaID = polePrincezien[i];
                }
            }
        }
        printf("Najblizsia princezna je %d vo vzdialenosti %d\n", najblizsiaPrinceznaID, najblizsiaPrinceznaVzdialenost);
        for(int j = 0; j < dummyPocetPrincezien ; ++j)
        {
            if(polePrincezien[j] == najblizsiaPrinceznaID)
            {
                *dlzka_cesty += findPath(startY, startX,najblizsiaPrinceznaID,array,mapa,pocetValidnychPolicok, dlzka_cesty, draha,1);
                polePrincezien[j] = -1;
            }
        }
        startY = draha[(*dlzka_cesty*2) - 1];
        startX = draha[(*dlzka_cesty*2) - 2];
        najblizsiaPrinceznaVzdialenost = INT_MAX;
        --pocetPrincezien;
    }*/
    //printf("%d\n", *dlzka_cesty);
    //findPath(0,0,drakId,array,mapa,pocetValidnychPolicok, dlzka_cesty, draha);
    /*for(int i =0; i < *dlzka_cesty; ++i)
    {
        printf("%d %d\n", draha[i], draha[i+1]);
    }*/



    return draha;
}

int main()
{
    char **mapa;
    int i, test, dlzka_cesty, cas, *cesta;
    int pocetZadanychPrincezien;
    int n=0, m=0, t=0;
    int cislo = 0;
    char *c = "C";
    char *h = "H";
    char *nn = "N";
    char *p = "P";
    char *d = "D";
    int miestopreDrakaX;
    int miestopreDrakaY;
    int vlozenyDrak = 0;
    int pocetPrinceznien = 0;
    char end = '\0';
    FILE* f;
    while(1){
        printf("Zadajte cislo testu (0 ukonci program):\n");
        scanf("%d",&test);
        dlzka_cesty = 0;
        n=m=t=0;
        switch(test){
            case 0://ukonci program
                return 0;
            case 1://nacitanie mapy zo suboru
                f=fopen("test.txt","r");
                if(f)
                    fscanf(f, "%d %d %d", &n, &m, &t);
                else
                    continue;
                mapa = (char**)malloc(n*sizeof(char*));
                for(i=0; i<n; i++){
                    mapa[i] = (char*)malloc(m*sizeof(char));
                    for (int j=0; j<m; j++){
                        char policko = fgetc(f);
                        if(policko == '\n') policko = fgetc(f);
                        mapa[i][j] = policko;
                    }
                }
                fclose(f);
                cesta = zachran_princezne(mapa, n, m, t, &dlzka_cesty);
                break;
            case 2://nacitanie preddefinovanej mapy
                n = 10;
                m = 10;
                t = 12;
                mapa = (char**)malloc(n*sizeof(char*));
                mapa[0]="CCHCNHCCHN";
                mapa[1]="NNCCCHHCCC";
                mapa[2]="DNCCNNHHHC";
                mapa[3]="CHHHCCCCCC";
                mapa[4]="CCCCCNHHHH";
                mapa[5]="PCHCCCNNNN";
                mapa[6]="NNNNNHCCCC";
                mapa[7]="CCCCCPCCCC";
                mapa[8]="CCCNNHHHHH";
                mapa[9]="HHHPCCCCCC";
                cesta = zachran_princezne(mapa, n, m, t, &dlzka_cesty);
                break;
            case 3: //pridajte vlastne testovacie vzorky
                n = 3;
                m = 4;
                t = 12;
                mapa = (char**)malloc(n*sizeof(char*));
                mapa[0]="HCHH";
                mapa[1]="HHCH";
                mapa[2]="PHCD";
                cesta = zachran_princezne(mapa, n, m, t, &dlzka_cesty);
                break;
            case 4://Mapa bez princezien
                n = 4;
                m = 5;
                t = 12;
                mapa = (char**)malloc(n*sizeof(char*));
                mapa[0]="CCCCC";
                mapa[1]="CCCCC";
                mapa[2]="CCCCC";
                mapa[3]="CCCCD";
                cesta = zachran_princezne(mapa, n, m, t, &dlzka_cesty);
                break;
            case 5://Mapa s obklucenim drakom
                n = 4;
                m = 5;
                t = 12;
                mapa = (char**)malloc(n*sizeof(char*));
                mapa[0]="CCCCC";
                mapa[1]="CCCCC";
                mapa[2]="CCCCN";
                mapa[3]="PCCND";
                cesta = zachran_princezne(mapa, n, m, t, &dlzka_cesty);
                break;
            case 6://Mapa s obklucenou princeznou
                n = 4;
                m = 5;
                t = 12;
                mapa = (char**)malloc(n*sizeof(char*));
                mapa[0]="CCCCC";
                mapa[1]="CCCCC";
                mapa[2]="CCCCN";
                mapa[3]="DCCNP";
                cesta = zachran_princezne(mapa, n, m, t, &dlzka_cesty);
                break;
            case 7://Mapa s nepristupnym drakom
                n = 4;
                m = 5;
                t = 12;
                mapa = (char**)malloc(n*sizeof(char*));
                mapa[0]="CCCCC";
                mapa[1]="NNCCC";
                mapa[2]="CCNCC";
                mapa[3]="DCNCP";
                cesta = zachran_princezne(mapa, n, m, t, &dlzka_cesty);
                break;
            case 8://Mapa s nepristupnou princeznou
                n = 4;
                m = 5;
                t = 12;
                mapa = (char**)malloc(n*sizeof(char*));
                mapa[0]="CCCCC";
                mapa[1]="CCCNN";
                mapa[2]="CCNCC";
                mapa[3]="DCNCP";
                cesta = zachran_princezne(mapa, n, m, t, &dlzka_cesty);
                break;
            case 9:// 1 princezna
                n = 7;
                m = 7;
                t = 50;
                mapa = (char**)malloc(n*sizeof(char*));
                mapa[0]="CCCHHCH";
                mapa[1]="CCHHHCH";
                mapa[2]="HCDCHHC";
                mapa[3]="HHHCHHC";
                mapa[4]="HCHHCCC";
                mapa[5]="HCHHCHH";
                mapa[6]="CCHCHHP";
                cesta = zachran_princezne(mapa, n, m, t, &dlzka_cesty);
                break;
            case 10:// 2 princezne
                n = 7;
                m = 7;
                t = 50;
                mapa = (char**)malloc(n*sizeof(char*));
                mapa[0]="CCCHHCH";
                mapa[1]="CCHHHCH";
                mapa[2]="HCDCHHC";
                mapa[3]="HHHCHHC";
                mapa[4]="HCHHCCC";
                mapa[5]="HCHHCHH";
                mapa[6]="PCHCHHP";
                cesta = zachran_princezne(mapa, n, m, t, &dlzka_cesty);
                break;
            case 11:// 3 princezne
                n = 7;
                m = 7;
                t = 50;
                mapa = (char**)malloc(n*sizeof(char*));
                mapa[0]="CCCHHCH";
                mapa[1]="CCHHHCH";
                mapa[2]="HCDCHHC";
                mapa[3]="HHPCHHC";
                mapa[4]="HCHHCCC";
                mapa[5]="HCHHCHH";
                mapa[6]="PCHCHHP";
                cesta = zachran_princezne(mapa, n, m, t, &dlzka_cesty);
                break;
            case 12:// 4 princezne
                n = 7;
                m = 7;
                t = 50;
                mapa = (char**)malloc(n*sizeof(char*));
                mapa[0]="CCCHHCH";
                mapa[1]="CCHHHCP";
                mapa[2]="HCDCHHC";
                mapa[3]="HHPCHHC";
                mapa[4]="HCHHCCC";
                mapa[5]="HCHHCHH";
                mapa[6]="PCHCHHP";
                cesta = zachran_princezne(mapa, n, m, t, &dlzka_cesty);
                break;
            case 13:// 5 princezne
                n = 7;
                m = 7;
                t = 50;
                mapa = (char**)malloc(n*sizeof(char*));
                mapa[0]="CCCHHCH";
                mapa[1]="CCHHHCP";
                mapa[2]="HCDCHHC";
                mapa[3]="PHPCHHC";
                mapa[4]="HCHHCCC";
                mapa[5]="HCHHCHH";
                mapa[6]="PCHCHHP";
                cesta = zachran_princezne(mapa, n, m, t, &dlzka_cesty);
                break;
            case 14://Velka mapa
                m = 100;
                t = 5000;
                n = 100;
                mapa = (char **) malloc(n * sizeof(char *));
                mapa[0] = "CCHCNHCCHNCCHCNHCCHNCCHCNHCCHNCCHCNHCCHNCCHCNHCCHNCCHCNHCCHNCCHCNHCCHNCCHCNHCCHNCCHCNHCCHNCCHCNHCCHN";
                mapa[1] = "NNCCCHHCCCNNCCCHHCCCNNCCCHHCCCNNCCCHHCCCNNCCCHHCCCNNCCCHHCCCNNCCCHHCCCNNCCCHHCCCNNCCCHHCCCNNCCCHHCCC";
                mapa[2] = "CNCCNNHHHCCNCCNNHHHCCNCCNNHHHCCNCCNNHHHCCNCCNNHHHCCNCCNNHHHCCNCCNNHHHCCNCCNNHHHCCNCCNNHHHCCNCCNNHHHC";
                mapa[3] = "CHHHCCCCCCCHHHCCCCCCCHHHCCCCCCCHHHCCCCCCCHHHCCCCCCCHHHCCCCCCCHHHCCCCCCCHHHCCCCCCCHHHCCCCCCCHHHCCCCCC";
                mapa[4] = "CCCCCNHHHHCCCCCNHHHHCCCCCNHHHHCCCCCNHHHHCCCCCNHHHHCCCCCNHHHHCCCCCNHHHHCCCCCNHHHHCCCCCNHHHHCCCCCNHHHH";
                mapa[5] = "CCHCCCNNNNCCHCCCNNNNCCHCCCNNNNCCHCCCNNNNCCHCCCNNNNCCHCCCNNNNCCHCCCNNNNCCHCCCNNNNCCHCCCNNNNCCHCCCNNNN";
                mapa[6] = "NNNNNHCCCCNNNNNHCCCCNNNNNHCCCCNNNNNHCCCCNNNNNHCCCCNNNNNHCCCCNNNNNHCCCCNNNNNHCCCCNNNNNHCCCCNNNNNHCCCC";
                mapa[7] = "CCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCC";
                mapa[8] = "CCCNNHHHHHCCCNNHHHHHCCCNNHHHHHCCCNNHHHHHCCCNNHHHHHCCCNNHHHHHCCCNNHHHHHCCCNNHHHHHCCCNNHHHHHCCCNNHHHHH";
                mapa[9] = "HHHCCCCCCCHHHCCCCCCCHHHCCCCCCCHHHCCCCCCCHHHPCCCCCCHHHCCCCCCCHHHCCCCCCCHHHCCCCCCCHHHCCCCCCCHHHCCCCCCC";
                mapa[10] = "CCHCNHCCHNCCHCNHCCHNCCHCNHCCHNCCHCNHCCHNCCHCNHCCHNCCHCNHCCHNCCHCNHCCHNCCHCNHCCHNCCHCNHCCHNCCHCNHCCHN";
                mapa[11] = "NNCCCHHCCCNNCCCHHCCCNNCCCHHCCCNNCCCHHCCCNNCCCHHCCCNNCCCHHCCCNNCCCHHCCCNNCCCHHCCCNNCCCHHCCCNNCCCHHCCC";
                mapa[12] = "CNCCNNHHHCCNCCNNHHHCCNCCNNHHHCCNCCNNHHHCCNCCNNHHHCCNCCNNHHHCCNCCNNHHHCCNCCNNHHHCCNCCNNHHHCCNCCNNHHHC";
                mapa[13] = "CHHHCCCCCCCHHHCCCCCCCHHHCCCCCCCHHHCCCCCCCHHHCCCCCCCHHHCCCCCCCHHHCCCCCCCHHHCCCCCCCHHHCCCCCCCHHHCCCCCC";
                mapa[14] = "CCCCCNHHHHCCCCCNHHHHCCCCCNHHHHCCCCCNHHHHCCCCCNHHHHCCCCCNHHHHCCCCCNHHHHCCCCCNHHHHCCCCCNHHHHCCCCCNHHHH";
                mapa[15] = "CCHCCCNNNNCCHCCCNNNNCCHCCCNNNNCCHCCCNNNNCCHCCCNNNNCCHCCCNNNNCCHCCCNNNNCCHCCCNNNNCCHCCCNNNNCCHCCCNNNN";
                mapa[16] = "NNNNNHCCCCNNNNNHCCCCNNNNNHCCCCNNNNNHCCCCNNNNNHCCCCNNNNNHCCCCNNNNNHCCCCNNNNNHCCCCNNNNNHCCCCNNNNNHCCCC";
                mapa[17] = "CCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCC";
                mapa[18] = "CCCNNHHHHHCCCNNHHHHHCCCNNHHHHHCCCNNHHHHHCCCNNHHHHHCCCNNHHHHHCCCNNHHHHHCCCNNHHHHHCCCNNHHHHHCCCNNHHHHH";
                mapa[19] = "HHHCCCCCCCHHHCCCCCCCHHHCCCCCCCHHHCCCCCCCHHHCCCCCCCHHHCCCCCCCHHHCCCCCCCHHHCCCCCCCHHHCCCCCCCHHHCCCCCCC";
                mapa[20] = "CCHCNHCCHNCCHCNHCCHNCCHCNHCCHNCCHCNHCCHNCCHCNHCCHNCCHCNHCCHNCCHCNHCCHNCCHCNHCCHNCCHCNHCCHNCCHCNHCCHN";
                mapa[21] = "NNCCCHHCCCNNCCCHHCCCNNCCCHHCCCNNCCCHHCCCNNCCCHHCCCNNCCCHHCCCNNCCCHHCCCNNCCCHHCCCNNCCCHHCCCNNCCCHHCCC";
                mapa[22] = "CNCCNNHHHCCNCCNNHHHCCNCCNNHHHCCNCCNNHHHCCNCCNNHHHCCNCCNNHHHCCNCCNNHHHCCNCCNNHHHCCNCCNNHHHCCNCCNNHHHC";
                mapa[23] = "CHHHCCCCCCCHHHCCCCCCCHHHCCCCCCCHHHCCCCCCCHHHCCCCCCCHHHCCCCCCCHHHCCCCCCCHHHCCCCCCCHHHCCCCCCCHHHCCCCCC";
                mapa[24] = "CCCCCNHHHHCCCCCNHHHHCCCCCNHHHHCCCCCNHHHHCCCCCNHHHHCCCCCNHHHHCCCCCNHHHHCCCCCNHHHHCCCCCNHHHHCCCCCNHHHH";
                mapa[25] = "CCHCCCNNNNCCHCCCNNNNCCHCCCNNNNCCHCCCNNNNCCHCCCNNNNCCHCCCNNNNCCHCCCNNNNCCHCCCNNNNCCHCCCNNNNCCHCCCNNNN";
                mapa[26] = "NNNNNHCCCCNNNNNHCCCCNNNNNHCCCCNNNNNHCCCCNNNNNHCCCCNNNNNHCCCCNNNNNHCCCCNNNNNHCCCCNNNNNHCCCCNNNNNHCCCC";
                mapa[27] = "CCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCC";
                mapa[28] = "CCCNNHHHHHCCCNNHHHHHCCCNNHHHHHCCCNNHHHHHCCCNNHHHHHCCCNNHHHHHCCCNNHHHHHCCCNNHHHHHCCCNNHHHHHCCCNNHHHHH";
                mapa[29] = "HHHCCCCCCCHHHCCCCCCCHHHCCCCCCCHHHCCCCCCCHHHCCCCCCCHHHCCCCCCCHHHCCCCCCCHHHCCCCCCCHHHCCCCCCCHHHCCCCCCC";
                mapa[30] = "CCHCNHCCHNCCHCNHCCHNCCHCNHCCHNCCHCNHCCHNCCHCNHCCHNCCHCNHCCHNCCHCNHCCHNCPHCNHCCHNCCHCNHCCHNCCHCNHCCHN";
                mapa[31] = "NNCCCHHCCCNNCCCHHCCCNNCCCHHCCCNNCCCHHCCCNNCCCHHCCCNNCCCHHCCCNNCCCHHCCCNNCCCHHCCCNNCCCHHCCCNNCCCHHCCC";
                mapa[32] = "CNCCNNHHHCCNCCNNHHHCCNCCNNHHHCCNCCNNHHHCCNCCNNHHHCCNCCNNHHHCCNCCNNHHHCCNCCNNHHHCCNCCNNHHHCCNCCNNHHHC";
                mapa[33] = "CHHHCCCCCCCHHHCCCCCCCHHHCCCCCCCHHHCCCCCCCHHHCCCCCCCHHHCCCCCCCHHHCCCCCCCHHHCCCCCCCHHHCCCCCCCHHHCCCCCC";
                mapa[34] = "CCCCCNHHHHCCCCCNHHHHCCCCCNHHHHCCCCCNHHHHCCCCCNHHHHCCCCCNHHHHCCCCCNHHHHCCCCCNHHHHCCCCCNHHHHCCCCCNHHHH";
                mapa[35] = "CCHCCCNNNNCCHCCCNNNNCCHCCCNNNNCCHCCCNNNNCCHCCCNNNNCCHCCCNNNNCCHCCCNNNNCCHCCCNNNNCCHCCCNNNNCCHCCCNNNN";
                mapa[36] = "NNNNNHCCCCNNNNNHCCCCNNNNNHCCCCNNNNNHCCCCNNNNNHCCCCNNNNNHCCCCNNNNNHCCCCNNNNNHCCCCNNNNNHCCCCNNNNNHCCCC";
                mapa[37] = "CCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCC";
                mapa[38] = "CCCNNHHHHHCCCNNHHHHHCCCNNHHHHHCCCNNHHHHHCCCNNHHHHHCCCNNHHHHHCCCNNHHHHHCCCNNHHHHHCCCNNHHHHHCCCNNHHHHH";
                mapa[39] = "HHHCCCCCCCHHHCCCCCCCHHHCCCCCCCHHHCCCCCCCHHHCCCCCCCHHHCCCCCCCHHHCCCCCCCHHHCCCCCCCHHHCCCCCCCHHHCCCCCCC";
                mapa[40] = "CCHCNHCCHNCCHCNHCCHNCCHCNHCCHNCCHCNHCCHNCCHCNHCCHNCCHCNHCCHNCCHCNHCCHNCCHCNHCCHNCCHCNHCCHNCCHCNHCCHN";
                mapa[41] = "NNCCCHHCCCNNCCCHHCCCNNCCCHHCCCNNCCCHHCCCNNCCCHHCCCNNCCCHHCCCNNCCCHHCCCNNCCCHHCCCNNCCCHHCCCNNCCCHHCCC";
                mapa[42] = "CNCCNNHHHCCNCCNNHHHCCNCCNNHHHCCNCCNNHHHCCNCCNNHHHCCNCCNNHHHCCNCCNNHHHCCNCCNNHHHCCNCCNNHHHCCNCCNNHHHC";
                mapa[43] = "CHHHCCCCCCCHHHCCCCCCCHHHCCCCCCCHHHCCCCCCCHHHCCCCCCCHHHCCCCCCCHHHCCCCCCCHHHCCCCCCCHHHCCCCCCCHHHCCCCCC";
                mapa[44] = "CCCCCNHHHHCCCCCNHHHHCCCCCNHHHHCCCCCNHHHHCCCCCNHHHHCCCCCNHHHHCCCCCNHHHHCCCCCNHHHHCCCCCNHHHHCCCCCNHHHH";
                mapa[45] = "CCHCCCNNNNCCHCCCNNNNCCHCCCNNNNCCHCCCNNNNCCHCCCNNNNCCHCCCNNNNCCHCCCNNNNCCHCCCNNNNCCHCCCNNNNCCHCCCNNNN";
                mapa[46] = "NNNNNHCCCCNNNNNHCCCCNNNNNHCCCCNNNNNHCCCCNNNNNHCCCCNNNNNHCCCCNNNNNHCCCCNNNNNHCCCCNNNNNHCCCCNNNNNHCCCC";
                mapa[47] = "CCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCC";
                mapa[48] = "CCCNNHHHHHCCCNNHHHHHCCCNNHHHHHCCCNNHHHHHCCCNNHHHHHCCCNNHHHHHCCCNNHHHHHCCCNNHHHHHCCCNNHHHHHCCCNNHHHHH";
                mapa[49] = "HHHCCCCCCCHHHCCCCCCCHHHCCCCCCCHHHCCCCCCCHHHCCCCCCCHHHCCCCCCCHHHCCCCCCCHHHCCCCCCCHHHCCCCCCCHHHCCCCCCC";
                mapa[50] = "CCHCNHCCHNCCHCNHCCHNCCHCNHCCHNCCHCNHCCHNCCHCNHCCHNCCHCNHCCHNCCHCNHCCHNCCHCNHCCHNCCHCNHCCHNCCHCNHCCHN";
                mapa[51] = "NNCCCHHCCCNNCCCHHCCCNNCCCHHCCCNNCCCHHCCCNNCCCHHCCCNNCCCHHCCCNNCCCHHCCCNNCCCHHCCCNNCCCHHCCCNNCCCHHCCC";
                mapa[52] = "CNCCNNHHHCCNCCNNHHHCCNCCNNHHHCCNCCNNHHHCCNCCNNHHHCCNCCNNHHHCCNCCNNHHHCCNCCNNHHHCCNCCNNHHHCCNCCNNHHHC";
                mapa[53] = "CHHHCCCCCCCHHHCCCCCCCHHHCCCCCCCHHHCCCCCCCHHHCCCCCCCHHHCCCCCCCHHHCCCCCCCHHHCCCCCCCHHHCCCCCCCHHHCCCCCC";
                mapa[54] = "CCCCCNHHHHCCCCCNHHHHCCCCCNHHHHCCCCCNHHHHCCCCCNHHHHCCCCCNHHHHCCCCCNHHHHCCCCCNHHHHCCCCCNHHHHCCCCCNHHHH";
                mapa[55] = "CCHCCCNNNNCCHCCCNNNNCCHCCPNNNNCCHCCCNNNNCCHCCCNNNNCCHCCCNNNNCCHCCCNNNNCCHCCCNNNNCCHCCCNNNNCCHCCCNNNN";
                mapa[56] = "NNNNNHCCCCNNNNNHCCCCNNNNNHCCCCNNNNNHCCCCNNNNNHCCCCNNNNNHCCCCNNNNNHCCCCNNNNNHCCCCNNNNNHCCCCNNNNNHCCCC";
                mapa[57] = "CCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCC";
                mapa[58] = "CCCNNHHHHHCCCNNHHHHHCCCNNHHHHHCCCNNHHHHHCCCNNHHHHHCCCNNHHHHHCCCNNHHHHHCCCNNHHHHHCCCNNHHHHHCCCNNHHHHH";
                mapa[59] = "HHHCCCCCCCHHHCCCCCCCHHHCCCCCCCHHHCCCCCCCHHHCCCCCCCHHHCCCCCCCHHHCCCCCCCHHHCCCCCCCHHHCCCCCCCHHHCCCCCCC";
                mapa[60] = "CCHCNHCCHNCCHCNHCCHNCCHCNHCCHNCCHCNHCCHNCCHCNHCCHNCCHCNHCCHNCCHCNHCCHNCCHCNHCCHNCCHCNHCCHNCCHCNHCCHN";
                mapa[61] = "NNCCCHHCCCNNCCCHHCCCNNCCCHHCCCNNCCCHHCCCNNCCCHHCCCNNCCCHHCCCNNCCCHHCCCNNCCCHHCCCNNCCCHHCCCNNCCCHHCCC";
                mapa[62] = "CNCCNNHHHCCNCCNNHHHCCNCCNNHHHCCNCCNNHHHCCNCCNNHHHCCNCCNNHHHCCNCCNNHHHCCNCCNNHHHCCNCCNNHHHCCNCCNNHHHC";
                mapa[63] = "CHHHCCCCCCCHHHCCCCCCCHHHCCCCCCCHHHCCCCCCCHHHCCCCCCCHHHCCCCCCCHHHCCCCCCCHHHCCCCCCCHHHPCCCCCCHHHCCCCCC";
                mapa[64] = "CCCCCNHHHHCCCCCNHHHHCCCCCNHHHHCCCCCNHHHHCCCCCNHHHHCCCCCNHHHHCCCCCNHHHHCCCCCNHHHHCCCCCNHHHHCCCCCNHHHH";
                mapa[65] = "CCHCCCNNNNCCHCCCNNNNCCHCCCNNNNCCHCCCNNNNCCHCCCNNNNCCHCCCNNNNCCHCCCNNNNCCHCCCNNNNCCHCCCNNNNCCHCCCNNNN";
                mapa[66] = "NNNNNHCCCCNNNNNHCCCCNNNNNHCCCCNNNNNHCCCCNNNNNHCCCCNNNNNHCCCCNNNNNHCCCCNNNNNHCCCCNNNNNHCCCCNNNNNHCCCC";
                mapa[67] = "CCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCC";
                mapa[68] = "CCCNNHHHHHCCCNNHHHHHCCCNNHHHHHCCCNNHHHHHCCCNNHHHHHCCCNNHHHHHCCCNNHHHHHCCCNNHHHHHCCCNNHHHHHCCCNNHHHHH";
                mapa[69] = "HHHCCCCCCCHHHCCCCCCCHHHCCCCCCCHHHCCCCCCCHHHCCCCCCCHHHCCCCCCCHHHCCCCCCCHHHCCCCCCCHHHCCCCCCCHHHCCCCCCC";
                mapa[70] = "CCHCNHCCHNCCHCNHCCHNCCHCNHCCHNCCHCNHCCHNCCHCNHCCHNCCHCNHCCHNCCHCNHCCHNCCHCNHCCHNCCHCNHCCHNCCHCNHCCHN";
                mapa[71] = "NNCCCHHCCCNNCCCHHCCCNNCCCHHCCCNNCCCHHCCCNNCCCHHCCCNNCCCHHCCCNNCCCHHCCCNNCCCHHCCCNNCCCHHCCCNNCCCHHCCC";
                mapa[72] = "CNCCNNHHHCCNCCNNHHHCCNCCNNHHHCCNCCNNHHHCCNCCNNHHHCCNCCNNHHHCCNCCNNHHHCCNCCNNHHHCCNCCNNHHHCCNCCNNHHHC";
                mapa[73] = "CHHHCCCCCCCHHHCCCCCCCHHHCCCCCCCHHHCCCCCCCHHHCCCCCCCHHHCCCCCCCHHHCCCCCCCHHHCCCCCCCHHHCCCCCCCHHHCCCCCC";
                mapa[74] = "CCCCCNHHHHCCCCCNHHHHCCCCCNHHHHCCCCCNHHHHCCCCCNHHHHCCPCCNHHHHCCCCCNHHHHCCCCCNHHHHCCCCCNHHHHCCCCCNHHHH";
                mapa[75] = "CCHCCCNNNNCCHCCCNNNNCCHCCCNNNNCCHCCCNNNNCCHCCCNNNNCCHCCCNNNNCCHCCCNNNNCCHCCCNNNNCCHCCCNNNNCCHCCCNNNN";
                mapa[76] = "NNNNNHCCCCNNNNNHCCCCNNNNNHCCCCNNNNNHCCCCNNNNNHCCCCNNNNNHCCCCNNNNNHCCCCNNNNNHCCCCNNNNNHCCCCNNNNNHCCCC";
                mapa[77] = "CCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCC";
                mapa[78] = "CCCNNHHHHHCCCNNHHHHHCCCNNHHHHHCCCNNHHHHHCCCNNHHHHHCCCNNHHHHHCCCNNHHHHHCCCNNHHHHHCCCNNHHHHHCCCNNHHHHH";
                mapa[79] = "HHHCCCCCCCHHHCCCCCCCHHHCCCCCCCHHHCCCCCCCHHHCCCCCCCHHHCCCCCCCHHHCCCCCCCHHHCCCCCCCHHHCCCCCCCHHHCCCCCCC";
                mapa[80] = "CCHCNHCCHNCCHCNHCCHNCCHCNHCCHNCCHCNHCCHNCCHCNHCCHNCCHCNHCCHNCCHCNHCCHNCCHCNHCCHNCCHCNHCCHNCCHCNHCCHN";
                mapa[81] = "NNCCCHHCCCNNCCCHHCCCNNCCCHHCCCNNCCCHHCCCNNCCCHHCCCNNCCCHHCCCNNCCCHHCCCNNCCCHHCCCNNCCCHHCCCNNCCCHHCCC";
                mapa[82] = "CNCCNNHHHCCNCCNNHHHCCNCCNNHHHCCNCCNNHHHCCNCCNNHHHCCNCCNNHHHCCNCCNNHHHCCNCCNNHHHCCNCCNNHHHCCNCCNNHHHC";
                mapa[83] = "CHHHCCCCCCCHHHCCCCCCCHHHCCCCCCCHHHCCCCCCCHHHCCCCCCCHHHCCCCCCCHHHCCCCCCCHHHCCCCCCCHHHCCCCCCCHHHCCCCCC";
                mapa[84] = "CCCCCNHHHHCCCCCNHHHHCCCCCNHHHHCCCCCNHHHHCCCCCNHHHHCCCCCNHHHHCCCCCNHHHHCCCCCNHHHHCCCCCNHHHHCCCCCNHHHH";
                mapa[85] = "CCHCCCNNNNCCHCCCNNNNCCHCCCNNNNCCHCCCNNNNCCHCCCNNNNCCHCCCNNNNCCHCCCNNNNCCHCCCNNNNCCHCCCNNNNCCHCCCNNNN";
                mapa[86] = "NNNNNHCCCCNNNNNHCCCCNNNNNHCCCCNNNNNHCCCCNNNNNHCCCCNNNNNHCCCCNNNNNHCCCCNNNNNHCCCCNNNNNHCCCCNNNNNHCCCC";
                mapa[87] = "CCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCC";
                mapa[88] = "CCCNNHHHHHCCCNNHHHHHCCCNNHHHHHCCCNNHHHHHCCCNNHHHHHCCCNNHHHHHCCCNNHHHHHCCCNNHHHHHCCCNNHHHHHCCCNNHHHHH";
                mapa[89] = "HHHCCCCCCCHHHCCCCCCCHHHCCCCCCCHHHCCCCCCCHHHCCCCCCCHHHCCCCCCCHHHCCCCCCCHHHCCCCCCCHHHCCCCCCCHHHCCCCCCC";
                mapa[90] = "CCHCNHCCHNCCHCNHCCHNCCHCNHCCHNCCHCNHCCHNCCHCNHCCHNCCHCNHCCHNCCHCNHCCHNCCHCNHCCHNCCHDNHCCHNCCHCNHCCHN";
                mapa[91] = "NNCCCHHCCCNNCCCHHCCCNNCCCHHCCCNNCCCHHCCCNNCCCHHCCCNNCCCHHCCCNNCCCHHCCCNNCCCHHCCCNNCCCHHCCCNNCCCHHCCC";
                mapa[92] = "CNCCNNHHHCCNCCNNHHHCCNCCNNHHHCCNCCNNHHHCCNCCNNHHHCCNCCNNHHHCCNCCNNHHHCCNCCNNHHHCCNCCNNHHHCCNCCNNHHHC";
                mapa[93] = "CHHHCCCCCCCHHHCCCCCCCHHHCCCCCCCHHHCCCCCCCHHHCCCCCCCHHHCCCCCCCHHHCCCCCCCHHHCCCCCCCHHHCCCCCCCHHHCCCCCC";
                mapa[94] = "CCCCCNHHHHCCCCCNHHHHCCCCCNHHHHCCCCCNHHHHCCCCCNHHHHCCCCCNHHHHCCCCCNHHHHCCCCCNHHHHCCCCCNHHHHCCCCCNHHHH";
                mapa[95] = "CCHCCCNNNNCCHCCCNNNNCCHCCCNNNNCCHCCCNNNNCCHCCCNNNNCCHCCCNNNNCCHCCCNNNNCCHCCCNNNNCCHCCCNNNNCCHCCCNNNN";
                mapa[96] = "NNNNNHCCCCNNNNNHCCCCNNNNNHCCCCNNNNNHCCCCNNNNNHCCCCNNNNNHCCCCNNNNNHCCCCNNNNNHCCCCNNNNNHCCCCNNNNNHCCCC";
                mapa[97] = "CCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCC";
                mapa[98] = "CCCNNHHHHHCCCNNHHHHHCCCNNHHHHHCCCNNHHHHHCCCNNHHHHHCCCNNHHHHHCCCNNHHHHHCCCNNHHHHHCCCNNHHHHHCCCNNHHHHH";
                mapa[99] = "HHHCCCCCCCHHHCCCCCCCHHHCCCCCCCHHHCCCCCCCHHHCCCCCCCHHHCCCCCCCHHHCCCCCCCHHHCCCCCCCHHHCCCCCCCHHHCCCCCCC";
                cesta = zachran_princezne(mapa, n, m, t, &dlzka_cesty);
                break;
            case 15:// Zablokovany zaciatok
                n = 7;
                m = 7;
                t = 50;
                mapa = (char**)malloc(n*sizeof(char*));
                mapa[0]="NCCHHCH";
                mapa[1]="CCHHHCP";
                mapa[2]="HCDCHHC";
                mapa[3]="PHPCHHC";
                mapa[4]="HCHHCCC";
                mapa[5]="HCHHCHH";
                mapa[6]="PCHCHHP";
                cesta = zachran_princezne(mapa, n, m, t, &dlzka_cesty);
                break;
            case 16:
                n = 99;
                m = 1;
                t = 1000;
                mapa = (char**) malloc (n * sizeof(char *));
                mapa[0] =  "P";
                mapa[1] =  "H";
                mapa[2] =  "C";
                mapa[3] =  "C";
                mapa[4] =  "C";
                mapa[5] =  "H";
                mapa[6] =  "C";
                mapa[7] =  "H";
                mapa[8] =  "H";
                mapa[9] =  "C";
                mapa[10] = "C";
                mapa[11] = "C";
                mapa[12] = "C";
                mapa[13] = "H";
                mapa[14] = "H";
                mapa[15] = "C";
                mapa[16] = "C";
                mapa[17] = "C";
                mapa[18] = "H";
                mapa[19] = "C";
                mapa[20] = "C";
                mapa[21] = "P";
                mapa[22] = "C";
                mapa[23] = "H";
                mapa[24] = "C";
                mapa[25] = "H";
                mapa[26] = "C";
                mapa[27] = "C";
                mapa[28] = "C";
                mapa[29] = "C";
                mapa[30] = "H";
                mapa[31] = "H";
                mapa[32] = "H";
                mapa[33] = "C";
                mapa[34] = "H";
                mapa[35] = "H";
                mapa[36] = "H";
                mapa[37] = "C";
                mapa[38] = "C";
                mapa[39] = "C";
                mapa[40] = "H";
                mapa[41] = "H";
                mapa[42] = "C";
                mapa[43] = "C";
                mapa[44] = "H";
                mapa[45] = "H";
                mapa[46] = "C";
                mapa[47] = "C";
                mapa[48] = "H";
                mapa[49] = "C";
                mapa[50] = "H";
                mapa[51] = "H";
                mapa[52] = "H";
                mapa[53] = "C";
                mapa[54] = "C";
                mapa[55] = "C";
                mapa[56] = "H";
                mapa[57] = "C";
                mapa[58] = "H";
                mapa[59] = "H";
                mapa[60] = "D";
                mapa[61] = "P";
                mapa[62] = "C";
                mapa[63] = "C";
                mapa[64] = "C";
                mapa[65] = "P";
                mapa[66] = "C";
                mapa[67] = "C";
                mapa[68] = "C";
                mapa[69] = "C";
                mapa[70] = "H";
                mapa[71] = "H";
                mapa[72] = "C";
                mapa[73] = "C";
                mapa[74] = "C";
                mapa[75] = "C";
                mapa[76] = "H";
                mapa[77] = "H";
                mapa[78] = "H";
                mapa[79] = "H";
                mapa[80] = "C";
                mapa[81] = "C";
                mapa[82] = "H";
                mapa[83] = "C";
                mapa[84] = "C";
                mapa[85] = "H";
                mapa[86] = "H";
                mapa[87] = "C";
                mapa[88] = "C";
                mapa[89] = "P";
                mapa[90] = "N";
                mapa[91] = "C";
                mapa[92] = "H";
                mapa[93] = "H";
                mapa[94] = "C";
                mapa[95] = "C";
                mapa[96] = "C";
                mapa[97] = "C";
                mapa[98] = "C";
                cesta = zachran_princezne(mapa, n, m, t, &dlzka_cesty);
                break;
            case 17:

                n = 33;;
                m = 2;
                t = 1000;
                mapa = (char**) malloc (n * sizeof(char *));
                mapa[0] =  "CC";
                mapa[1] =  "CN";
                mapa[2] =  "CC";
                mapa[3] =  "NC";
                mapa[4] =  "CC";
                mapa[5] =  "CN";
                mapa[6] =  "CC";
                mapa[7] =  "NC";
                mapa[8] =  "CC";
                mapa[9] =  "NC";
                mapa[10] = "CC";
                mapa[11] = "HC";
                mapa[12] = "CC";
                mapa[13] = "CH";
                mapa[14] = "CC";
                mapa[15] = "HN";
                mapa[16] = "CC";
                mapa[17] = "CC";
                mapa[18] = "CC";
                mapa[19] = "CC";
                mapa[20] = "CC";
                mapa[21] = "CC";
                mapa[22] = "PC";
                mapa[23] = "CC";
                mapa[24] = "PC";
                mapa[25] = "CC";
                mapa[26] = "DC";
                mapa[27] = "CC";
                mapa[28] = "PC";
                mapa[29] = "CC";
                mapa[30] = "PC";
                mapa[31] = "CC";
                mapa[32] = "PC";
                cesta = zachran_princezne(mapa, n, m, t, &dlzka_cesty);
                break;
            case 18:
                n = 44;
                m = 69;
                t = 1000;
                mapa = (char**) malloc (n * sizeof(char *));
                mapa[0] = "CCHHCHHHHCCHCCCCCHCCCCHNCCNCCHCCNHCCCCNCCCCCHCCCCCCHCCCCCCCCCNHCCCCHC";
                mapa[1] = "CCCHCCCNCNCCCNCHHCNHHNCCCNHCHNHHHCNHHCCHCCHNHCHCCNCCCCNCCCHNCNCCHCCNC";
                mapa[2] = "NNHCCNCHHHCCNCCCNCCCNHCHNCNHCCCHCNCCCHCNCCNCCCCCHNNCNCCNHHCNCCCNHCCCH";
                mapa[3] = "NNCHCCCCNCCCCCHNCCCCCHCCHCCCCHNCHCNNCCCCHCCNCNHCCCCCHHCCCCCNCHHCCNCHH";
                mapa[4] = "CCCHCCCCCNCNCHHCHHHCCNHNHNCHCCCCCCCCCCHCCCHHCNCCHNCHCCHNCNHNNNHHCHHHC";
                mapa[5] = "CHCHNCCHCCCNHCNHNHCCHCHHCCHCCNNCNCNCCCCCCHNCCHHCNCCHNNCHCHHCNCCHNCCNC";
                mapa[6] = "NCNCHHCNCCHNCHHHNCNHNNCCCHCCCCCNCCCHCCHNNNCHCCHHCHCCCNCCHCCCCCCCCHNCC";
                mapa[7] = "CCCCHCCHHCHCHHHCNNCCCHCCCNCCNHCHCNCCCNNNHCCCCCCCNHCCCCNCHCCCCCCCNCCCC";
                mapa[8] = "CCCCCCCCCCCHHCHHCCCCCCHNNHNCCNCCCCCCCNCCCCNCHHCHCCHCNCCCNCNCCHCHCNNNC";
                mapa[9] = "HNCCNNCHNCNCCCNCCCCCCNHCCNNCCHNCNHNCNNHCHCCCCNCCCHCCCHCCCCHHHCCNCCCCC";
                mapa[10] = "CCCNCNHCHHCNCCNNCCHHHCHCCHNNPCHCCNHHCNCCHHCHCHCCHCNNCCCNCCHCCCCHNCCNC";
                mapa[11] = "CNCHCCCCCCCCCCNCCNNCHCCHCNCNHCCHNCCCCCNHHNCCHCCCHHCCNCCHCCNCCCNHNNNNC";
                mapa[12] = "CHCHNCCCHNNCCNCCHCCNCCNCHNNCCNCCCHHCCCCHCCCCCCCCHNCHNCCCCCCCHCCCCCCCC";
                mapa[13] = "HCNCNHCCCCCHHNCNNHCCNHCCNHHCNCCCNHHNCNCCCHNNNNCHCCCNCHHCCCCCHNCCCCCCH";
                mapa[14] = "HHNCCCCCCNCNNCCCHHCCCHCPHCNNCCCHNHNCCCCNNCCCCCCCCNCCCCNHCCNNCCNCCCCHC";
                mapa[15] = "CHNCCCNNNHCCNHCCNCNNNHCNCCCHHCCCNNCHNNCCHHCCCCNNHCCCNNNCCHHHCHNCCNCNC";
                mapa[16] = "CHCCHCNHNCCCCNCCCCCCNNCHHCCCCNCCCCCCCCCCCCNHCNCCCNCCNCCNCNCCCCCCCCHHC";
                mapa[17] = "CNNHNCCCCCNCHHCCCCHCHCCNNNCCHCCCCNHCCNCNNHHNCCCCCCCNHCCCCCCNCCCCHCCHN";
                mapa[18] = "NCCNCCCCHCCCHHCHCCNCCNHCNCHCNCCHHHNCNCCCCNNCNCHHCCCNNCHCCCHNNCCNNPNCC";
                mapa[19] = "CHNCCCHCNHCCHCNCCCCNNCCCCHNHHCCCNCHCNCCCHCCNNCHCNCCCCCNNCCCCCNCCCCCCH";
                mapa[20] = "CCCCCCCHCHHCNHHCCCCHNHCCCCHCNCHCHHCNCCCCCHCCCNCCCNHHCHCHCCHCCCCCNCCCC";
                mapa[21] = "CCCHCCCNCCCCCCCCCCHNCHCNCCCCCNNCCHCCCCNNCCCHNNCNCCCCNNHCHCCNCNCCNCCNC";
                mapa[22] = "CNNCCHNCCCCNCNCCCCNNCCHCCCCCCNCCCNHNCNNNCNCCNHCCCHNCNCCCDCHCCCNCCNCCN";
                mapa[23] = "CHHCCCCHCHCCCHCCHNCCNHCCCNCNHCCHCHCNCCCCNCCCHCHCCCCCCHCCNCCCHCCHCNHHN";
                mapa[24] = "HCCHCCCCNCCNNCCNCCNHCHCCHNCCCCCHCCNHCCCHCHCCCCHCCHCNCNHCCHCCCCNCNHCCC";
                mapa[25] = "CNCHHCCCCNCNHHCNNNCHCCCNCHCCCCCHNCCCCNCHCNNNNCCCCCCNCHCCHNNNCHCCHCHCC";
                mapa[26] = "HCCHCCCNHNNCCNCNCCCCCNNHHCCCNCCCHHHCCNCCCHCCNCCHCCNHCNNCNCCCCCCCHCCCC";
                mapa[27] = "HCCCCCCCCNCCCCCCCHCNNCCCCCHCCHHCCHNHCCHCCHCNCCCHNNCHCNCCCCNNCNCCCCHCH";
                mapa[28] = "CCNNCCNCCCHHHCCCCCCNHHNCCHHCHCCCHCCNCHCCHCHNCCCCCCNCCCCCCCCHCNCCNNCCC";
                mapa[29] = "CCCHCCCNHCCCNCHHCCCNHCHNCNCCCCCNHCCCCCCCCCHNCCHCHNCNCCHCCCCCCCCCHCNCN";
                mapa[30] = "CCHHHHCCCNHCNCCCHCHCHCCCHCCNCHNCCCNCCCCCCCCHCCHCCNCNCHNCCNCCCNCCNCCNC";
                mapa[31] = "CCNCNNHCCNCHCCCCCCHCCHHCCHNNCCNCCCHCNHHHHCCCHCNNCCNCHNCCCCCCCCCCHCNCC";
                mapa[32] = "CCCNHCCHCNCNCNCHCCHCCNHNCCCHNHHCHCCCHCCCNCCCCCCCCNNCCNCCNHNHCCCCCCHCH";
                mapa[33] = "CNHNHNNCCHCHCPHCCCHHCCCHHHCCCHCCNCNCCNCCHCCCHCCHNCCCCCHHCCCNHCNNCNNCC";
                mapa[34] = "NCCNNCNNHCHCCHCCCHCCCCCNCCCCNCCCNHHCHCCCNCCHNNCCCNHCHHCCCNCNNNCCCCCCC";
                mapa[35] = "CCCCCHHCHCHCCCCHHHHNCCCCNCHCCHCNNCCHCCCNHCNCNCCHCNCCCHCHNCHHCHNCCCCCC";
                mapa[36] = "HCNCCNNNNHHCCCCCNCCCCCCCCNHCHCCCCCCCNNCCCNCCCCHCHCCNCCCNCHCCCCCNHNCCC";
                mapa[37] = "NHCHNCHCNNCCCCCHNNNNCCNCHCCNHCHCNHCCCCHNNHCNNCNCCCNNHHCHNHCCCCNHNNCCC";
                mapa[38] = "CCCCNCNHCCHCHCCNNCCCCHCNHCCCCCCCCNCCHCCCCCCCCNCCCCCCCCCCHCCCNCNCNCHCC";
                mapa[39] = "HNCNCCNHCCCHCCCCNCCCCCCNHHNCHNHCCCHCHCCCHHCCHCNCNCCNCCCHHCNCNHHCCCHCH";
                mapa[40] = "CCCCCNHCCCHCCHCHCNCCCHCHNCCNHNNNCCCCHCCCCHCNNCHNNCHCNNCNCCNCHNCCHCCHC";
                mapa[41] = "CCCHCNCNCHCCCCCCHCCCHCNCCNNCCCCHNCNCNHCHCNHNNCCCCCCCCCNCHCHCCCNNCCCNC";
                mapa[42] = "CNCCCCCCNCCHNNHCNCHCHNCCNCCCNCNCCCHCNNCCNCCCCCHCCCCCHHCCNCCCCHCCHHCCC";
                mapa[43] = "NCCCCHCCNHCCNCHCHCCCHCCNHCNCCCCNCNCCNCCCHCNNCCCHCCCNCCCCCCHCCCNHCCNNH";
                cesta = zachran_princezne(mapa, n, m, t, &dlzka_cesty);
                break;
            case 19://Randome vygenerovane mapy
                printf("Zadajte pocet stlpcov a pocet riadkov:\n");
                scanf("%d %d", &n, &m);
                t = 1000000;
                srand(time(0));
                mapa = (char**) malloc (n * sizeof(char *));
                for(int i = 0; i < n; ++i)
                {
                    mapa[i] = (char*)calloc(n, 1);
                }
                vlozenyDrak = 0;
                pocetPrinceznien = 0;
                //char slovo[100] = "ABC";
                //strcat(slovo, c);
                //strncat(mapa[0], &c, 1);
                //strcpy(*mapa[i],"AAAAA");
                miestopreDrakaX = rand() % (m-1);
                miestopreDrakaY = rand() % (n-1);
                //printf("%d", miestopreDraka);
                for(int i = 0; i < n; ++i)
                {
                    //strncat(mapa[i], &n, 1);
                    for(int j =0; j < m; ++j)
                    {
                        cislo = rand() % 100;
                        if(i == miestopreDrakaY && j == miestopreDrakaX)
                        {
                            strcat(mapa[i],d);
                        }
                        else if(cislo + 6*pocetPrinceznien < 30)
                        {
                            strcat(mapa[i], p);
                            ++pocetPrinceznien;
                        }
                        else if(cislo < 15)
                        {
                            strcat(mapa[i], nn);
                        }
                        else if(cislo < 45)
                        {
                            strcat(mapa[i], h);
                        }
                        else
                        {
                            strcat(mapa[i], c);
                        }
                    }
                    //strncat(mapa[i], &end, 1);
                }
                printf("Nasa mapa:\n");
                for(int i = 0; i < n; ++i)
                {
                    printf("%s\n",mapa[i]);
                }
                cesta = zachran_princezne(mapa, n, m, t, &dlzka_cesty);
                break;
            case 20://Mapa s drakom na zaciatku
                n = 4;
                m = 5;
                t = 12;
                mapa = (char**)malloc(n*sizeof(char*));
                mapa[0]="DCCCC";
                mapa[1]="CCCCC";
                mapa[2]="CCNCC";
                mapa[3]="CCNCP";
                cesta = zachran_princezne(mapa, n, m, t, &dlzka_cesty);
                break;

            case 21://Mapa s princeznou na zaciatku
                n = 4;
                m = 5;
                t = 12;
                mapa = (char**)malloc(n*sizeof(char*));
                mapa[0]="PCCCC";
                mapa[1]="CCCCC";
                mapa[2]="CCDCC";
                mapa[3]="CCNCP";
                cesta = zachran_princezne(mapa, n, m, t, &dlzka_cesty);
                break;
            case 22:// Test cien drahy
                n = 2;
                m = 6;
                t = 12;
                mapa[0] ="CHHHCD";
                mapa[1] ="CCCCCP";
                cesta = zachran_princezne(mapa, n, m, t, &dlzka_cesty);
                break;
            default:
                continue;
        }
        cas = 0;
        for(i=0; i<dlzka_cesty; i++){
            printf("%d %d\n", cesta[i*2], cesta[i*2+1]);
            if(mapa[cesta[i*2+1]][cesta[i*2]] == 'H')
                cas+=2;
            else
                cas+=1;
            if(mapa[cesta[i*2+1]][cesta[i*2]] == 'D' && cas > t)
                printf("Nestihol si zabit draka!\n");
            if(mapa[cesta[i*2+1]][cesta[i*2]] == 'N')
                printf("Prechod cez nepriechodnu prekazku!\n");
            if(i>0 && abs(cesta[i*2+1]-cesta[(i-1)*2+1])+abs(cesta[i*2]-cesta[(i-1)*2])>1)
                printf("Neplatny posun Popolvara!\n");
        }
        printf("%d\n",cas);
        free(cesta);
        for(i=0; i<n; i++){
            free(mapa[i]);
        }
        free(mapa);
    }
    return 0;
}
