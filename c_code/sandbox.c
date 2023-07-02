#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define HASHTAB_LEN 127 // dimensions of hash table

void aggiungi_stazione(FILE *); // all function prototypes
void demolisci_stazione();
void aggiungi_auto();
void rottama_auto();
void pianifica_percorso();
int hash_func(int);

typedef struct Station Station;
struct Station {
    unsigned int dist; // stores distance of station from start of road

    unsigned int car_stack[512]; // array of all available cars at station implemented as stack
    unsigned int maximum_autonomia; // stores maximum car autonomy
    int stack_pointer; // points to first available position in stack

    struct Station *next; // used to point to next station node in hash table
};

int station_count = 0; // keeps count of number of stations in hash_table
Station *hash_table[HASHTAB_LEN] = {NULL}; // creates the hash table initialized with null pointers; open hashing

int main() {
    char command[20]; // used to store command string when parsing input
    unsigned int buf1; // various buffers for reading input
    unsigned int buf2;

    FILE *f;
    f = fopen("archivio_test_aperti\\open_1.txt", "r"); // opens file for reading

    if (f == NULL) { // error occured while opening file
        printf("Error opening file");
        exit(0);
    }

   while (fscanf(f, "%s", command) != EOF) { // starts reading new line 
        if (strcmp(command, "aggiungi-stazione") == 0) { 
            printf("i have read aggiungi-stazione\n");

            aggiungi_stazione(f); 
        }
        else if (strcmp(command, "demolisci-stazione") == 0) {
            printf("i have read demolisci-stazione\n");
        }
        else if (strcmp(command, "aggiungi-auto") == 0) {
            printf("i have read aggiungi auto\n");
        }
        else if (strcmp(command, "rottama-auto") == 0) {
            printf("i have read rottama-auto\n");
        }
        else if (strcmp(command, "pianifica-percorso") == 0) {
            printf("i have read pianifica percorso\n");
        }
        else {
            printf("unknown command read\n");
            // return 0;
        }
   }

    fclose(f); 
}

void aggiungi_stazione(FILE *file) {
    // creates Station object with malloc, initializes its attributes and adds it to the hash table
    // this function is called after main function reads corresponding command; this function continues reading the rest of the instruction
    
    // CHECK IF STATION OJECT IS ALREADY IN HASH TABLE
    
    unsigned int station_dist; 
    fscanf(file, "%d", &station_dist);

    Station *search; // used to check if station is already in hash table
    search = hash_table[hash_func(station_dist)]; 

    printf("We are asked to add station at distance <%d>, with hash value <%d>\n", station_dist, hash_func(station_dist));

    while (search != NULL) { // scans hash table chain at given position to see if station object is already in hash table
        if (search->dist == station_dist) return; // station object is already in hash table
        else {
            search = search->next; // check next station object in hash table chain
        }
    }

    // STATION OBJECT IS NOT IN HASH TABLE; NEEDS TO BE CREATED AND INSERTED

    printf("station object needs to be added\n");

    Station *new_station = (Station *)malloc(sizeof(Station)); 

    new_station->dist = station_dist;
    new_station->maximum_autonomia = -1;
    new_station->stack_pointer = 0;

    unsigned int car_autonomia;
    unsigned int car_count;
    fscanf(file, "%d", &car_count);

    printf("num cars: <%d>\n", car_count);

    // ADD ALL THE CARS TO THE STATION

    for (unsigned int i=0; i<car_count; i++) { // adds all the cars to the station struct object
        fscanf(file, "%d", &car_autonomia); // read car autonomia and save it into variable
        printf("read car <%d>\n", car_autonomia);
        if (car_autonomia > new_station->maximum_autonomia) { // if the car has the highest autonomia yet, maximum autonomia is updated
            new_station->maximum_autonomia = car_autonomia;
        } 
    
        new_station->car_stack[new_station->stack_pointer] = car_autonomia; // add the car to the station 
        new_station->stack_pointer = new_station->stack_pointer + 1;
    } 

    // ADD STATION TO THE HASH TABLE

    new_station->next = hash_table[hash_func(station_dist)]; // insert new station object into hash table
    hash_table[hash_func(station_dist)] = new_station;

    station_count++; 
}

void demolisci_stazione() {
}

void aggiungi_auto() {
}

void rottama_auto() {
}

void pianifica_percorso() {
}

int hash_func(int val) {
    // takes station distance as input and returns its position in hash table

    return val % HASHTAB_LEN;
}