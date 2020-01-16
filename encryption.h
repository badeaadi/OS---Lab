
#ifndef ENCRYPTOR_ENCRYPTION_H
#define ENCRYPTOR_ENCRYPTION_H

#include <stdio.h>
#include <fcntl.h>
#include <errno.h>
#include <zconf.h>
#include <time.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/wait.h>

int *get_random_permutation(int len)
{
    int *perm = malloc(len * sizeof(int));
    int i, j, k, e[len];

    for (i = 0; i < len; i++)
        e[i] = i + 1;

    for (i = 0; i < len; i++) {
        k = rand() % (len - i) + 1;
        j = -1;
        while (k) {
            j++;
            if (e[j])
                k--;
        }
        perm[i] = e[j] - 1;
        e[j] = 0;
    }
    return perm;
}
const char perm_name [] = "perm.in";


int encrypt(char *filename)
{
    FILE * file_in = fopen(filename, "r");
    int i, j, n = 0;
    char *word = malloc(50 * sizeof(char));

    while (fscanf(file_in, "%s ", word) != -1) {
        n++;
    }

    free(word);

    fclose(file_in);
    file_in = fopen(filename, "r");

    FILE * file_perm = fopen(perm_name, "w");

    char shm_name[] = "myshm";
    int shm_fd = shm_open(shm_name, O_CREAT|O_RDWR, S_IRUSR|S_IWUSR);
    if (shm_fd < 0) {
        perror(NULL);
        printf("Error at shm_open\n");
        return errno;
    }

    if (ftruncate(shm_fd, n * getpagesize()) == -1) {
        perror(NULL);
        shm_unlink(shm_name);
        printf("Error at ftruncate\n");
        return errno;
    }
    srand(time(NULL));

    pid_t pids[n];

    for (i = 0; i < n; i++) {
        char *word = malloc(50 * sizeof(char));

        fscanf(file_in, "%s", word);
        int nword = strlen(word);

        pids[i] = fork();
        if (pids[i] < 0) {
            perror("fork"); abort();
        }
        else
        if (pids[i] == 0) {
            printf("Started process %d ...\n", getpid());
            int *perm = get_random_permutation(nword);

            printf("%d letters in the %dth word\n", nword, i + 1);

            fprintf(file_perm, "%d\n", nword);
            for (j = 0; j < nword; j++)
                fprintf(file_perm, "%d ", perm[j]);
            fprintf(file_perm, "\n");

            char *shm_ptr = mmap(0, 100, PROT_WRITE, MAP_SHARED, shm_fd, getpagesize() * i);

            int index = 0;
            for (j = 0; j < strlen(word); j++)
                shm_ptr[index++] = word[perm[j]];

            free(perm);
            free(word);

            munmap(shm_ptr, getpagesize());
            return 0;
        }
    }
    fclose(file_in);
    fclose(file_perm);

    FILE * file_out = fopen(filename, "w");

    int child_status;
    pid_t pid;

    int nn = n;
    while (n > 0) {
        pid = wait(&child_status);
        printf("Child with pid %ld exited with status %d.\n", (long)pid, child_status);
        n -= 1;
    }
    printf("Now we will be retrieving everything from shared memory.\n");

    for (int i = 0; i < nn; i++) {
        char* shm_ptr = mmap(0, 100, PROT_READ, MAP_SHARED, shm_fd, getpagesize() * i);

        while (*shm_ptr != 0) {
            fprintf(file_out, "%c", *shm_ptr);
            shm_ptr++;
        }
        fprintf(file_out, " ");
    }
    fclose(file_out);
    free(file_in); free(file_perm); free(file_out);
    
    shm_unlink(shm_name);
    return 0;
}

#endif
