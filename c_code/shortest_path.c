#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <limits.h>

#define HASHTAB_LEN 127 // dimensions of hash table

typedef struct Station Station; // used to store informations about stations in open hashing hash table
struct Station { // used to store info about stations in hash tables
    unsigned int dist; // stores distance of station from start of road

    unsigned int car_stack[512]; // array of all available cars at station implemented as stack
    unsigned int maximum_autonomia; // stores maximum car autonomy
    int stack_pointer; // points to first available position in stack

    struct Station *next; // used to point to next station node in hash table
};

typedef struct TreeNode TreeNode; // used in pianifica_percorso to build shortest path tree
struct TreeNode {
    unsigned int dist;
    int child_count; // number of children 

    struct TreeNode *father; // pointer to father node
    struct TreeNode **children_arr; // pointer to array of addresses of children nodes
};

typedef struct QueueNode QueueNode; // used in pianifica_percorso to keep track of next TreeNode object to process
struct QueueNode {
    TreeNode *node;

    struct QueueNode *next; // points to next QueueNode element in queue
};

void aggiungi_stazione(); // all function prototypes
void demolisci_stazione();
void aggiungi_auto();
void rottama_auto();
void pianifica_percorso();
void pianifica_forwards();
void pianifica_backwards();
int hash_func(int); 
int fill_stations_array(unsigned int *, unsigned int, unsigned int); 
int compare_ascending(const void *, const void *);
int compare_descending(const void *, const void *);
int compare_ascending_level(const void *, const void *);
void queue_add(QueueNode **, QueueNode **, TreeNode *);
TreeNode * queue_pop(QueueNode **, QueueNode **);
TreeNode * new_treenode(unsigned int);
int mod_bin_search_forwards(unsigned int *, int, unsigned int);
int mod_bin_search_backwards(unsigned int *, int, unsigned int);
void free_tree(TreeNode *);
void free_queue(QueueNode *);
void fscanf_unlocked(unsigned int *);

int station_count = 0; // keeps count of number of stations in hash_table
Station *hash_table[HASHTAB_LEN] = {NULL}; // creates the hash table initialized with null pointers; open hashing

int fscanf_ret; // holds return value for any fscanf calls

int main() {
    char command[20]; // used to store command string when parsing input

    while (fscanf(stdin, "%s", command) != EOF) { // starts reading new line 
        if (strcmp(command, "aggiungi-stazione") == 0) { 
            aggiungi_stazione(); 
        }
        else if (strcmp(command, "demolisci-stazione") == 0) {
	    demolisci_stazione();
        }
        else if (strcmp(command, "aggiungi-auto") == 0) {
	    aggiungi_auto();
        }
        else if (strcmp(command, "rottama-auto") == 0) {
	    rottama_auto();
        }
        else if (strcmp(command, "pianifica-percorso") == 0) {
	    pianifica_percorso();
	}
    }
}

void aggiungi_stazione() {
    // creates Station object with malloc, initializes its attributes and adds it to the hash table
    // this function is called after main function reads corresponding command; this function continues reading the rest of the instruction
    
    unsigned int station_dist; 
    fscanf_unlocked(&station_dist);

    Station *search; // used to check if station is already in hash table
    search = hash_table[hash_func(station_dist)]; 

    while (search != NULL) { // scans hash table chain at given position to see if station object is already in hash table
        if (search->dist == station_dist) {
	    fprintf(stdout, "non aggiunta\n");
	    return; // station object is already in hash table
	}
        else {
            search = search->next; // check next station object in hash table chain
        }
    }

    Station *new_station = (Station *)malloc(sizeof(Station)); 

    new_station->dist = station_dist;
    new_station->maximum_autonomia = 0;
    new_station->stack_pointer = 0;
    memset(new_station->car_stack, 0, sizeof(new_station->car_stack)); // fills car_stack with all 0s

    unsigned int car_autonomia;
    unsigned int car_count;
    fscanf_unlocked(&car_count);

    for (unsigned int i=0; i<car_count; i++) { // adds all the cars to the station struct object
	fscanf_unlocked(&car_autonomia); // read car autonomia and save it into variable
        if (car_autonomia > new_station->maximum_autonomia) { // if the car has the highest autonomia yet, maximum autonomia is updated
            new_station->maximum_autonomia = car_autonomia;
        } 
    
        new_station->car_stack[new_station->stack_pointer] = car_autonomia; // add the car to the station 
        new_station->stack_pointer = new_station->stack_pointer + 1;
    } 

    new_station->next = hash_table[hash_func(station_dist)]; // insert new station object into hash table
    hash_table[hash_func(station_dist)] = new_station;

    station_count++; 

    fprintf(stdout, "aggiunta\n");
}

void demolisci_stazione() {
    // checks if station is in hash map and deletes it by deallocating memory from heap

    unsigned int station_dist; 
    fscanf_unlocked(&station_dist);

    Station *search; // used to check if station is in hash table
    search = hash_table[hash_func(station_dist)]; 

    if (search == NULL) {
	fprintf(stdout, "non demolita\n");
	return; // hash table chain is empty and station object is not in hash table
    }
    else if (search->dist == station_dist) { // station object is first element of chain
	hash_table[hash_func(station_dist)] = search->next;		
	free(search);
	station_count--;
	fprintf(stdout, "demolita\n");
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
		fprintf(stdout, "demolita\n");
		return;
	    }
	    else {
		search_prev = search;
		search = search->next;
	    }
	}
	fprintf(stdout, "non demolita\n");
    }
}

void aggiungi_auto() {
    // searches hash table for station object, and if found adds car to station car stack
   
    unsigned int station_dist, car_autonomia; 
    fscanf_unlocked(&station_dist);
    fscanf_unlocked(&car_autonomia);

    Station *search;
    search = hash_table[hash_func(station_dist)]; 

    while (search != NULL) {
	if (search->dist == station_dist) {
	    if (car_autonomia > search->maximum_autonomia) {
		search->maximum_autonomia = car_autonomia;
	    }
	    search->car_stack[search->stack_pointer] = car_autonomia;
	    search->stack_pointer = search->stack_pointer + 1;

	    fprintf(stdout, "aggiunta\n");
	    return;
	}
	else {
	    search = search->next;
	}
    }

    fprintf(stdout, "non aggiunta\n");
}

void rottama_auto() {
    // searches hash table for station object, and if found removes car from station car stack

    unsigned int station_dist, car_autonomia; 
    fscanf_unlocked(&station_dist);
    fscanf_unlocked(&car_autonomia);

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

		    fprintf(stdout, "rottamata\n");
		    return;
		}
		else continue;
	    }
	    // car was not present in station
	    fprintf(stdout, "non rottamata\n");
	    return;
	}
	else {
	    search = search->next;
	}
    }

    fprintf(stdout, "non rottamata\n");
}

void pianifica_percorso() {
    unsigned int station_dist1, station_dist2; 
    fscanf_unlocked(&station_dist1);
    fscanf_unlocked(&station_dist2);

    if (station_dist1 < station_dist2) {
	pianifica_forwards(station_dist1, station_dist2);
    }
    else {
	pianifica_backwards(station_dist1, station_dist2);
    }
}

void pianifica_forwards(unsigned int station_dist1, unsigned int station_dist2) {
    // called by pianifica_percorso when starting station is closer to road start than destination station

    unsigned int *distances = (unsigned int *)malloc(sizeof(unsigned int)*station_count); // create array to hold all station distances
    int count = fill_stations_array(distances, station_dist1, station_dist2); // populate array
    qsort(distances, count, sizeof(unsigned int), compare_ascending); // sort array with quicksort in ascending order
    
    QueueNode *queue_head, *queue_tail; // queue that holds addresses of TreeNode objects to process
    queue_head = queue_tail = NULL;

    TreeNode *root = new_treenode(distances[0]); // root node of entire tree
    queue_add(&queue_head, &queue_tail, root);

    Station *search; // used to search in hash table
    TreeNode *curr; // points to TreeNode object being processed
    TreeNode *child; // used to temporariliy store address of child node
    unsigned int max_reach; // stores maximum distance reachable from station
    int furthest_idx; // stores the index of the biggest element smaller than max_reach in distances array
    int lim_idx = 0; // stores the index of the element with biggest dist parameter in tree
    while (queue_head != NULL) { // while the queue is full
	curr = queue_pop(&queue_head, &queue_tail);

	search = hash_table[hash_func(curr->dist)]; 
	while (search->dist != curr->dist) { // find station object in hash table
	    search = search->next;
	}

	max_reach = curr->dist + search->maximum_autonomia;
	furthest_idx = mod_bin_search_forwards(distances, count, max_reach);	

	if (furthest_idx == count-1) { // found shortest path to destination station
	    // calculate optimal path
	    unsigned int *stack = malloc(sizeof(unsigned int)*count); // optimal path stack
	    int sp = 0;

	    stack[sp] = station_dist2;
	    sp++;

	    TreeNode *scanner = curr; // used to scan tree structure for best path
	    while (scanner != NULL) {
		stack[sp] = scanner->dist;
		sp++;
		scanner = scanner->father;
	    }

	    for (int j=sp-1; j>=0; j--) {
		if (j == 0) { // last element
		    fprintf(stdout, "%d\n", stack[j]);
		}
		else { // not last element 
		    fprintf(stdout, "%d ", stack[j]);
		}
	    }

	    free(distances);
	    free(stack);
	    free_tree(root);
	    free_queue(queue_head);

	    return; // exits function
	}
	else if (furthest_idx <= lim_idx) { // current node will not have children
	    continue;
	}
	else { // node will have children 
	    curr->child_count = furthest_idx - lim_idx;
	    curr->children_arr = (TreeNode **)malloc(sizeof(TreeNode *)*curr->child_count); // allocate array of pointers to children nodes

	    for (int i=lim_idx+1; i<=furthest_idx; i++) { // add children nodes
		child = new_treenode(distances[i]);
		child->father = curr;

		curr->children_arr[i-lim_idx-1] = child; // add child node to father node array
		queue_add(&queue_head, &queue_tail, child); // add child node to queue
	    }

	    lim_idx = furthest_idx;
	}
    }
    // there is no path between the 2 selected stations

    free(distances);
    free_tree(root);

    fprintf(stdout, "nessun percorso\n");
}

void pianifica_backwards(unsigned int station_dist1, unsigned int station_dist2) {
    // called by pianifica_percorso when starting station is further away from road start than destination station
    // execution cost slightly more expensive than pianifica_forwards
    
    unsigned int *distances = (unsigned int *)malloc(sizeof(unsigned int)*station_count); // create array to hold all station distances
    int count = fill_stations_array(distances, station_dist2, station_dist1); // populate array
    qsort(distances, count, sizeof(unsigned int), compare_descending); // sort array with quicksort in descending order
    
    QueueNode *queue_head, *queue_tail; // queue that holds addresses of TreeNode objects to process
    queue_head = queue_tail = NULL;

    TreeNode *root = new_treenode(distances[0]); // root node of entire tree
    queue_add(&queue_head, &queue_tail, root);

    Station *search; // used to search in hash table
    TreeNode *curr; // points to TreeNode object being processed
    TreeNode *child; // used to temporariliy store address of child node
    unsigned int max_reach; // stores maximum distance reachable from station
    int furthest_idx; // stores the index of the biggest element smaller than max_reach in distances array
    int lim_idx = 0; // stores the index of the element with biggest dist parameter in tree

    TreeNode **level_buffer = (TreeNode **)malloc(sizeof(TreeNode *)*count); // holds addresses of all nodes on the same level being analyzed 
    int buffer_sp = 0; // stack pointer of level_buffer array 
    while (1) { // INFINITE LOOP; MAY CAUSE FREEZING IF HANDLED INCORRECTLY
	curr = queue_pop(&queue_head, &queue_tail);

	if (curr == NULL) { // queue was empty; either we have finished analyzing a level or we have finished analyzing entire path
	    if (buffer_sp == 0) break; // we have analyzed all nodes
	    else { // we must analyze next level
		qsort(level_buffer, buffer_sp, sizeof(TreeNode *), compare_ascending_level); // orders array of node addresses in descending order such that when queued, they will be in ascending order
		for (int i=0; i<buffer_sp; i++) { // enqueue nodes of next level in ascending order
		    queue_add(&queue_head, &queue_tail, level_buffer[i]);
		}
		buffer_sp = 0; // empty level_buffer
		continue;
	    }
	}

	search = hash_table[hash_func(curr->dist)]; 
	while (search->dist != curr->dist) { // find station object in hash table
	    search = search->next;
	}

	max_reach = curr->dist > search->maximum_autonomia ? curr->dist - search->maximum_autonomia : 0;
	furthest_idx = mod_bin_search_backwards(distances, count, max_reach);	

	if (furthest_idx == count-1) { // found shortest path to destination station
	    // calculate optimal path
	    unsigned int *stack = malloc(sizeof(unsigned int)*count); // optimal path stack
	    int sp = 0;

	    stack[sp] = station_dist2;
	    sp++;

	    TreeNode *scanner = curr; // used to scan tree structure for best path
	    while (scanner != NULL) {
		stack[sp] = scanner->dist;
		sp++;
		scanner = scanner->father;
	    }

	    for (int j=sp-1; j>=0; j--) {
		if (j == 0) { // last element
		    fprintf(stdout, "%d\n", stack[j]);
		}
		else { // not last element 
		    fprintf(stdout, "%d ", stack[j]);
		}
	    }

	    free(distances);
	    free(level_buffer);
	    free(stack);
	    free_tree(root);
	    free_queue(queue_head);

	    return; // exits function
	}
	else if (furthest_idx <= lim_idx) { // current node will not have children
	    continue;
	}
	else { // node will have children 
	    curr->child_count = furthest_idx - lim_idx;
	    curr->children_arr = (TreeNode **)malloc(sizeof(TreeNode *)*curr->child_count); // allocate array of pointers to children nodes

	    for (int i=furthest_idx; i>lim_idx; i--) { // add children nodes
		child = new_treenode(distances[i]);
		child->father = curr;

		curr->children_arr[i-lim_idx-1] = child; // add child node to father node array

		level_buffer[buffer_sp] = child; // add child address to level_buffer
		buffer_sp++;
	    }

	    lim_idx = furthest_idx;
	}
    }
    // there is no path between the 2 selected stations

    free(distances);
    free(level_buffer);
    free_tree(root);

    fprintf(stdout, "nessun percorso\n");
}

////////////////////////////////////////////////////////////////////////// SUPPORT FUNCTIONS //////////////////////////////////////////////////////////////////////////

int hash_func(int val) {
    // hash table hash function; takes distance of station as argument and returns position of station object inside hash table

    return val % HASHTAB_LEN;
}

int fill_stations_array(unsigned int *arr, unsigned int station_dist1, unsigned int station_dist2) {
    // fills array with all station distances between 2 given station distances

    int count = 0; // counter containing total number of stations added in array
    int idx = 0; // index of first available slot in array
    Station *search; // used to search through hash table

    for (int i=0; i<HASHTAB_LEN; i++) { // for each row in hash table 
	search = hash_table[i];
	while (search != NULL) {
	    if (search->dist >= station_dist1 && search->dist <= station_dist2) { // station is within 2 stations
		arr[idx] = search->dist; // add element to array
		count++; 
		idx++;
	    }
	    search = search->next; // go to next station object in chain
	}
    }

    return count;
}

int compare_ascending(const void *x, const void *y) {
    // used by qsort to order in ascending order
    return *(int *)x - *(int *)y;
}

int compare_descending(const void *x, const void *y) {
    // used by qsort to order in descending order
    return *(int *)y - *(int *)x;
}

int compare_ascending_level(const void *x, const void *y) {
    // used by qsoft to order in ascending order nodes in pianifica_backwards function
    return (*(TreeNode **)x)->dist - (*(TreeNode **)y)->dist;
}

void queue_add(QueueNode **head, QueueNode **tail, TreeNode *node) {
    // used in pianifica_forwards and pianifica_backwards functions
    // adds node to back of queue
    
    QueueNode *new = (QueueNode *)malloc(sizeof(QueueNode));
    new->next = NULL;
    new->node = node;
    
    if (*head == NULL) { // queue is empty
	*head = new;
	*tail = new;
    }
    else { // queue is not empty
	(*tail)->next = new;
	*tail = new;
    }
}

TreeNode * queue_pop(QueueNode **head, QueueNode **tail) {
    // used in pianifica_forwards and pianifica_backwards functions
    // removes first element from queue and returns its address
    
    TreeNode *popped;
    QueueNode *temp;

    if (*head == NULL) return NULL; // queue is empty
    else if (*head == *tail) { // queue contains one element 
	popped = (*head)->node; // save address of TreeNode object to return
	free(*head);
	*head = *tail = NULL;
	return popped;
    }
    else { // queue contains at least 2 elements
	popped = (*head)->node;
	temp = *head;
	*head = (*head)->next;
	free(temp);
	return popped;
    }
}

TreeNode * new_treenode(unsigned int dist) {
    // creates new TreeNode object and returns its address

    TreeNode *new = (TreeNode *)malloc(sizeof(TreeNode));

    new->dist = dist;
    new->child_count= 0;
    new->father = NULL;
    new->children_arr = NULL;

    return new;
}

int mod_bin_search_forwards(unsigned int *arr, int count, unsigned int num) {
    // modified binary search function called by pianifica_percorso_forwards
    // returns index of array with biggest number smaller than num
    // array will have at least 2 elements
    
    int start = 0;
    int end = count-1;
    int mid;

    if (num >= arr[end]) return end;
    while (end - start != 1) { // while we have not narrowed the search down to 2 elements
	mid = (start+end)/2; // finds midpoint
	if (num == arr[mid]) return mid;
	else if (num > arr[mid]) start = mid;
	else end = mid;
    }

    if (num == arr[start]) return start;
    else if (num == arr[end]) return end;
    else return start;
}

int mod_bin_search_backwards(unsigned int *arr, int count, unsigned int num) {
    // modified binary search function called by pianifica_percorso_backwards
    // returns index of array with smallest number bigger than num
    // array will have at least 2 elements
    
    int start = 0;
    int end = count-1;
    int mid;

    if (num <= arr[end]) return end;
    while (end - start != 1) { // while we have not narrowed the search down to 2 elements
	mid = (start+end)/2; // finds midpoint
	if (num == arr[mid]) return mid;
	else if (num < arr[mid]) start = mid;
	else end = mid;
    }

    if (num == arr[start]) return start;
    else if (num == arr[end]) return end;
    else return start;

}

void free_tree(TreeNode *root) {
    // takes root as parameter and frees entire tree structure; works recursively

    if (root->child_count == 0) { // base case; we are at leaf
	free(root);
    }
    else { // we are the root of a tree
	for (int i=0; i<root->child_count; i++) { // calls itself on children
	    free_tree(root->children_arr[i]);
	}
	free(root->children_arr);
	free(root);
    }
}

void free_queue(QueueNode *head) {
    // frees entire queue built by pianifica_percorso()
    // takes queue head as parameter
    
    QueueNode *temp;
    
    while (head != NULL) {
	temp = head;
	head = head->next;
	free(temp);
    }
}

void fscanf_unlocked(unsigned int *num) {
    // faster fscanf using getc_unlocked for reading a number from stdin
    
    *num = 0; // initializes number to 0
   
    char c;
    
    c = getchar_unlocked();
    while (c < '0' || c > '9') {
	c = getchar_unlocked();
    }
    while (c >= '0' && c <= '9') {
	*num = ((*num)<<3) + ((*num)<<1) + c - '0';
	c = getchar_unlocked();
    }
}
