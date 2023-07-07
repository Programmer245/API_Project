#include <stdlib.h>
#include <string.h>
#include <stdio.h>

int main() {
    for (int i=1; i<=111; i++) {
	char command1[100]; // command to run shortest_path on input file
	char command2[100]; // command to compare outputted file with expected output file

	printf("Testing file: %d\n", i);

	sprintf(command1, "./shortest_path < archivio_test_aperti/open_%d.txt > out_text.txt", i);
	system(command1);

	sprintf(command2, "cmp archivio_test_aperti/open_%d.output.txt out_text.txt", i);
	system(command2);
    }
}
