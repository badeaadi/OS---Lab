# Encryptor, using pids and shared memory


## Compile

```
gcc encryptor.c -o encryptor -lrt

```
## Start

To encrypt a file :

./encryptor encrypt FILENAME

```

To decrypt a file :
./decryptor decrypt FILENAME FILEPERM


Where FILENAME is the name of the file to encrypt/decrypt (for example file.in) and
FILEPERM is implicitly perm.in (file containing the permutations randomly generated from the encryption, needed for decryption)
