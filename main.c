#include <string.h>
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>
#include <stdlib.h>

#include "encryption.h"
#include "decryption.h"

int main(int argc, char const *argv[])
{
    int n = argc - 1;

    if (strcmp(argv[1], "encrypt") == 0 && n == 2) {
        char * str = malloc(sizeof(char) * strlen(argv[2]));
        strcpy(str, argv[2]);
        int retval = encrypt(str);
        free(str);
        return retval;
    }

    if (strcmp(argv[2], "decrypt") && n == 3)  {
        char * str1 = malloc(sizeof(char) * strlen(argv[2]));
        char * str2 = malloc(sizeof(char) * strlen(argv[3]));
        strcpy(str1, argv[2]);
        strcpy(str2, argv[3]);
        int retval = decrypt(str1, str2);
        free(str1); free(str2);
        return retval;
    }
    
    if (strcmp(argv[1], "help") == 0) {
        printf("Encryption app using pthreads and shared memory - encryption and decryption available:\n");
        printf("Format of commands:\n");
        printf("./encryptor encrypt filename\n");
        printf("./encryptor decrypt filename perm_filename\n");
        return 0;
    }
    printf("Wrong command");

    return 0;
}
