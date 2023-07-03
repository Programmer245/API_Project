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

    FILE *f;
    f = fopen("archivio_test_aperti/open_1.txt", "r"); // opens file for reading

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

	    demolisci_stazione(f);
        }
        else if (strcmp(command, "aggiungi-auto") == 0) {
            printf("i have read aggiungi auto\n");

	    aggiungi_auto(f);
        }
        else if (strcmp(command, "rottama-auto") == 0) {
            printf("i have read rottama-auto\n");

	    rottama_auto(f);
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

    printf("station object needs to be added\n");

    Station *new_station = (Station *)malloc(sizeof(Station)); 

    new_station->dist = station_dist;
    new_station->maximum_autonomia = 0;
    new_station->stack_pointer = 0;
    memset(new_station->car_stack, 0, sizeof(new_station->car_stack)); // fills car_stack with all 0s

    unsigned int car_autonomia;
    unsigned int car_count;
    fscanf(file, "%d", &car_count);

    printf("num cars: <%d>\n", car_count);

    for (unsigned int i=0; i<car_count; i++) { // adds all the cars to the station struct object
        fscanf(file, "%d", &car_autonomia); // read car autonomia and save it into variable
        printf("read car <%d>\n", car_autonomia);
        if (car_autonomia > new_station->maximum_autonomia) { // if the car has the highest autonomia yet, maximum autonomia is updated
            new_station->maximum_autonomia = car_autonomia;
        } 
    
        new_station->car_stack[new_station->stack_pointer] = car_autonomia; // add the car to the station 
        new_station->stack_pointer = new_station->stack_pointer + 1;
    } 

    new_station->next = hash_table[hash_func(station_dist)]; // insert new station object into hash table
    hash_table[hash_func(station_dist)] = new_station;

    station_count++; 
}

void demolisci_stazione(FILE *file) {
    // checks if station is in hash map and deletes it by deallocating memory from heap

    unsigned int station_dist; 
    fscanf(file, "%d", &station_dist);

    Station *search; // used to check if station is in hash table
    search = hash_table[hash_func(station_dist)]; 

    if (search == NULL) return; // hash table chain is empty and station object is not in hash table
    else if (search->dist == station_dist) { // station object is first element of chain
	hash_table[hash_func(station_dist)] = search->next;		
	free(search);
	station_count--;
	return;
    }
    else { // hash table chain is not empty and station object is not first element
	Station *search_prev; // used to easily patch hash table chain

	search_prev = search;
	search = search->next;

	while (search != NULL) {
	    if (search->dist == station_dist) {
		search_prev->next = search->next;
		free(search);
		station_count--;
		return;
	    }
	    else {
		search_prev = search;
		search = search->next;
	    }
	}
    }
}

void aggiungi_auto(FILE *file) {
    // searches hash table for station object, and if found adds car to station car stack
   
    unsigned int station_dist, car_autonomia; 
    fscanf(file, "%d %d", &station_dist, &car_autonomia);

    Station *search;
    search = hash_table[hash_func(station_dist)]; 

    while (search != NULL) {
	if (search->dist == station_dist) {
	    if (car_autonomia > search->maximum_autonomia) {
		search->maximum_autonomia = car_autonomia;
	    }
	    search->car_stack[search->stack_pointer] = car_autonomia;
	    search->stack_pointer = search->stack_pointer + 1;

	    return;
	}
	else {
	    search = search->next;
	}
    }
}

void rottama_auto(FILE *file) {
    // searches hash table for station object, and if found removes car from station car stack
    
    printf("we in rottama auto");

    unsigned int station_dist, car_autonomia; 
    fscanf(file, "%d %d", &station_dist, &car_autonomia);

    Station *search;
    search = hash_table[hash_func(station_dist)]; 

    while (search != NULL) {
	if (search->dist == station_dist) {
	    for (int i=0; i<search->stack_pointer; i++) { // scan entire car stack to find car to remove
		if (search->car_stack[i] == car_autonomia) { // we have found the car to remove
		    search->stack_pointer = search->stack_pointer - 1;
		    search->car_stack[i] = search->car_stack[search->stack_pointer];
		    search->car_stack[search->stack_pointer] = 0; // writes 0 at stack pointer to keep stack zeroes from stack_pointer onwards

		    if (car_autonomia == search->maximum_autonomia) { // we are removing one of the cars with the highest autonomia; we must recalculate maximum_autonomia
			int maximum = 0; // recalculate maximum
			for (int j=0; j<search->stack_pointer; j++) {
			    if (search->car_stack[j] > maximum) { 
				maximum = search->car_stack[j];
			    }
			}
			search->maximum_autonomia = maximum; // update maximum autonomia
		    }

		    return;
		}
		else continue;
	    }
	    // car was not present in station
	    return;
	}
	else {
	    search = search->next;
	}
    }
}

void pianifica_percorso() {
}

int hash_func(int val) {
    // takes station distance as input and returns its position in hash table

    return val % HASHTAB_LEN;
}
