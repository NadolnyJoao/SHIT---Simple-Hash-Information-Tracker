#include "commit.h"
#include <time.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <sha1.h>

int FileOfIndex(FileSave **files, int *num_files)
{
    *files = NULL;
    *num_files = 0;

    FILE *fileIndex = fopen(".shit/index", "r");
    // ler cada linha
    char linha[100];
    if (fileIndex == NULL)
    {
        printf("Erro ao abrir file index");
        return -1;
    }
    while (fgets(linha, sizeof(linha), fileIndex))
    {
        linha[strcspn(linha, "\r\n")] = 0;
        const char delimitador[] = "\t";
        FileSave nfile;
        // Primeira chamada: passa a string original
        char *token = strtok(linha, delimitador);
        if (token == NULL)
            continue;

        strncpy(nfile.pathFile, token, sizeof(nfile.pathFile) - 1);
        nfile.pathFile[sizeof(nfile.pathFile) - 1] = '\0';

        token = strtok(NULL, delimitador);
        if (token == NULL)
            continue;

        strncpy(nfile.hashVersion, token, sizeof(nfile.hashVersion) - 1);
        nfile.hashVersion[sizeof(nfile.hashVersion) - 1] = '\0';

        // printf("filesave\nname: %s\nhash: %s\n\n", nfile.pathFile,nfile.hashVersion);

        // adicionar essa porra na estrutura do commit FileSave* files;
        int nNum_files = *num_files + 1;
        FileSave *temp = realloc(*files, nNum_files * sizeof(FileSave));
        if (temp == NULL)
        {
            printf("Erro para alocar memoria ");
            fclose(fileIndex);
            return -1;
        }
        *files = temp;
        *(files)[*num_files] = nfile;
        *num_files = nNum_files;
    }
    fclose(fileIndex);
    return *num_files;
}

int CreateIdHash(char id_hash[41], const char *key)
{
    SHA1_CTX contexto;
    unsigned char digest[20];

    SHA1Init(&contexto);
    SHA1Update(&contexto, (unsigned char *)key, strlen(key));
    SHA1Final(digest, &contexto);

    for (int i = 0; i < 20; i++)
        snprintf(&id_hash[i * 2], 3, "%02x", digest[i]);
    id_hash[40] = '\0';

    return 1;
}

int GetDate(char date[100])
{
    time_t t = time(NULL);
    char *today = ctime(&t);
    if (today == NULL)
        return -1;

    strncpy(date, today, 24);
    date[24] = '\0';

    return 1;
}

int CanCreateCommit(){
    // verificar se index esta vaziu
    FILE *indexFile  = fopen(".shit/index","r");
    if(indexFile  == NULL){
        printf("Erro ao abrir index para verificar se pode fazer commit");
        fclose(indexFile);
        return -1;
    }
    char line[100];
    if(fgets(line,sizeof(line),indexFile) == NULL){
        printf("Faça um shit add para ter arquivo ara commitar");
        fclose(indexFile);
        return 0;
    }

    fclose(indexFile);

    return 1;

}

Commit *CreateCommit(char *flags[], int num_flags)
{
    // verifica se pode faze commit
    if(CanCreateCommit()<=0){
        return NULL;
    }
    Commit *nCommit = calloc(1, sizeof(Commit));
    if (nCommit == NULL)
    {
        printf("Erro ao alocar Commit\n");
        return NULL;
    }

    // pegar hora do commit
    if (GetDate(nCommit->date) < 0)
    {
        printf("Erro ao obter data\n");

        free(nCommit);
        return NULL;
    }

    // mensagem
    char *mensage = NULL;

    for (int i = 0; i < num_flags; i++)
    {
        if (strcmp(flags[i], "-m") == 0)
        {
            // proxima flag tem que ser a mensagem
            // verifica se temos mais uma flag
            if (i + 1 >= num_flags)
            {
                printf("Erro: -m exigi uma mensagem em seguida\n");
                free(nCommit);
                return NULL;
            }
            mensage = flags[i + 1];
            break;
        }
    }

    if (mensage == NULL)
    {
        printf("por que não deu certo");
        free(nCommit);
        return NULL;
    }

    strncpy(nCommit->menssage, mensage, sizeof(nCommit->menssage) - 1);
    nCommit->menssage[sizeof(nCommit->menssage) - 1] = '\0';

    // fazer a configuração dos arquivos
    int nfiles = FileOfIndex(&nCommit->files, &nCommit->num_files);
    if (nfiles < 0)
    {
        printf("Erro na leitura de arquivos pela funçaõ FileOfindex");
        free(nCommit);
        return NULL;
    }

    // gerar id_hash
    if (CreateIdHash(nCommit->id_hash, nCommit->date) < 0)
    {
        printf("Erro na criaçõ de hash pela funçaõ CreateIdHash\n");
        free(nCommit);
        return NULL;
    }

    // pega ultimo commit pra adicionar no previus
    FILE *head = fopen(".shit/HEAD", "r");
    if (head == NULL)
    {
        printf("Erro ao abrir heda file, linha 116 commit.c");
        free(nCommit);
        return NULL;
    }
    // pegar a primeira linha
    strncpy(nCommit->previous, " ", sizeof(nCommit->previous) - 1);
    nCommit->previous[sizeof(nCommit->previous) - 1] = '\0';
    char id_hash_previus[42];
    if (fgets(id_hash_previus, sizeof(id_hash_previus), head) == NULL)
    {
        printf("Erro ao buscar ultimo commit no head ou não tem ");
        fclose(head);
        free(nCommit);
        return NULL;
    }
    fclose(head);
    strncpy(nCommit->previous, id_hash_previus, sizeof(nCommit->previous) - 1);
    nCommit->previous[sizeof(nCommit->previous) - 1] = '\0';
    // printf("ultimo commit : %s\n",id_hash_previus);

    // geristrar isso no head
    head = fopen(".shit/HEAD", "w");
    if (head == NULL)
    {
        printf("Erro ao abrir head apra registra o commir novo ");
        fclose(head);
        free(nCommit);
        return NULL;
    }
    fprintf(head, nCommit->id_hash);
    fclose(head);

    // finalização
    printf("commit gerado com sucesso\nid:%s\nultimo commit: %s\nnumero de files:%d\ndata: %s\n        %s\n\n", nCommit->id_hash, nCommit->previous, nCommit->num_files, nCommit->date, nCommit->menssage);
    // registrar em arquivo  o commit
    RegisterCommitFile(nCommit);
    // limpra index
    FILE *indexFile = fopen(".shit/index","w");
    if(indexFile == NULL){
        printf("Erro ao abrir index file para limpar na função create commit");
        fclose(indexFile);
        free(nCommit);
        return NULL;
    }
    fclose(indexFile);// espero que limpe

    return nCommit;
}

void RegisterCommitFile(const Commit *c)
{
    char path[256];
    snprintf(path, sizeof(path), ".shit/commits/%s", c->id_hash);
    FILE *file = fopen(path, "w");
    if (file == NULL)
    {
        printf("Erro na abertura do arquico commit ");
        fclose(file);
        return;
    }
    fprintf(file, "COMMIT\n");
    fprintf(file, "id %s\n", c->id_hash);
    fprintf(file, "date %s\n", c->date);
    fprintf(file, "prev %s\n", c->previous);
    fprintf(file, "msg %s\n", c->menssage);
    fprintf(file, "num_files %d\n", c->num_files);
    for (int i = 0; i < c->num_files; i++)
    {
        // char lineFileSave[255];
        FileSave fs;
        fs = c->files[i];
        // snprintf(lineFileSave, sizeof(lineFileSave), "%s %s", fs.pathFile, fs.hashVersion);
        fprintf(file, fs.pathFile, fs.hashVersion);
    }
    fprintf(file, "END\n");

    fclose(file);

    printf("commit salcom sucesso ");
}