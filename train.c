#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define MAX_LINII 100
#define MAX_CARACTERE 100
#define CARACTERE 1000
typedef char Tip;

//structura pentru tren
typedef struct Vagon{
    Tip value;
    struct Vagon *next;
    struct Vagon *prev;
} Vagon;

typedef struct Tren{
    Vagon *santinela; //locomotiva
    Vagon *mecanic;
} Tren;

//structura pentru coada
typedef struct Comanda{
    struct Comanda *prev;
    struct Comanda *next;
    char functie[CARACTERE];
} Comanda;

typedef struct Coada{
    Comanda *primul; 
    Comanda *ultimul; 
} Coada;

//santinela
Vagon *initSantinela(){
    Vagon *santinela=(Vagon*)calloc(1,sizeof(Vagon));
    Vagon *vagon=(Vagon*)calloc(1,sizeof(Vagon));
    santinela->next=vagon;
    vagon->prev=santinela;
    santinela->prev=vagon;
    vagon->next=santinela;
    vagon->value='#'; 
    return santinela;              
}

//init tren
Tren* initTren(){
    Tren *tren=(Tren*)calloc(1,sizeof(Tren));
    Vagon *santinela= initSantinela();
    tren->mecanic=santinela->next;
    tren->santinela=santinela;
    return tren;
}

//init coada  
Coada* initCoada(){
    Coada *coada=(Coada*)calloc(1,sizeof(Coada));
    return coada;
}

//init comanda
Comanda* initComanda(){
    Comanda *comanda=(Comanda*)calloc(1,sizeof(Comanda));
    return comanda;
}

//UPDATE

void MOVE_LEFT(Tren *tren){
    if (tren == NULL) {
        return;
    }
    if (tren->mecanic->prev==tren->santinela){
        tren->mecanic=tren->santinela->prev;
        return;
    }
    tren->mecanic=tren->mecanic->prev;
}

void MOVE_RIGHT(Tren *tren){
    if (tren == NULL) {
        return;
    }
    if (tren->mecanic->next==tren->santinela){
        Vagon *vagon=(Vagon*)calloc(1,sizeof(Vagon));
        if (!vagon) {
            return;
        }
        vagon->value='#';
        tren->mecanic->next=vagon;
        vagon->next=tren->santinela;
        tren->santinela->prev=vagon;
        vagon->prev=tren->mecanic;
        tren->mecanic=vagon;  
        return;            
    }
    tren->mecanic=tren->mecanic->next;
}

void WRITE(Tren *tren, Tip elem){
    if (tren == NULL) {
        return;
    }
    tren->mecanic->value=elem;
}

void CLEAR_CELL(Tren *tren){
    if (tren == NULL) {
        return;
    }
    Vagon *vagon=tren->mecanic;
    if(vagon->prev==tren->santinela && vagon->next==tren->santinela){
        vagon->value='#';   
        return;             //daca trenul avea doar un vagon
    } else if(vagon->prev==tren->santinela){
        tren->santinela->next=vagon->next;
        vagon->next->prev=tren->santinela;
        tren->mecanic=tren->santinela->prev;
        free(vagon);       
        return;             //daca vagonul mecanicului e primul vagon
    }else if (vagon->next==tren->santinela){
        tren->santinela->prev=vagon->prev;
        vagon->prev->next=tren->santinela;
        tren->mecanic=tren->santinela->next;
        free(vagon);
        return;             //daca vagonul mecanicului e ultimul vagon
    }  
    //daca vagonul mecanicului e orice vagon inafara de primul si ultimul
    vagon->prev->next=vagon->next;
    vagon->next->prev=vagon->prev;
    tren->mecanic=vagon->prev; 
    free(vagon);
}

void CLEAR_ALL(Tren *tren){
    if (tren == NULL) {
        return;
    }
    //parcurg vagon cu vagon si le dezaloc
    while (tren->mecanic->next != tren->santinela || tren->mecanic->prev != tren->santinela) {
        tren->mecanic = tren->santinela->next; 
        CLEAR_CELL(tren); 
    } 
    CLEAR_CELL(tren);
}

void INSERT_LEFT(Tren *tren,Tip elem,FILE *OUT){
    if (tren == NULL) {
        return;
    }
    Vagon *exVagon=tren->mecanic;
    //verific daca mecanicul e in stanga trenului langa santinela
    if(exVagon->prev==tren->santinela){
        fprintf(OUT,"ERROR\n");
        return;
    }
    Vagon *vagonNou=(Vagon*)malloc(sizeof(Vagon));
    vagonNou->prev=exVagon->prev;
    vagonNou->next=exVagon;
    exVagon->prev->next = vagonNou;
    exVagon->prev=vagonNou;
    vagonNou->value=elem;
    tren->mecanic = vagonNou;
}

void INSERT_RIGHT(Tren *tren, Tip elem){
    if (tren == NULL) {
        return;
    }
    Vagon *exVagon=tren->mecanic;
    Vagon *vagonNou=(Vagon*)malloc(sizeof(Vagon));
    //verific daca mecanicul e in dreapta trenului langa santinela
    if (exVagon->next==tren->santinela){
        exVagon->next=vagonNou;
        vagonNou->prev=exVagon;
        vagonNou->next=tren->santinela;
        tren->santinela->prev=vagonNou;
        vagonNou->value=elem;
        tren->mecanic=vagonNou;
        return;                  //daca adaug ultimul vagon--^
    }
    vagonNou->next=exVagon->next;
    vagonNou->prev=exVagon;
    exVagon->next->prev=vagonNou;
    exVagon->next=vagonNou;
    vagonNou->value=elem;
    tren->mecanic=vagonNou;
}

// SEARCH
void SEARCH(Tren *tren, char s[], FILE* OUT) {
    if (tren == NULL) {
        return;
    }
    Vagon *iter = tren->mecanic;
    int lungime_sir = strlen(s);
    int sir_gasit = 0;
    Vagon *contor_iter;
    int o_rotatie=0;
    //parcurg trenul pana cand nu gasesc sirul sau pana nu fac o rotatie completa pana la mecanic
    while (sir_gasit==0 && (o_rotatie!=1 || iter != tren->mecanic )) {
        int i; // este resetat la fiecare iteratie
        contor_iter=iter;
        sir_gasit = 1; 
        //are loc doar daca incepem sa gasim caracterele din sir
        for (i=0; i < lungime_sir; i++) {
            //adaugam variabila cuurent pentru a itera cu ea prin sir pentru a nu pierde variabila iter...
            //...care pastreaza pozitial initiala de verificare a sirului
            if (contor_iter == tren->santinela) {
                //deoarece trebuie sa trecem print toata lista o data pana nu ajungem inapoi la mecanic
                contor_iter = contor_iter->next;
            }
            if (contor_iter->value != s[i]) {
                sir_gasit = 0; 
                break;
            }
            contor_iter = contor_iter->next;
        }
        if (sir_gasit==1) {
            tren->mecanic = iter; 
            break;
        }
        iter = iter->next;
        if (iter == tren->santinela) {
            iter = iter->next; 
        }
        // daca a facut o rotatie completa pana inapoi la mecanic
        if (iter == tren->mecanic) {
            o_rotatie=1;
        }
    }
    if (sir_gasit==0) {
        fprintf(OUT, "ERROR\n"); 
    }else {
        tren->mecanic = iter; 
    }
}

void SEARCH_LEFT(Tren *tren, char s[],FILE* OUT){
    if (tren == NULL) {
        return;
    }
    Vagon *iter=tren->mecanic;
    int i=0; //contorul pentru sirul cautat
    int lungime_sir=strlen(s);
    int sir_gasit=0; 
    Vagon *pozitie_initiala_mecanic=tren->mecanic;
    //parcurg tot trenul pana nu gasesc sirul sau pana nu ajung la santinela
    while(s[i]!='\0' && iter->prev!=tren->santinela){
        if(iter->value==s[i]){
            i++;
            //are loc doar daca incepem sa gasim caracterele din sir
            if(i==lungime_sir){
                sir_gasit=1;
                tren->mecanic=iter;
                break;
            }
        } else{
            i=0;  //resetez contorul pentru urmatoarele iteratii prin sir
        } 
        iter=iter->prev;
    }
    if(sir_gasit==0){
        fprintf(OUT,"ERROR\n");
        tren->mecanic=pozitie_initiala_mecanic; //revin la pozitia initiala a mecanicului daca nu a fost gasit sirul
    }else{
        tren->mecanic=iter;
    }
}

void SEARCH_RIGHT(Tren *tren, char s[], FILE* OUT) {
    if (tren == NULL) {
        return;
    }
    Vagon *iter = tren->mecanic; 
    int lungime_sir = strlen(s);
    int sir_gasit = 0;
    Vagon *init = iter; //salvez inceputul cautarii
    Vagon *contor_iter;
    int start_pct = 0; // dacă while a trecut deja prin init
    while (iter != init || !start_pct) { 
        start_pct = 1; 
        contor_iter = iter;
        int i; //contorul pentru sirul cautat
        //are loc doar daca incepem sa gasim caracterele din sir
        for (i = 0; i < lungime_sir; i++) {
            if (contor_iter == tren->santinela) {
                    contor_iter = contor_iter->next;
            } 
            if (contor_iter->value != s[i]){
                break;
            } 
            contor_iter = contor_iter->next;
        }
        if (i == lungime_sir) { 
            sir_gasit = 1;
            tren->mecanic = contor_iter->prev; //mecanicul e pus pe ultimul caracter diin sir
            break;
        }
        // verificăm dacă am ajuns la santinela
        if (iter->next == tren->santinela) {
            iter = iter->next->next;
        } else {
            iter = iter->next;
        }
    }
    if (!sir_gasit) {
        fprintf(OUT, "ERROR\n");
    }
}

//QUERY

void SHOW_CURRENT(Tren *tren,FILE* OUT){
    if (tren == NULL) {
        return;
    }
    fprintf(OUT,"%c\n",tren->mecanic->value);
}

void SHOW(Tren *tren,FILE* OUT){
    if (tren == NULL) {
        return;
    }
    Vagon *iter=tren->santinela->next;
    while(iter!=tren->santinela){
        if(iter==tren->mecanic){
            fprintf(OUT,"|%c|",iter->value);
        }else{
            fprintf(OUT,"%c",iter->value);
        }
        iter=iter->next;
    }  
    fprintf(OUT,"\n");
}

void SWITCH(Coada *coada){
    if (coada == NULL || coada->primul == NULL) {
        return; //verific daca coada e goala sau nu exista
    }
    Comanda *comanda = coada->primul;
    Comanda *tmp; //pentru a retine adresa la urmatoarea functie
    while(comanda!=NULL){
        tmp=comanda->next;
        comanda->next=comanda->prev;
        comanda->prev=tmp;
        comanda=tmp;
    }
    //recatualizeze capul si finalul cozii
    tmp=coada->primul;
    coada->primul=coada->ultimul;
    coada->ultimul=tmp;   
}

void EXECUTE(Tren* tren, Coada* coada, FILE *OUT) {
    if (tren == NULL) {
        return;
    }
    if(coada->primul == NULL) {
        return; // verific daca coada e goala
    }
    Comanda* comandaCurenta = coada->primul;
    //actualizez capul cozii la urmatoarea functie
    coada->primul = comandaCurenta->next;
    if (coada->primul != NULL) {
        coada->primul->prev = NULL;
    } else {
        // pointez catre null daca nu mai sunt comenzi
        coada->ultimul = NULL;
    }
    // separ functia si parametrul sau de pe linie
    char comanda[100], parametru[100];
    sscanf(comandaCurenta->functie, "%s %s", comanda, parametru);
    // verific si execut comanda daca aceasta coincide cu cele pre-indicate
    if(strcmp(comanda, "MOVE_LEFT") == 0) {
        MOVE_LEFT(tren);
    } else if(strcmp(comanda, "MOVE_RIGHT") == 0) {
        MOVE_RIGHT(tren);
    } else if(strcmp(comanda, "WRITE") == 0) {
        WRITE(tren, parametru[0]);
    } else if(strcmp(comanda, "CLEAR_CELL") == 0) {
        CLEAR_CELL(tren);
    } else if(strcmp(comanda, "CLEAR_ALL") == 0) {
        CLEAR_ALL(tren);
    } else if(strcmp(comanda, "INSERT_LEFT") == 0) {
        INSERT_LEFT(tren, parametru[0], OUT);
    } else if(strcmp(comanda, "INSERT_RIGHT") == 0) {
        INSERT_RIGHT(tren, parametru[0]);
    } else if(strcmp(comanda, "SEARCH") == 0) {
        SEARCH(tren, parametru, OUT);
    } else if(strcmp(comanda, "SEARCH_LEFT") == 0) {
        SEARCH_LEFT(tren, parametru, OUT);
    } else if(strcmp(comanda, "SEARCH_RIGHT") == 0) {
        SEARCH_RIGHT(tren, parametru, OUT);
    }
    // eliberez memoria alocata
    if (comandaCurenta != NULL) {
        free(comandaCurenta);
    } 
}

//ELIBERAREA MEMORIEI
void freeMem(Tren *tren, Coada *coada) {
    //eliberez trenul
    //verific daca exista trenyl
    if (tren == NULL) { 
        return;
    }
    Vagon *vagon = tren->santinela->next;
    //parcurg tot trenul pana la santinela
    while (vagon != tren->santinela) {
        //adaug un nou nod pentru a nu-l pierde pe celalalt pe care il eliberez
        Vagon *temp = vagon;
        vagon = vagon->next;
        free(temp);
    }
    free(tren->santinela);//dau free si la coada
    free(tren);

    //eliberez coada
    //verific daca exista coada
    if (coada == NULL) {
        return;
    }
    Comanda *comanda = coada->primul;
    //parcurg toata coada pana nu pointeaza spre NULL
    while (comanda != NULL) {
        //adaug un nou nod pentru a nu pierde capul listei
        Comanda *temp = comanda;
        comanda = comanda->next;
        free(temp);
    }
    free(coada);
}

int main(void) {
    //initializez trenul si coada
    Tren * tren = initTren(); 
    if(tren==NULL){
        return 1;
    }
    Coada* coada=initCoada();
    if(coada==NULL){
        return 1;
    }
    //deschid fisierul de intrare si cel de iesire
    FILE *IN ;
    IN= fopen("tema1.in", "r");
    FILE *OUT ;
    OUT= fopen("tema1.out", "w");

    int nr_comenzi;
    char linie[MAX_LINII],functia[MAX_CARACTERE],elem[MAX_CARACTERE];
    //extrag numarul de comenzi de pe prima linie din fisier
    fscanf(IN, "%d\n",&nr_comenzi); 
    //parcurg fiecare linie din fisier
    while(fgets(linie,sizeof(linie),IN)){
        if(linie[0]=='\n'){ 
            break; //verific daca linia e goala
        }
        sscanf(linie,"%s %s",functia,elem); //extrag numele functiei si parametrul sau
        //functiile speciale indicate in cerinta ce nu sunt adaugate in coada
        if(!strcmp(functia,"SHOW")){
            SHOW(tren,OUT);
            continue;
        }
        else if(!strcmp(functia,"SHOW_CURRENT")){
            SHOW_CURRENT(tren,OUT);
            continue;
        }
        else if(!(strcmp(functia,"SWITCH"))){
            SWITCH(coada);
            continue;
        }else if(!strcmp(functia,"EXECUTE")){  
            EXECUTE(tren,coada,OUT);
            continue;
        }
        //adaug o comanda verificand daca e prima, atunci o adaug drept cap, daca nu,ca ultima
        if(coada->primul==NULL && coada->ultimul==NULL){
            coada->primul=coada->ultimul=initComanda();
            strcpy(coada->primul->functie,linie);
        }
        else{
            Comanda* new_node=initComanda();
            strcpy(new_node->functie,linie);
            Comanda* tmp=coada->ultimul;
            coada->ultimul=new_node;
            new_node->prev=tmp;
            tmp->next=new_node;
        }
    }
    fclose(IN);
    fclose(OUT);
    //eliberez memoria 
    freeMem(tren,coada);
    return 0;
}

