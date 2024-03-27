#Operating Systems CS3620
#Sirena Backham (sbackham)

#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>


void modify_file(const char* file_path, const char* hawkid) {
    // open the file with rw permissions
    int fd = open(file_path, O_RDWR);
    if (fd < 0) {
        fprintf(stderr, "Failed to open file %s\n", file_path);
        return;
    }

    char buffer[4096];
    // read up to 4096 bytes from the file into buffer
    ssize_t bytes_read = read(fd, buffer, sizeof(buffer));
    if (bytes_read < 0) {
        perror("Read error");
        close(fd);
        return;
    }

    char modified_content[4096];
    // add the hawkid to the file content
    snprintf(modified_content, sizeof(modified_content), "%s%s", hawkid, buffer);

    // move the file pointer to the beginning of the file
    lseek(fd, 0, SEEK_SET);
    // write the modified content back to the file
    write(fd, modified_content, strlen(modified_content));

    // close the file
    close(fd);
}

// function to recursively search directories and modify qualifying files
void search_directory(const char* dir_path, const char* hawkid) {
    // open the directory
    DIR* dir = opendir(dir_path);
    if (dir == NULL) {
        fprintf(stderr, "Failed to open directory %s\n", dir_path);
        return;
    }

    struct dirent *entry;
    // iterate through each entry in the directory
    while ((entry = readdir(dir)) != NULL) {
        if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0)
            continue;

        char path[1024];
        // make path for each entry
        snprintf(path, sizeof(path), "%s/%s", dir_path, entry->d_name);

        struct stat path_stat;
        stat(path, &path_stat);

        // recursively call search_directory if entry is a directory
        if (S_ISDIR(path_stat.st_mode)) {
            search_directory(path, hawkid);
        } 
        // modify file if it starts with "CS3620"
        else if (strncmp(entry->d_name, "CS3620", 6) == 0) {
            printf("Modifying file: %s\n", path);
            modify_file(path, hawkid);
        }
    }

    // close directory
    closedir(dir);
}

int main() {
    const char* directory_to_search = ".";
    const char* hawkid = "sbackham";

    search_directory(directory_to_search, hawkid);

    return 0;
}

