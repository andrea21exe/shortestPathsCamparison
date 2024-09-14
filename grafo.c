#include <stdio.h>
#include <time.h>
#include <stdlib.h>

// Struttura per rappresentare un nodo della lista di adiacenza
struct Nodo {
    int vertice;
    int peso;  // Aggiungi il peso dell'arco
    struct Nodo* prossimo;
};

// Struttura per rappresentare la lista di adiacenza di un vertice
struct Lista {
    struct Nodo* testa;
    int distanza;
    int parent;
};

// Struttura per rappresentare un grafo
struct Grafo {
    int numVertici;
    struct Lista* arrayListe;
};

// Funzione per creare un nuovo nodo della lista di adiacenza
struct Nodo* creaNodo(int vertice, int peso) {
    struct Nodo* nuovoNodo = (struct Nodo*)malloc(sizeof(struct Nodo));
    nuovoNodo->vertice = vertice;
    nuovoNodo->peso = peso;
    nuovoNodo->prossimo = NULL;
    return nuovoNodo;
}

//Struttura per rappresentare grafo tramite matrice di adiacenza
struct GrafoMatrice {
    int numVertici;
    int** matrice;
};



// Funzione per creare un grafo con un determinato numero di vertici
struct Grafo* creaGrafo(int numVertici) {
    struct Grafo* grafo = (struct Grafo*)malloc(sizeof(struct Grafo));
    grafo->numVertici = numVertici;

    // Crea un array di liste di adiacenza
    grafo->arrayListe = (struct Lista*)malloc(numVertici * sizeof(struct Lista));

    // Inizializza ogni lista di adiacenza come vuota
    for (int i = 0; i < numVertici; i++) {
        grafo->arrayListe[i].testa = NULL;
    }

    return grafo;
}

// Funzione per aggiungere un arco in un grafo con peso
void aggiungiArco(struct Grafo* grafo, int sorgente, int destinazione, int peso) {
    // Aggiungi un arco dalla sorgente alla destinazione con un certo peso
    struct Nodo* nuovoNodo = creaNodo(destinazione, peso);
    nuovoNodo->prossimo = grafo->arrayListe[sorgente].testa;
    grafo->arrayListe[sorgente].testa = nuovoNodo;

    // Aggiungi un arco dalla destinazione alla sorgente (per un grafo non orientato)
    nuovoNodo = creaNodo(sorgente, peso);
    nuovoNodo->prossimo = grafo->arrayListe[destinazione].testa;
    grafo->arrayListe[destinazione].testa = nuovoNodo;
}

// Funzione per stampare il grafo con i pesi degli archi
void stampaGrafo(struct Grafo* grafo) {
    for (int i = 0; i < grafo->numVertici; i++) {
        struct Nodo* temp = grafo->arrayListe[i].testa;
        printf("\n Lista di adiacenza del vertice %d\n", i);
        while (temp) {
            printf(" -> %d (peso: %d)", temp->vertice, temp->peso);
            temp = temp->prossimo;
        }
        printf("\n");
    }
}

int bellmanFord(struct Grafo* grafo, int sorgente){

    //Inizializzazione
    for(int i = 0; i < grafo->numVertici; i++){
        grafo->arrayListe[i].distanza = INT_MAX;
    }
    grafo->arrayListe[sorgente].distanza = 0;
    grafo->arrayListe[sorgente].parent = sorgente;

    //Algoritmo
    for(int j = 0; j < grafo->numVertici; j++){
        for(int i = 0; i < grafo->numVertici; i++){
            struct Nodo* arco = grafo->arrayListe[i].testa;
            while(arco){
                if(grafo->arrayListe[i].distanza != INT_MAX && grafo->arrayListe[i].distanza + arco->peso < grafo->arrayListe[arco->vertice].distanza){
                    grafo->arrayListe[arco->vertice].distanza = grafo->arrayListe[i].distanza + arco->peso;
                    grafo->arrayListe[arco->vertice].parent = i;
                }
                arco = arco->prossimo;
            }

        }
    }

    //Controllo cicli negativi
    for(int i = 0; i < grafo->numVertici; i++){
        struct Nodo* arco = grafo->arrayListe[i].testa;
        while(arco){
            if(grafo->arrayListe[i].distanza != INT_MAX && grafo->arrayListe[i].distanza + arco->peso < grafo->arrayListe[arco->vertice].distanza){
                return -1;
            }
            arco = arco->prossimo;
        }

    }

    return 0;
}

void exchange(int*v, int *pos, int a, int b){

    if (a < 0 || b < 0) {
        printf("Errore: Indice negativo. a = %d, b = %d\n", a, b);
        exit(1);
    }
    int temp = v[a];
    v[a] = v[b];
    v[b] = temp;

    pos[v[a]] = a;
    pos[v[b]] = b;
    
}

void minHeapify(struct Grafo* grafo, int* heap, int* pos, int i, int dim) {
    int left = 2 * i + 1;
    int right = 2 * i + 2;
    int smallest = i;

    if (left < dim && grafo->arrayListe[heap[left]].distanza < grafo->arrayListe[heap[smallest]].distanza) {
        smallest = left;
    }
    if (right < dim && grafo->arrayListe[heap[right]].distanza < grafo->arrayListe[heap[smallest]].distanza) {
        smallest = right;
    }
    if (smallest != i) {
        exchange(heap, pos, i, smallest);
        minHeapify(grafo, heap, pos, smallest, dim);
    }
}

void makeMinHeap(struct Grafo* grafo, int* heap, int *pos, int dim){
    
    for(int i = dim/2; i >= 0; i--){
        minHeapify(grafo, heap, pos, i, dim);
    }
}

void decreaseKey(struct Grafo* grafo, int* heap, int* pos, int v, int key){

    grafo->arrayListe[v].distanza = key;
    int i = pos[v];

    // Risali l'heap finché la proprietà del min-heap è soddisfatta
    int parent = (i - 1) / 2;
    while (i > 0 && grafo->arrayListe[heap[parent]].distanza > grafo->arrayListe[v].distanza) {
        //printf("\nparent: %d, i: %d\n", parent, i );
        exchange(heap, pos, parent, i);
        i = parent;
        parent = (i - 1) / 2;
    }
}

int extractMin(struct Grafo* grafo, int* heap, int* pos, int dim) {
    int min = heap[0];
    //printf("\ni1: %d, i2: %d\n", 0, dim-1 );
    exchange(heap, pos, 0, dim - 1);
    minHeapify(grafo, heap, pos, 0, dim - 1); 
    return min;
}




void dijkstra(struct Grafo* grafo, int sorgente){
    //Creo il vettore per l'heap +  vettore pos in cui inserisco l'indice in heap di ogni vertice
    int *heap = (int*)malloc(sizeof(int)*(grafo->numVertici));
    int *pos = (int*)malloc(sizeof(int)*(grafo->numVertici));

    for(int i = 0; i < grafo->numVertici; i++){
        heap[i] = i;
        pos[i] = i;
        grafo->arrayListe[i].distanza = INT_MAX;
        grafo->arrayListe[i].parent = -1;
    }

    grafo->arrayListe[sorgente].distanza = 0;
    grafo->arrayListe[sorgente].parent = sorgente;   

    makeMinHeap(grafo, heap, pos, grafo->numVertici);
    int dim = grafo->numVertici - 1;
    

    while(dim > 0){
        int vertice = extractMin(grafo, heap, pos, dim--);
        struct Nodo* arco = grafo->arrayListe[vertice].testa;
        while(arco){
        
            if(grafo->arrayListe[vertice].distanza != INT_MAX && grafo->arrayListe[vertice].distanza + arco->peso < grafo->arrayListe[arco->vertice].distanza){
                grafo->arrayListe[arco->vertice].parent = vertice;
                decreaseKey(grafo, heap, pos, arco->vertice, grafo->arrayListe[vertice].distanza + arco->peso);
                
            }   
            
            arco = arco->prossimo;
        }
    }

}

// Funzione per verificare se un arco esiste già
int esisteArco(struct Grafo* grafo, int sorgente, int destinazione) {
    struct Nodo* arcoCorrente = grafo->arrayListe[sorgente].testa;
    while (arcoCorrente) {
        if (arcoCorrente->vertice == destinazione) {
            return 1;  // Arco già esistente
        }
        arcoCorrente = arcoCorrente->prossimo;
    }
    return 0;  // Arco non esistente
}

// Funzione per aggiungere archi casuali
void aggiungiArchi(struct Grafo* grafo, int numArchi) {
    srand(10);  // Inizializza il generatore di numeri casuali

    for (int i = 0; i < numArchi; i++) {
        int sorgente = rand() % grafo->numVertici;
        int destinazione = rand() % grafo->numVertici;
        int peso = rand() % 100 + 1;  // Pesi tra 1 e 100

        // Assicurati che non ci siano archi che puntano allo stesso nodo o che esistano già
        if (sorgente != destinazione && esisteArco(grafo, sorgente, destinazione) == 0) {
            aggiungiArco(grafo, sorgente, destinazione, peso);
        } else {
            i--;  // Ripeti il ciclo se sorgente e destinazione sono uguali o l'arco esiste già
        }
    }
}

struct GrafoMatrice* creaGrafoMatrice(int numVertici){
    struct GrafoMatrice* grafo = (struct GrafoMatrice*)malloc(sizeof(struct GrafoMatrice));
    grafo->numVertici = numVertici;
    grafo->matrice = (int**)malloc(sizeof(int*)*numVertici);
    for(int i = 0; i < numVertici; i++){
        grafo->matrice[i] = (int*)malloc(sizeof(int)*numVertici);
    }
    return grafo;
}

void copiaGrafo(struct GrafoMatrice* grafoMatrice, struct Grafo* grafo){

    for(int i = 0; i < grafoMatrice->numVertici; i++){
        for(int j = 0; j < grafoMatrice->numVertici; j++){
            if(i == j) {
                grafoMatrice->matrice[i][j] = 0;
            } else{
                grafoMatrice->matrice[i][j] = INT_MAX;
            }
        }
    }

    for(int i = 0; i < grafo->numVertici; i++){
        struct Nodo* arco = grafo->arrayListe[i].testa;
        while(arco){
            if(grafoMatrice->matrice[i][arco->vertice] > arco->peso){
                grafoMatrice->matrice[i][arco->vertice] = arco->peso;
            }
            arco = arco->prossimo;
        }
    }

}

void stampaGrafoMatrice(struct GrafoMatrice* grafo) {
    for (int i = 0; i < grafo->numVertici; i++) {
        for (int j = 0; j < grafo->numVertici; j++) {
            printf("%d\t", grafo->matrice[i][j]);
        }
        printf("\n");
    }
}

int main() {

    //Creazione grafo ----
    int numVertici = 100;  
    int numArchi = 400;    

    struct Grafo* grafo = creaGrafo(numVertici);
    aggiungiArchi(grafo, numArchi);

    //Per debug, voglio vedere se ottengo gli stessi risultati    
    int* bfVector = (int*)malloc(sizeof(int)*grafo->numVertici);
    int* dijkstraVector = (int*)malloc(sizeof(int)*grafo->numVertici);

    struct GrafoMatrice* grafoMatrice = creaGrafoMatrice(numVertici);;
    copiaGrafo(grafoMatrice, grafo);

    int size1 = sizeof(struct Grafo) + sizeof(struct Lista) * grafo->numVertici + sizeof(struct Nodo) * numArchi;
    int size2 = sizeof(int) * numVertici * numVertici;

    printf("Dimensione liste di adiacenza: %d B --- Dimensione matrice: %d B", size1, size2);

    //stampaGrafo(grafo);
    //stampaGrafoMatrice(grafoMatrice);

    /*

    //Dijkstra
    printf("\n\nDIJKSTRA...\n");
    time_t inizio2 = clock();
    dijkstra(grafo, 1);
    time_t fine2 = clock();

    //Bellman-Ford
    double bfTime;
    printf("BELLMANFORD...\n");
    time_t inizio = clock();
    int bf = bellmanFord(grafo, 1);
    time_t fine = clock();

    printf("\n BF: %fs --- Dijkstra: %fs\n", bfTime, (double)(fine2 - inizio2)/CLOCKS_PER_SEC);
    */
    
    /*
    for(int i = 0; i < grafo->numVertici; i++){
        int distanza = grafo->arrayListe[i].distanza;
        int parent = grafo->arrayListe[i].parent;
        bfVector[i] = grafo->arrayListe[i].distanza;
        printf("\nDistanza da 1 a %d: %d, Parent: %d", i, distanza, parent);
    }
      

    for(int i = 0; i < grafo->numVertici; i++){
        if(dijkstraVector[i] != bfVector[i]){
            printf("\nMale!\n");
            return -2;
        }
    }
    
    printf("\nFUNZIONA");
    */
    return 0;
}

