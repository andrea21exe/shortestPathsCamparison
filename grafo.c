#include <stdio.h>
#include <time.h>
#include <stdlib.h>

#define N 12
#define ATTEMPTS_X_GRAPH 3

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

// Crea un nuovo nodo della lista di adiacenza
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

void deallocaGrafo(struct Grafo* grafo) {
    if (grafo == NULL) {
        return;
    }

    // Dealloca ogni lista di adiacenza
    for (int i = 0; i < grafo->numVertici; i++) {
        struct Nodo* corrente = grafo->arrayListe[i].testa;
        while (corrente != NULL) {
            struct Nodo* temp = corrente;
            corrente = corrente->prossimo;
            free(temp); 
        }
    }

    // Dealloca l'array di liste di adiacenza
    free(grafo->arrayListe);

    // Dealloca la struttura del grafo
    free(grafo);
}

// Funzione per aggiungere un arco in un grafo non orientato
void aggiungiArco(struct Grafo* grafo, int sorgente, int destinazione, int peso) {
    struct Nodo* nuovoNodo = creaNodo(destinazione, peso);
    nuovoNodo->prossimo = grafo->arrayListe[sorgente].testa;
    grafo->arrayListe[sorgente].testa = nuovoNodo;

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


// Funzione per verificare se un arco esiste
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
void aggiungiArchi(struct Grafo* grafo, int numArchi, int seed) {
    srand(seed);  
    connettiFortemente(grafo);

    // si aggiungono archi casuali per il resto del grafo
    for (int i = 0; i < numArchi - grafo->numVertici; i++) {
        int sorgente = rand() % grafo->numVertici;
        int destinazione = rand() % grafo->numVertici;
        int peso = rand() % 100 + 1;  // Pesi tra 1 e 100

        // ci si assicura non ci siano archi che puntano allo stesso nodo o che esistano già
        if (sorgente != destinazione && esisteArco(grafo, sorgente, destinazione) == 0) {
            aggiungiArco(grafo, sorgente, destinazione, peso);
        } else {
            i--;  // si ripete il ciclo se sorgente e destinazione sono uguali o l'arco esiste già
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

void deallocaGrafoMatrice(struct GrafoMatrice* grafo) {
    if (grafo == NULL) {
        return;
    }

    // Dealloca ogni riga della matrice di adiacenza
    for (int i = 0; i < grafo->numVertici; i++) {
        free(grafo->matrice[i]);  // Libera ogni riga
    }

    // Dealloca l'array di puntatori (la matrice)
    free(grafo->matrice);

    // Dealloca la struttura del grafo
    free(grafo);
}

int** floydWarshall(struct GrafoMatrice *grafo){
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
                if(listaMatrici[index ^ 1][j][i] != INT_MAX &&
                   listaMatrici[index ^ 1][i][k] != INT_MAX &&
                   listaMatrici[index ^ 1][j][k] > listaMatrici[index ^ 1][j][i] + listaMatrici[index ^ 1][i][k]){

                    listaMatrici[index][j][k] = listaMatrici[index ^ 1][j][i] + listaMatrici[index ^ 1][i][k];
                    
                } else {
                    listaMatrici[index][j][k] = listaMatrici[index ^ 1][j][k];
                }
            }
        }
    }

    return listaMatrici[index];

    /*
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

    for(int i = 0; i < 2; i++){
        for(int j = 0; j < grafo->numVertici; j++){
            free(listaMatrici[i][j]);
        }
    }
    free(listaMatrici);

    return v;

    */
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
    double bellmanFordTimes[N];
    double dijkstraTimes[N];
    double floydWarshallTimes[N];
    FILE *file = fopen("risultati_tempi.txt", "w");


    int numVertici = 100;
    // Aumenta il numero di nodi
    for(int round = 1; round <= N/2; round++){
        // Grafo con numero di archi circa uguale al numero di nodi
        // Oppure numero di archi molto maggiore al numero di nodi
        
        for(int round2 = 0; round2 < 2; round2++){
            // 3 prove uguali per ogni grafo ---> si fa la media
            int numArchi;
            if(round2 == 0){
                numArchi = numVertici * 2;
            } else {
                numArchi = (numVertici * numVertici / 8);
            }

            double bellmanFordAttempts[ATTEMPTS_X_GRAPH];
            double dijkstraAttempts[ATTEMPTS_X_GRAPH];
            double floydWarshallAttempts[ATTEMPTS_X_GRAPH];
            int size1, size2, size3;

            for(int attempt = 0; attempt < ATTEMPTS_X_GRAPH; attempt++){

                // Inizio esperimento
                struct Grafo* grafo = creaGrafo(numVertici);
                aggiungiArchi(grafo, numArchi, attempt);

                /*
                int** bfVector = (int**)malloc(sizeof(int*)*grafo->numVertici);
                int** dijkstraVector = (int**)malloc(sizeof(int*)*grafo->numVertici);
                for(int i = 0; i < grafo->numVertici; i++){
                    bfVector[i] = (int*)malloc(sizeof(int)*grafo->numVertici);
                    dijkstraVector[i] = (int*)malloc(sizeof(int)*grafo->numVertici);
                }
                */

                struct GrafoMatrice* grafoMatrice = creaGrafoMatrice(numVertici);;
                copiaGrafo(grafoMatrice, grafo);

                size1 = sizeof(struct Grafo) + sizeof(struct Lista) * grafo->numVertici + sizeof(struct Nodo) * numArchi;
                size3 = size1 + 2 * sizeof(int) * grafo->numVertici + 2 * sizeof(int*);
                size2 = sizeof(struct GrafoMatrice) + sizeof(int*) * 3 * grafoMatrice->numVertici + sizeof(int) * 3 * grafoMatrice->numVertici * grafoMatrice->numVertici + sizeof(int**) * 2;

                // ALGORITMI
                // Floyd Warshall
                printf("Floyd Warshall...\n");
                double fwTime;
                time_t fine; time_t inizio = clock();
                floydWarshall(grafoMatrice);
                fine = clock();
                fwTime = (double)(fine - inizio)/CLOCKS_PER_SEC;
                floydWarshallAttempts[attempt] = fwTime;

                // Dijkstra
                printf("Dijkstra...\n");
                double dTime; int source;
                inizio = clock();
                for(source = 0; source < grafo->numVertici; source++){
                    dijkstra(grafo, source);
                    /*
                    for(int i = 0; i < grafo->numVertici; i++){
                        dijkstraVector[source][i] = grafo->arrayListe[i].distanza;
                    }
                    */
                }    
                fine = clock();
                dTime =  (double)(fine - inizio)/CLOCKS_PER_SEC;  
                dijkstraAttempts[attempt] = dTime;

                // Bellman-Ford
                if(numArchi < 100000){
                    double bfTime;
                    printf("Bellman Ford...\n");
                    inizio = clock();
                    for(source = 0; source < grafo->numVertici; source++){
                        bellmanFord(grafo, source);
                    /*
                    for(int i = 0; i < grafo->numVertici; i++){
                        bfVector[source][i] = grafo->arrayListe[i].distanza;
                    }
                    */
                    }
                    fine = clock();
                    bfTime = (double)(fine - inizio)/CLOCKS_PER_SEC;
                    bellmanFordAttempts[attempt] = bfTime;

                }

                // Deallocazione risorse di memoria
                deallocaGrafo(grafo);
                deallocaGrafoMatrice(grafoMatrice);
                /*
                for(int i = 0; i < grafo->numVertici; i++){
                    free(bfVector[i]);
                    free(dijkstraVector[i]);
                }
                free(bfVector); free(dijkstraVector);
                */
            }

            double sumDijkstra = 0;
            double sumFloydWarshall = 0;
            double sumBellmanFord = 0;
            for(int i = 0; i < ATTEMPTS_X_GRAPH; i++){
                sumDijkstra += dijkstraAttempts[i];
                sumBellmanFord += bellmanFordAttempts[i];
                sumFloydWarshall += floydWarshallAttempts[i];
            }
            floydWarshallTimes[round + round2] = sumFloydWarshall / ATTEMPTS_X_GRAPH;
            bellmanFordTimes[round + round2] = sumBellmanFord / ATTEMPTS_X_GRAPH;
            dijkstraTimes[round + round2] = sumDijkstra / ATTEMPTS_X_GRAPH;

            printf("%d nodi e %d archi: Dimensione dijkstra:  %d B --- Dimensione Bellman-Ford: %d B --- Dimensione Floyd-Warshall: %d B\n", numVertici, numArchi, size3, size1, size2);
            printf("\nMEDIA %d nodi, %d archi: Floyd Warshall: %fs --- Bellman Ford: %fs --- Dijkstra: %fs\n\n", numVertici, numArchi, floydWarshallTimes[round + round2], bellmanFordTimes[round + round2],  dijkstraTimes[round + round2]);


            fprintf(file, "Grafo con %d vertici e %d archi\n", numVertici, numArchi);
            fprintf(file, "Bellman-Ford: %f sec\n", bellmanFordTimes[round + round2]);
            fprintf(file, "Dijkstra: %f sec\n", dijkstraTimes[round + round2]);
            fprintf(file, "Floyd-Warshall: %f sec\n\n", floydWarshallTimes[round + round2]);
        }

        numVertici = numVertici * 2;
    }
    
    /*
    int count = 0;
    for(int i = 0; i < grafo->numVertici; i++){
        struct Nodo*arco = grafo->arrayListe[i].testa;
        while(arco){
            ++count;
            arco = arco->prossimo;
        }
    }
    printf("%d", count);
    return 0;
    */

    // Per debug, voglio vedere se ottengo gli stessi risultati    
    
    
    //stampaGrafo(grafo);
    //stampaGrafoMatrice(grafoMatrice);
    
    
    /*
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
    */

    fclose(file);
    return 0;
}

