#include <pthread.h>

#define MAX_COMPRESSED_SIZE 1000000

// any additional variables, functions, structs etc.

// ------------------
// A helper function that compresses an assigned workload of text;
// meant to be called once for each thread via pthread_create (see docs).
// ------------------
void* pzip(void* args) {
    // TODO: process through and compress all assigned text
}

// GLHF!
int main(int argc, char *argv[]) {
    // TODO: usage checking, process arguments

    // TODO: mmap file to memory

    // TODO: distribute workload and spawn threads
    
    // TODO: write all compressed results to stdout 

    // cleanup
}
