#ifndef ENCRYPTOR_DECRYPTION_H
#define ENCRYPTOR_DECRYPTION_H

const int maxn = 5000;



int decrypt(char *filename, char *perm_targetname)
{
    FILE * file_in = fopen(filename, "r");
    int i, j, n = 0;

    char *word = malloc(50 * sizeof(char));

    while (fscanf(file_in, "%s ", word) != -1) {
        n++;
    }

    free(word);

    printf("%d\n", n);

    fclose(file_in);
    file_in = fopen(filename, "r");


    FILE * file_perm = fopen(perm_targetname, "r");

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


    pid_t pids[n];

    int np[n];
    int *perm[n];

    for (i = 0; i < n; i++) {
        fscanf(file_perm, "%d", &np[i]);

        perm[i] = malloc(np[i] * sizeof(int));

        for (j = 0; j < np[i]; j++)
            fscanf(file_perm, "%d", &perm[i][j]);
    }
    /*
    for (i = 0; i < n; i++) {
        printf("%d\n", np[i]);
        for (j = 0; j < np[i]; j++)
            printf("%d ", perm[i][j]);
        printf("\n");
    }
*/
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

            int *invperm = malloc(sizeof(int) * nword);
            for (j = 0; j < nword; j++)
                invperm[perm[i][j]] = j;

            printf("%d letters in the %dth word\n", nword, i + 1);

            char *shm_ptr = mmap(0, 100, PROT_WRITE, MAP_SHARED, shm_fd, getpagesize() * i);

            int index = 0;
            for (j = 0; j < strlen(word); j++)
                shm_ptr[index++] = word[invperm[j]];

            free(invperm);
            free(word);

            munmap(shm_ptr, getpagesize());
            return 0;
        }
    }


    int child_status;
    pid_t pid;

    int nn = n;
    while (n > 0) {
        pid = wait(&child_status);
        printf("Child with pid %ld exited with status %d.\n", (long)pid, child_status);
        n -= 1;
    }
    printf("Now we will be retrieving everything from shared memory.\n");

    FILE * decrypted_file = fopen(filename, "w");

    for (int i = 0; i < nn; i++) {
        char * shm_ptr = mmap(0, 100, PROT_READ, MAP_SHARED, shm_fd, getpagesize() * i);

        while (*shm_ptr != 0) {
            fprintf(decrypted_file, "%c", *shm_ptr);
            shm_ptr++;
        }
        fprintf(decrypted_file, " ");
    }
    fclose(decrypted_file);
    remove(perm_name);

    fclose(file_perm);
    shm_unlink(shm_name);
    return 0;
}
#endif
