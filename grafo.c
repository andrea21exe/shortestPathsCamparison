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

// Funzione per creare una componente fortemente connessa di base
void connettiFortemente(struct Grafo* grafo) {
    for (int i = 0; i < grafo->numVertici - 1; i++) {
        int peso = rand() % 100 + 1;  // Pesi tra 1 e 100
        if (esisteArco(grafo, i, i + 1) == 0) {
            aggiungiArco(grafo, i, i + 1, peso);
        }
    }
    
    // Aggiungi un arco dall'ultimo vertice al primo per chiudere il ciclo
    int peso = rand() % 100 + 1;
    if (esisteArco(grafo, grafo->numVertici - 1, 0) == 0) {
        aggiungiArco(grafo, grafo->numVertici - 1, 0, peso);
    }
}

// Funzione modificata per aggiungere archi casuali e garantire la forte connessione
void aggiungiArchi(struct Grafo* grafo, int numArchi) {
    srand(0);  // Inizializza il generatore di numeri casuali

    // Prima creiamo una componente fortemente connessa di base
    connettiFortemente(grafo);

    // Poi aggiungiamo archi casuali per il resto del grafo
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

int* floydWarshall(struct GrafoMatrice *grafo){
    // Primo indice: 1 o 2, secondo indice: sorgente, terzo indice: destinazione
    int ***listaMatrici = (int***)malloc(sizeof(int**) * 2);
    for(int i = 0; i < 2; i++){
        listaMatrici[i] = (int**)malloc(sizeof(int*) * grafo->numVertici);
        for(int j = 0; j < grafo->numVertici; j++){
            listaMatrici[i][j] = (int*)malloc(sizeof(int) * grafo->numVertici);
        }
    }

    // Copia la matrice di adiacenza nella struttura
    for(int i = 0; i < grafo->numVertici; i++){
        for(int j = 0; j < grafo->numVertici; j++){
            listaMatrici[0][i][j] = grafo->matrice[i][j];
        }
    }

    // Le matrici vengono riutilizzate
    int index;
    for(int i = 0; i < grafo->numVertici; i++){
        index = (i + 1) % 2;
        for(int j = 0; j < grafo->numVertici; j++){
            for(int k = 0; k < grafo->numVertici; k++){
                if(listaMatrici[index ^ 1][j][i] != INT_MAX && listaMatrici[index ^ 1][i][k] != INT_MAX && listaMatrici[index ^ 1][j][k] > listaMatrici[index ^ 1][j][i] + listaMatrici[index ^ 1][i][k]){
                    listaMatrici[index][j][k] = listaMatrici[index ^ 1][j][i] + listaMatrici[index ^ 1][i][k];
                } else {
                    listaMatrici[index][j][k] = listaMatrici[index ^ 1][j][k];
                }
            }
        }
    }

    int max = INT_MIN;
    int source = -1;
    int destination = -1;
    for(int i = 0; i < grafo->numVertici; i++){
        for(int j = 0; j < grafo->numVertici; j++){
            //printf("%d\t",listaMatrici[index][i][j]);
            if(max < listaMatrici[index][i][j]){
                max = listaMatrici[index][i][j];
                source = i;
                destination = j;
            }
        }
        //printf("\n");
    }

    int *v = (int*)malloc(sizeof(int) * 3);
    v[0] = max; v[1] = source; v[2] = destination;

    return v;
}

void dijkstra(struct Grafo* grafo, int sorgente) {
    int numVertici = grafo->numVertici;
    
    // Inizializza le distanze e l'heap
    int* heap = (int*)malloc(numVertici * sizeof(int));
    int* pos = (int*)malloc(numVertici * sizeof(int));
    
    for (int i = 0; i < numVertici; i++) {
        grafo->arrayListe[i].distanza = INT_MAX; 
        grafo->arrayListe[i].parent = -1; 
        heap[i] = i;
        pos[i] = i;
    }
    
    // La distanza dalla sorgente a se stessa è 0. E' l'unica = 0. Le altre sono INT_MAX
    decreaseKey(grafo, heap, pos, sorgente, 0);
    
    int dim = numVertici;
    while (dim > 0) {
        int u = extractMin(grafo, heap, pos, dim);
        dim--;
        
        struct Nodo* nodo = grafo->arrayListe[u].testa;
        while (nodo != NULL) {
            int v = nodo->vertice;
            int peso = nodo->peso;
            
            // Relaxation
            if (grafo->arrayListe[u].distanza != INT_MAX && grafo->arrayListe[v].distanza > grafo->arrayListe[u].distanza + peso) {
                decreaseKey(grafo, heap, pos, v, grafo->arrayListe[u].distanza + peso);
                grafo->arrayListe[v].parent = u;
            }
            nodo = nodo->prossimo;
        }
    }

    // Stampa dei risultati
    /*
    printf("Nodo\tDistanza dalla sorgente%d \tPercorso\n", sorgente);
    for (int i = 0; i < numVertici; i++) {
        printf("%d\t%d\t\t\t", i, grafo->arrayListe[i].distanza);
        int parent = grafo->arrayListe[i].parent;
        while (parent != -1) {
            printf("%d <- ", parent);
            parent = grafo->arrayListe[parent].parent;
        }
        printf("N/A\n");
    }
    */

    free(heap);
    free(pos);
}

int bellmanFord(struct Grafo* grafo, int sorgente) {
    int numVertici = grafo->numVertici;

    // Inizializza le distanze e i parent all'interno delle strutture del grafo
    for (int i = 0; i < numVertici; i++) {
        grafo->arrayListe[i].distanza = INT_MAX;  
        grafo->arrayListe[i].parent = -1;         
    }
    grafo->arrayListe[sorgente].distanza = 0;     

    for (int i = 0; i < numVertici - 1; i++) {
        for (int u = 0; u < numVertici; u++) {
            struct Nodo* nodo = grafo->arrayListe[u].testa;
            while (nodo != NULL) {
                int v = nodo->vertice;
                int peso = nodo->peso;

                // Relax
                if (grafo->arrayListe[u].distanza != INT_MAX && grafo->arrayListe[v].distanza > grafo->arrayListe[u].distanza + peso) {
                    grafo->arrayListe[v].distanza = grafo->arrayListe[u].distanza + peso;
                    grafo->arrayListe[v].parent = u;
                }
                nodo = nodo->prossimo;
            }
        }
    }

    // Cicli Negativi?
    for (int u = 0; u < numVertici; u++) {
        struct Nodo* nodo = grafo->arrayListe[u].testa;
        while (nodo != NULL) {
            int v = nodo->vertice;
            int peso = nodo->peso;
            if (grafo->arrayListe[u].distanza != INT_MAX && grafo->arrayListe[v].distanza > grafo->arrayListe[u].distanza + peso) {
                return -1;
            }
            nodo = nodo->prossimo;
        }
    }

    // Stampa dei risultati
    /*
    printf("Nodo\tDistanza dalla sorgente%d \tPercorso\n", sorgente);
    for (int i = 0; i < numVertici; i++) {
        printf("%d\t%d\t\t\t", i, grafo->arrayListe[i].distanza);
        int parent = grafo->arrayListe[i].parent;
        while (parent != -1) {
            printf("%d <- ", parent);
            parent = grafo->arrayListe[parent].parent;
        }
        printf("N/A\n");
    }
    */

   return 0;
}


int main() {
    
    // Creazione grafo ----
    int numVertici = 100;  
    int numArchi = 400;    

    struct Grafo* grafo = creaGrafo(numVertici);
    aggiungiArchi(grafo, numArchi);

    // Per debug, voglio vedere se ottengo gli stessi risultati    
    int** bfVector = (int**)malloc(sizeof(int*)*grafo->numVertici);
    int** dijkstraVector = (int**)malloc(sizeof(int*)*grafo->numVertici);
    for(int i = 0; i < grafo->numVertici; i++){
        bfVector[i] = (int*)malloc(sizeof(int)*grafo->numVertici);
        dijkstraVector[i] = (int*)malloc(sizeof(int)*grafo->numVertici);
    }

    struct GrafoMatrice* grafoMatrice = creaGrafoMatrice(numVertici);;
    copiaGrafo(grafoMatrice, grafo);

    int size1 = sizeof(struct Grafo) + sizeof(struct Lista) * grafo->numVertici + sizeof(struct Nodo) * numArchi;
    int size2 = sizeof(struct GrafoMatrice) + sizeof(int*) * grafoMatrice->numVertici + sizeof(int) * grafoMatrice->numVertici * grafoMatrice->numVertici;
    printf("Dimensione liste di adiacenza: %d B --- Dimensione matrice: %d B\n", size1, size2);

    // Floyd Warshall
    printf("Floyd Warshall...\n");
    double fwTime;
    time_t inizio3 = clock();
    int* v = floydWarshall(grafoMatrice);
    time_t fine3 = clock();
    fwTime = (double)(fine3 - inizio3)/CLOCKS_PER_SEC;

    //stampaGrafo(grafo);
    //stampaGrafoMatrice(grafoMatrice);

    // Dijkstra
    printf("Dijkstra...\n");
    double dTime;
    time_t inizio2 = clock();
    int source;
    for(source = 0; source < grafo->numVertici; source++){
        dijkstra(grafo, source);
        for(int i = 0; i < grafo->numVertici; i++){
            dijkstraVector[source][i] = grafo->arrayListe[i].distanza;
        }
    }
    time_t fine2 = clock();
    dTime =  (double)(fine2 - inizio2)/CLOCKS_PER_SEC;
    
    // Bellman-Ford
    double bfTime;
    printf("Bellman Ford...\n");
    time_t inizio = clock();
    for(source = 0; source < grafo->numVertici; source++){
        bellmanFord(grafo, source);
        for(int i = 0; i < grafo->numVertici; i++){
            bfVector[source][i] = grafo->arrayListe[i].distanza;
        }
    }
    time_t fine = clock();
    bfTime = (double)(fine - inizio)/CLOCKS_PER_SEC;

    int max = INT_MIN;
    for(int i = 0; i < grafo->numVertici; i++){
        for(int j = 0; j < grafo->numVertici; j++){
            if(dijkstraVector[i][j] != bfVector[i][j]){
                printf("%d %d\n", i,j);
            }
            if(max < dijkstraVector[i][j]){
                max = dijkstraVector[i][j];
            }
        }
    }
    if(max != v[0]){
        printf("\nNOOOOO");
        return -1;
    } 

    printf("Max Path: Source: %d, Destination: %d, Max Length: %d", v[1], v[2], v[0]);
    printf("\nFloyd Warshall: %fs --- Bellman Ford: %fs --- Dijkstra: %fs\n", fwTime, bfTime, dTime);
    
    return 0;
}

