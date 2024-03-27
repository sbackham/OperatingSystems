#include <string.h>
#include <openssl/sha.h>
#include <stdio.h> 
#include <stdlib.h>
#include <pthread.h>
#include <signal.h>
#include <unistd.h>
#include <openssl/evp.h>

typedef struct {
    unsigned short tid;
    unsigned short challenge;
    unsigned short numthreads; //number of threads 
} tinput_t;

unsigned int NSOLUTIONS = 8;

//the following two global variables are used by worker threads to communicate back the found results to the main thread
unsigned short found_solutions = 0;
unsigned long* solutions;

pthread_rwlock_t rwlock;  //create rwlock

unsigned short divisibility_check(unsigned long n) {
    //very not efficient algorithm
    for (unsigned long i = 1000000; i <= 1500000; i++) {
        if (n % i == 0) {
            return 0;
        }
    }
    return 1;
}

//uses EVP instead of SHA256
short try_solution(unsigned short challenge, unsigned long attempted_solution) {
    // Buffer for the hash
    unsigned char digest[EVP_MAX_MD_SIZE]; // Make sure this is large enough to hold any hash
    unsigned int digest_length; // Will hold the actual length of the hash

    // Create a context for the hash operation
    EVP_MD_CTX *ctx = EVP_MD_CTX_new();
    if (ctx == NULL) {
        fprintf(stderr, "Failed to create digest context\n");
        return 0;
    }

    // initialize the hash operation
    if (EVP_DigestInit_ex(ctx, EVP_sha256(), NULL) != 1) {
        // handle error
        EVP_MD_CTX_free(ctx);
        fprintf(stderr, "Failed to initialize digest context\n");
        return 0;
    }

    // message to be hashed
    if (EVP_DigestUpdate(ctx, &attempted_solution, sizeof(attempted_solution)) != 1) {
        EVP_MD_CTX_free(ctx);
        fprintf(stderr, "Failed to update digest\n");
        return 0;
    }

    // finalize the hash and get the digest
    if (EVP_DigestFinal_ex(ctx, digest, &digest_length) != 1) {
        EVP_MD_CTX_free(ctx);
        fprintf(stderr, "Failed to finalize digest\n");
        return 0;
    }

    EVP_MD_CTX_free(ctx);

    // check if the resulting hash matches the challenge
    if ((*(unsigned short*)digest) == challenge) {
        return 1;
    } else {
        return 0;
    }
   
}

//i tried to use SHA256 but the SHA functions were deprecated on my version of openSSL 3.0 so they suggested EVP instead for hashing

/*short try_solution(unsigned short challenge, unsigned long attempted_solution){
        //check if sha256(attempted_solution) is equal to digest
        //attempted_solution is converted to an 8byte buffer preserving endianness
        //the first 2 bytes of the hash are considered as a little endian 16bit number and compared to challenge
        unsigned char digest[SHA256_DIGEST_LENGTH];
        SHA256_CTX ctx;
        SHA256_Init(&ctx);
        SHA256_Update(&ctx, &attempted_solution, 8);
        SHA256_Final(digest, &ctx);

        if((*(unsigned short*)digest) == challenge){
            return 1;
        }else{
            return 0;
        }
} */

void* worker_thread_function(void *tinput_void) {
    tinput_t* tinput = (tinput_t*) tinput_void;
    unsigned long start = 0;
    unsigned long end = start + 1000000000l;
    unsigned long psize = 1000000000l / tinput->numthreads;  //psize for dividing the space among threads

    for (unsigned long attempted_solution = start; attempted_solution < end; attempted_solution++) {
        if (found_solutions == NSOLUTIONS) {
            break;
        }
        
        //condition1: sha256(attempted_solution) == challenge
        if (try_solution(tinput->challenge, attempted_solution)) {
            //condition2: the last digit must be different in all the solutions
            short bad_solution = 0;
            pthread_rwlock_wrlock(&rwlock); //set lock here

            for (int i = 0; i < found_solutions; i++) {
                if (attempted_solution % 10 == solutions[i] % 10) {
                    bad_solution = 1;
                    break;
                }
            }
            //condition3: no solution should be divisible by any number in the range [1000000, 1500000]
            if (bad_solution || !divisibility_check(attempted_solution)) {
                pthread_rwlock_unlock(&rwlock);  //unlock here
                continue;
            }

            solutions[found_solutions] = attempted_solution;
            found_solutions++;
            pthread_rwlock_unlock(&rwlock);  //unlock

            if (found_solutions == NSOLUTIONS) {
                return NULL;
            }
        }
    }

    return NULL;
}

void solve_one_challenge(unsigned short challenge, unsigned short numthreads) {
    pthread_t th[numthreads];
    tinput_t inputs[numthreads];

    pthread_rwlock_init(&rwlock, NULL); //init lock here 

    found_solutions = 0;
    solutions = (unsigned long*)malloc(NSOLUTIONS * (sizeof(unsigned long)));
    memset(solutions, 0, NSOLUTIONS * sizeof(unsigned long));

    for (int i = 0; i < numthreads; i++) {
        inputs[i].tid = i;
        inputs[i].challenge = challenge;
        inputs[i].numthreads = numthreads;
        pthread_create(&th[i], NULL, worker_thread_function, &inputs[i]);
    }

    for (int i = 0; i < numthreads; i++) {
        pthread_join(th[i], NULL);
    }

    printf("%hu", challenge);  // print solns in same format as hw6
    for (unsigned short i = 0; i < NSOLUTIONS; i++) {
        printf(" %lu", solutions[i]); 
    }
    printf("\n");  // end with a newline

    free(solutions);
    pthread_rwlock_destroy(&rwlock);
 
}

int main(int argc, char* argv[]) {
    // argv[1] is the number of worker threads we must use
    // the other arguments are the challenges we must solve
    if (argc < 3) {
        printf("Usage: %s <numthreads> <challenges...>\n", argv[0]);
        return 1;
    }

	//make sure threads are between 0 and 100
    // Make sure threads are between 1 and 99
    unsigned short numthreads = strtol(argv[1], NULL, 10);
    if (numthreads == 0 || numthreads > 99) {
        printf("The number of threads should be between 1 and 99.\n");
    return 1;
}


    // loop through each challenge provided as command-line argument
    for (int i = 2; i < argc; i++) {
        unsigned short challenge = strtol(argv[i], NULL, 10);
        solve_one_challenge(challenge, numthreads);
    }

    return 0;
}

/*
compile using:
gcc -ggdb -O0 -o task task.c -lpthread -lcrypto

It may require installing libssl-dev:
sudo apt-get install libssl-dev
*/
