#include <pthread.h>
#include <stdio.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>

#define MAX_COMPRESSED_SIZE 1000000

// any additional variables, functions, structs etc.

struct ThreadOutput {
    char *rle; // compressed run length encoding
    size_t length; // the length of the rle
};

struct ThreadArgs {
    char *chunk; // chunk of text a worker thread will compress
    size_t chunk_size; // size of chunk depends on # of threads 
};

// ------------------
// A helper function that compresses an assigned workload of text;
// meant to be called once for each thread via pthread_create (see docs).
// ------------------
void* pzip(void* args) {
    // TODO: process through and compress all assigned text
    struct ThreadArgs *thread_args = (struct ThreadArgs *) args;
    size_t rle_i = 0;
    char *rle = malloc(MAX_COMPRESSED_SIZE);
    if (rle == NULL) {
        perror("malloc failed");
        pthread_exit(NULL);
    }

    // use two pointers to get count of same characters
    size_t left = 0;
    while (left < thread_args->chunk_size) {
        int right = left + 1;
        char curr_char = thread_args->chunk[left];
        while (right < thread_args->chunk_size && curr_char == thread_args->chunk[right]) {
            right++;
        }
        int count = right - left;
        // copy the 4 byte int (the run length)
        memcpy(rle + rle_i, &count, sizeof(int));
        // copy the single character
        rle[rle_i + 4] = curr_char;
        // increment to next rle encoding
        rle_i += 5;
        
        left = right;
    }

    struct ThreadOutput *thread_output = malloc(sizeof(struct ThreadOutput));
    thread_output->rle = rle;
    thread_output->length = rle_i;
    pthread_exit(thread_output);
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
        close(fd);
        exit(1);
    }

    if (fstat(fd, &st) == -1) {
        perror("failed to get size of file");
        close(fd);
        exit(1);
    }

    char *file_data = mmap(NULL, st.st_size, PROT_READ, MAP_PRIVATE, fd, 0);
    if (file_data == MAP_FAILED) {
        perror("mmap failed");
        close(fd);
        exit(1);
    }

    num_threads = atoi(argv[2]);
    if (num_threads <= 0) {
        fprintf(stderr, "pzip: file num_threads\n");
        munmap(file_data, st.st_size);
        close(fd);
        exit(1);
    }


    // TODO: distribute workload and spawn threads
    // Dividing up contents into equal portions for each thread to work
    size_t chunk_size = st.st_size / num_threads;
    size_t remainder = st.st_size % num_threads;

    pthread_t *threads = malloc(sizeof(pthread_t) * num_threads);
    if (threads == NULL) {
        perror("malloc failed for threads");
        exit(1);
    }
    struct ThreadArgs *thread_args = malloc(sizeof(struct ThreadArgs) * num_threads);
    if (thread_args == NULL) {
        perror("malloc failed for thread args");
        exit(1);
    }
    struct ThreadOutput **thread_output = malloc(sizeof(struct ThreadOutput *) * num_threads);
    if (thread_output == NULL) {
        perror("malloc failed for thread output");
        exit(1);
    }

    for (int i = 0; i < num_threads; i++) {
        char *chunk = file_data + i * chunk_size;
        thread_args[i].chunk = chunk;
        thread_args[i].chunk_size = chunk_size;

        if (i == num_threads - 1) {
            thread_args[i].chunk_size += remainder;
        }

        pthread_create(&threads[i], NULL, pzip, &thread_args[i]);
    }

    // waiting for threads to finish and storing result in a pointer
    for (int i = 0; i < num_threads; i++) {
        void *output_p;
        pthread_join(threads[i], &output_p);
        thread_output[i] = (struct ThreadOutput *) output_p;
    }
    // post process the threads output by combining same characters run length
    for (int i = 1; i < num_threads; i++) {
        struct ThreadOutput *prev = thread_output[i - 1];
        struct ThreadOutput *curr = thread_output[i];

        // ex. prev = 10b3c4a curr = 5a6b10c -> need to combine 4a & 5a
        char prev_char = prev->rle[prev->length - 1];
        char curr_char = curr->rle[4];

        if (prev_char == curr_char) {
            int prev_count, curr_count;
            memcpy(&prev_count, prev->rle + prev->length - 5, sizeof(int));
            memcpy(&curr_count, curr->rle, sizeof(int));
            int total_count = prev_count + curr_count;

            // update count in previous thread output
            memcpy(prev->rle + prev->length - 5, &total_count, sizeof(int));

            // shift current output left to remove the first 5 bytes
            memmove(curr->rle, curr->rle + 5, curr->length - 5);
            curr->length -= 5;
        }
    }

    // TODO: write all compressed results to stdout 
    for (int i = 0; i < num_threads; i++) {
        fwrite(thread_output[i]->rle, 1, thread_output[i]->length, stdout);
        free(thread_output[i]->rle);
        free(thread_output[i]);
    }
    // cleanup
    munmap(file_data, st.st_size);
    close(fd);
    free(threads);
    free(thread_args);
    free(thread_output);
}
