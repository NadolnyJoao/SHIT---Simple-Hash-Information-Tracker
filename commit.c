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
    printf("commit gerado com sucesso\nid:%s\ndata: %s\n        %s", id_hash, nCommit.date, nCommit.menssage);


}