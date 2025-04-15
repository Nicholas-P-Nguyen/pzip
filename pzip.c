#include <pthread.h>
#include <stdio.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <unistd.h>
#include <stdlib.h>
#include "pthread.h"

#define MAX_COMPRESSED_SIZE 1000000

// any additional variables, functions, structs etc.

struct Pzip {
    pthread_t *threads;
    char *chunk;
    size_t chunk_size;
    int num_threads;
    char *file_data;
    char *output;
};

// ------------------
// A helper function that compresses an assigned workload of text;
// meant to be called once for each thread via pthread_create (see docs).
// ------------------
void* pzip(void* args) {
    // TODO: process through and compress all assigned text
    struct Pzip *zip = (struct Pzip *) args;

    for (int i = 0; i < zip->chunk_size; i++) {
        
    }

}

void init_pzip(struct Pzip *zip, size_t chunk_size, int num_threads, char *file_data) {
    zip->num_threads = num_threads;
    zip->output[num_threads];
    zip->chunk_size = chunk_size;
    zip->file_data = file_data;

}

// GLHF!
// use fwrite() to write out integer in binary format
// TODO: Handle remainder 
int main(int argc, char *argv[]) {
    // TODO: usage checking, process arguments
    int fd, num_threads;
    struct stat st;

    if (argc != 3) {
        printf("pzip: file num_threads\n");
        exit(1);
    }

    // TODO: mmap file to memory
    fd = open(argv[1], O_RDONLY);
    if (fd == -1) {
        perror("failed to open file");
        exit(1);
    }

    if (fstat(fd, &st) == -1) {
        perror("failed to get size of file");
    }

    char *file_data = mmap(NULL, st.st_size, PROT_READ, MAP_PRIVATE, fd, 0);
    if (file_data == MAP_FAILED) {
        perror("mmap failed");
    }

    num_threads = argv[2];


    // TODO: distribute workload and spawn threads
    // Dividing up contents into equal portions for each thread to work
    size_t chunk_size = st.st_size / num_threads;
    struct Pzip zip;

    init_pzip(&zip, chunk_size, num_threads, file_data);

    for (int i = 0; i < num_threads; i++) {
        char *chunk = file_data + i * chunk_size;
        zip.chunk = chunk;
        pthread_create(zip.threads[i], NULL, pzip, &zip);
    }

    for (int i = 0; i < num_threads; i++) {
        pthread_join(zip.threads[i], zip.output[i]);
    }


    // TODO: write all compressed results to stdout 

    // cleanup
}
