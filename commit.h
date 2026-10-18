#ifndef COMMIT_H
#define COMMIT_H

#define MAX_LEN_HASH 41

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
    char pathFile[255];
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

#endif