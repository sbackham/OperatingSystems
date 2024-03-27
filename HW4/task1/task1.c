#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>

//check for correct number of args
int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <required_address>\n", argv[0]);
        exit(-1);
    }
    
    //convert address from hex to unsigned long int
    unsigned long required_address = strtol(argv[1], NULL, 16);
    //path to maps file
    char maps_file[256];
    sprintf(maps_file, "/proc/%d/maps", getpid());
    
    //open the maps file
    FILE *file = fopen(maps_file, "r");
    if (!file) {
        perror("Failed to open maps file");
        exit(-1);
    }

    unsigned long start, end;
    char perms[5];
    int found = 0;
    
    //parse the maps file
    while (fscanf(file, "%lx-%lx %4s", &start, &end, perms) != EOF) {
    //is the required address within the current memory region
        if (required_address >= start && required_address < end) {
            if (perms[0] == 'r') {  // Check if the region is readable
                found = 1;
                break;
            }
        }
        //skip the rest of the line
        while (fgetc(file) != '\n');
    }
    fclose(file);

    if (found) {
        printf("%02x\n", *((char *)required_address));
        exit(0);
    } else {
        exit(-1);
    }
}

