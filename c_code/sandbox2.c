#include <stdio.h>
#include <string.h>
#include <stdlib.h>

int main() {
    char command[20]; // used to store command string when parsing input

    FILE *f;
    f = fopen("C:\\Users\\bruna\\Desktop\\API_Project\\code\\archivio_test_aperti\\open_1.txt", "r"); // opens file for reading

    if (f == NULL) { // error occured while opening file
        printf("Error opening file");
        exit(0);
    }

   while (fscanf(f, "%s", command) != EOF) { // starts reading new line 
        if (strcmp(command, "aggiungi-stazione") == 0) { 

            printf("i have read aggiungi-stazione\n");

            int val;

            if (fscanf(f, "%d", &val) != EOF) {
                printf("value: <%d>\n", val);
            }
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