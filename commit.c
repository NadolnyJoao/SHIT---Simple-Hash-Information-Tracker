#include "commit.h"
#include <time.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <sha1.h>

void CreateCommit(char *flags[], int num_flags)
{
    // ler arquivos que estão na pasta index
    // criar uma função que faz isso

    time_t t = time(NULL);
    char *today = ctime(&t);

    Commit nCommit;
    // hora do commit
    strncpy(nCommit.date, today, sizeof(nCommit.date) - 1);
    nCommit.date[sizeof(nCommit.date) - 1] = '\0';

    // mensagem
    char *mensage;

    for (int i = 0; i < num_flags; i++)
    {
        if (strcmp(flags[i], "-m") == 0)
        {
            // proxima flag tem que ser a mensagem
            // verifica se temos mais uma flag
            if (i + 1 >= num_flags)
            {
                printf("Erro: -m exigi uma mensagem em seguida\n");
                return;
            }
            mensage = flags[i + 1];
            break;
        }
    }

    if (mensage == NULL)
    {
        printf("por que não deu certo");
        return;
    }

    strncpy(nCommit.menssage, mensage, sizeof(nCommit.menssage) - 1);
    nCommit.menssage[sizeof(nCommit.menssage) - 1] = '\0';

    // fazer a configuração dos arquivos
    //setando variaveiz sacanas
    nCommit.files = NULL;
    nCommit.num_files = 0;
    FILE *fileIndex = fopen(".shit/index", "r");
    // ler cada linha
    char linha[100];
    while (fgets(linha, sizeof(linha), fileIndex))
    {
        linha[strcspn(linha, "\r\n")] = 0;
        const char delimitador[] = "\t";
        FileSave nfile;
        // Primeira chamada: passa a string original
        char *token = strtok(linha, delimitador);

        strncpy(nfile.pathFile, token, sizeof(nfile.pathFile) - 1);
        nfile.pathFile[sizeof(nfile.pathFile) - 1] = '\0';
        token = strtok(NULL, delimitador);
        strncpy(nfile.hashVersion, token, sizeof(nfile.hashVersion) - 1);
        nfile.hashVersion[sizeof(nfile.hashVersion) - 1] = '\0';

        // printf("filesave\nname: %s\nhash: %s\n\n", nfile.pathFile,nfile.hashVersion);

        // adicionar essa porra na estrutura do commit FileSave* files;
        int nNum_files = nCommit.num_files + 1;
        FileSave* temp = realloc(nCommit.files, nNum_files * sizeof(FileSave));
        if(temp == NULL){
            printf("Erro para alocar memoria ");
            fclose(fileIndex);
            return;
        }
        nCommit.files = temp;
        nCommit.files[nCommit.num_files] = nfile;
        nCommit.num_files = nNum_files;
    }
    fclose(fileIndex);
    // printf("ola mundo");


    // gerar id_hash
    SHA1_CTX contexto;
    unsigned char digist[20];
    SHA1Init(&contexto);
    SHA1Update(&contexto, (unsigned char *)nCommit.date, sizeof(nCommit.date));
    SHA1Final(digist, &contexto);

    char *id_hash = malloc(41);
    if (id_hash == NULL)
    {
        printf("Erro critico na geração do id_hash");
        return;
    }
    for (int i = 0; i < 20; i++)
    {
        sprintf(&id_hash[i * 2], "%02x", digist[i]);
    }
    id_hash[40] = '\0';

    // geristrar isso no head

    // finalização
    printf("commit gerado com sucesso\nid:%s\nnumero de files:%d\ndata: %s\n        %s", id_hash,nCommit.num_files, nCommit.date, nCommit.menssage);
}