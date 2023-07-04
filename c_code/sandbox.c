#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <limits.h>

#define HASHTAB_LEN 127 // dimensions of hash table

void aggiungi_stazione(FILE *); // all function prototypes
void demolisci_stazione(FILE *);
void aggiungi_auto(FILE *);
void rottama_auto(FILE *);
void pianifica_percorso(FILE *);
int hash_func(int);
void best_path(unsigned int *, int);

typedef struct Station Station;
struct Station { // used to store info about stations in hash tables
    unsigned int dist; // stores distance of station from start of road

    unsigned int car_stack[512]; // array of all available cars at station implemented as stack
    unsigned int maximum_autonomia; // stores maximum car autonomy
    int stack_pointer; // points to first available position in stack

    struct Station *next; // used to point to next station node in hash table
};

typedef struct SpecialStation SpecialStation;
struct SpecialStation { // used with pianifica_percorso()
    unsigned int dist; // distance of station 
    unsigned int maximum_autonomia; // maximum_autonomia of current 
    unsigned int cost; // cost of reaching this station (dijkstra)
    unsigned int previous; // previous station (dijkstra)

    struct SpecialStation *prev;
    struct SpecialStation *next;
};

int station_count = 0; // keeps count of number of stations in hash_table
Station *hash_table[HASHTAB_LEN] = {NULL}; // creates the hash table initialized with null pointers; open hashing

int main() {
    char command[20]; // used to store command string when parsing input

    FILE *f;
    f = fopen("archivio_test_aperti/open_100.txt", "r"); // opens file for reading

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

	    pianifica_percorso(f);
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
    
    printf("we in rottama auto\n");

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

void pianifica_percorso(FILE *file) {
    // calculates and prints the shortest path between 2 given stations

    int station_count = 0;
    
    unsigned int station_dist1, station_dist2; 
    fscanf(file, "%d %d", &station_dist1, &station_dist2);

    Station *search; // used to search hash table
    SpecialStation *head = NULL; // points to double linked list that is created in order to use dijkstra's algorithm
    SpecialStation *temp, *temp2; // used to scan double linked list 

    // build double linked list
    for (int i=0; i<HASHTAB_LEN; i++) { // scan entire hash table
	search = hash_table[i]; // sets pointer to first object of hash table chain i
	while (search != NULL) { // scan until end of chain
	    if ((search->dist >= station_dist1 && search->dist <= station_dist2) || (search->dist <= station_dist1 && search->dist >= station_dist2)) { // station object is the start and end stations
		station_count++;

		SpecialStation *new_node = (SpecialStation *)malloc(sizeof(SpecialStation)); 

		new_node->dist = search->dist;
		new_node->maximum_autonomia = search->maximum_autonomia;
		new_node->cost = UINT_MAX;
		new_node->prev = NULL;
		new_node->next = NULL;

		if (head == NULL) { // double linked list is empty
		    head = new_node;
		}
		else { // double linked list is not empty
		    temp = head;
		    while ((temp->dist < new_node->dist) && (temp->next != NULL)) { // we stop when either temp points to station with bigger dist value, or next node is NULL 
			temp = temp->next;
		    }
		    if (temp->dist < new_node->dist) { // temp->next = NULL; need to insert new node to the right
			new_node->prev = temp;
			temp->next = new_node;
		    }
		    else { // temp->next != NULL; need to insert new node to the left
			if (temp->prev == NULL) { // temp points to first node of linked list
			    new_node->next = temp;
			    temp->prev = new_node;
			    head = new_node;
			}
			else { 
			    new_node->next = temp;
			    new_node->prev = temp->prev;
			    temp->prev->next = new_node;
			    temp->prev = new_node;
			}
		    }
		}
	    }
	    search = search->next;
	}
    }

    // debugging purposes
    temp = head;
    printf("NULL->");
    while (temp != NULL) {
	printf("%d->", temp->dist);
	temp = temp->next;
    }
    printf("NULL\n");

    if (station_dist1 < station_dist2) {

	// run dijkstra algorithm on graph
	head->cost = 0; // initializes first node to cost 0
	head->previous = head->dist;
	
	temp = head;
	while (temp != NULL) {
	    if (temp->cost == UINT_MAX) { // we have found an unreachable station; final station cannot be reached from starting station
		printf("unreachable station\n");
		return;
	    }
	    
	    temp2 = temp->next;
	    while (temp2 != NULL && (temp2->dist <= temp->dist + temp->maximum_autonomia)) {
		if ((temp->cost + 1 < temp2->cost) || ((temp->cost + 1 == temp2->cost) && (temp->dist < temp2->previous))) { // found shorter path 
		    temp2->cost = temp->cost + 1;
		    temp2->previous = temp->dist;
		}
		temp2 = temp2->next;
	    }

	    temp = temp->next;
	}
	// temp == NULL at end of while loop

	temp = head;
	while (temp->next != NULL) temp = temp->next; // moves temp to last node of double linked list

	// construct optimal path by moving backwards from final station to starting station
	int stack_pointer = 0;
	unsigned int target; // used for finding previous node 
	unsigned int *optimal_path = (unsigned int*)malloc(sizeof(unsigned int)*station_count); // optimal_path stack to memorize shortest path backwards

	while (temp != NULL) { 
	    optimal_path[stack_pointer] = temp->dist;
	    stack_pointer++;

	    target = temp->previous; 
	    do {
		temp = temp->prev;
	    } while (temp != NULL && temp->dist != target);
	}

	best_path(optimal_path, stack_pointer);

	// deallocate used heap space
	free(optimal_path);
	while (head != NULL) {
	    temp = head;
	    head = head->next;
	    free(temp);
	}
    }
    else { // station1 > station2

	// run dijkstra algorithm on graph
	while (head->next != NULL) head = head->next; // head is last element
	head->cost = 0; // initializes first node to cost 0
	head->previous = head->dist;
	
	temp = head;
	while (temp != NULL) {
	    if (temp->cost == UINT_MAX) { // we have found an unreachable station; final station cannot be reached from starting station
		printf("unreachable station\n");
		return;
	    }
	    
	    temp2 = temp->prev;
	    while (temp2 != NULL && (temp2->dist + temp->maximum_autonomia >= temp->dist)) {
		if ((temp->cost + 1 < temp2->cost) || ((temp->cost + 1 == temp2->cost) && (temp->dist < temp2->previous))) { // found shorter path 
		    temp2->cost = temp->cost + 1;
		    temp2->previous = temp->dist;
		}
		temp2 = temp2->prev;
	    }

	    temp = temp->prev;
	}
	// temp == NULL at end of while loop

	temp = head;
	while (temp->prev != NULL) temp = temp->prev; // moves temp to last node of double linked list
	
	// construct optimal path by moving backwards from final station to starting station
	int stack_pointer = 0;
	unsigned int target; // used for finding previous node 
	unsigned int *optimal_path = (unsigned int*)malloc(sizeof(unsigned int)*station_count); // optimal_path stack to memorize shortest path backwards

	while (temp != NULL) { 
	    optimal_path[stack_pointer] = temp->dist;
	    stack_pointer++;

	    target = temp->previous; 
	    do {
		temp = temp->next;
	    } while (temp != NULL && temp->dist != target);
	}

	best_path(optimal_path, stack_pointer);

	// deallocate used heap space
	free(optimal_path);
	while (head != NULL) {
	    temp = head;
	    head = head->prev;
	    free(temp);
	}
    }
}

// support functions

int hash_func(int val) {
    // hash table hash function; takes distance of station as argument and returns position of station object inside hash table

    return val % HASHTAB_LEN;
}

void best_path(unsigned int *stack, int sp) {
    // arguments are stack containing shortest path from end station to start station and stack pointer that points to next available slot
    // function prints best path
    
    printf("optimal path: ");
    for (int i=sp-1; i>=0; i--) {
	printf("%d->", stack[i]);
    }
    printf("end\n");
}
