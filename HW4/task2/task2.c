#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

//function to reverse contents
void reverse_content(char *content, size_t size) {
    char temp;
    for (size_t i = 0; i < size / 2; i++) {
        temp = content[i];
        content[i] = content[size - 1 - i];
        content[size - 1 - i] = temp;
    }
}

int main(int argc, char *argv[]) {
    if (argc != 2) { //check for correct number of args
        fprintf(stderr, "Usage: %s <file_path>\n", argv[0]);
        exit(-1);
    }
    //open the file (from mmap_eg4.c)
    int fd = open(argv[1], O_RDWR);
    if (fd == -1) {
        perror("Failed to open file");
        exit(-1);
    }
    
    //get file size (from mmap_eg4.c)
    struct stat sb;
    fstat(fd, &sb);
    size_t file_size = sb.st_size;
    
    //map file into memory (from mmap_eg4.c)
    char *addr = (char *)mmap(0, file_size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    //if map file fails
    if (addr == MAP_FAILED) {
        perror("Failed to map file");
        close(fd);
        exit(-1);
    }
    
    //reverse content
    reverse_content(addr, file_size);
    //sync the new changes back to file
    msync(addr, file_size, MS_SYNC);
    //unmap file from memory
    munmap(addr, file_size);
    //close file descriptor
    close(fd);

    return 0;
}

