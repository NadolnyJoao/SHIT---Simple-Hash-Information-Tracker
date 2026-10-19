#ifndef COMMIT_H
#define COMMIT_H

#define MAX_LEN_HASH 41
#if defined(_WIN32)|| defined(_WIN64)
    #include <windows.h>
    #define SH_PATH_MAX MAX_PATH
    /* Cria um nome comum SH_PATH_MAX que tem um valor diferente dependendo da plataforma que o programa esta sendo compilado  */
#elif defined(__linux__)
    #include <limits.h>
    #define SH_PATH_MAX PATH_MAX
#else 
    #define SH_PATH_MAX 4096
    /* fallback para outras plataformas */
#endif 

/*
como o commit funciona
ele é uma estrutura com os dados:
id: hash
data da criação
os arquivos { name : hash }
como caralhos eu sei quas são so arquivos?

ideia

head - hash commit
este commit tem commit anterior
e assim eu consigo a ordem dos commit

*/
typedef struct{
    char pathFile[SH_PATH_MAX]; /* <---- */
    char hashVersion[MAX_LEN_HASH];

}FileSave;

typedef struct
{
    char id_hash[MAX_LEN_HASH];
    char date[100];
    char menssage[255];
    FileSave* files;
    int num_files;
    char previous[MAX_LEN_HASH];

} Commit;

Commit *CreateCommit(char* flags[], int num_flags);
void RegisterCommitFile(const Commit *c);
Commit *ReadCommit(const char *hash);
char * GetLastCommit();
#endif
