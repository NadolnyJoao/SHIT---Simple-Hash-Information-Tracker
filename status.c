#include <stdio.h>

#include "dirent.h"
#include "sha1.h"

#include "status.h"
#include "commit.h"
#include "file_utils.h"

void Status()
{
    // pegar arquivos do diretorio

    struct dirent *de;
    DIR *dr = opendir(".");
    if (dr == NULL)
    {
        printf("Erro ao abrir o diretorio para analise\n");
        return;
    }
    printf("Seguimos com o diretorio aberto para analise\n");

    FileSave *filesOfDir = NULL;
    int numFiles = 0;

    while ((de = readdir(dr)) != NULL)
    {
        // ignorar algsn arquivos, como o ., .. , 
        if(de->d_type != DT_REG){
            continue;
        }

        printf("trabalhado com o arquivo %s\n",de->d_name);
        
        FileSave nfile;
        strncpy(nfile.pathFile, de->d_name, sizeof(nfile.pathFile) - 1);
        nfile.pathFile[sizeof(nfile.pathFile) - 1] = '\0';
        // printf("registrado nome no filesave agora criano o hash\n");
        // create hash
        char *hasFile = calcula_sha1(de->d_name);
        if (hasFile == NULL)
        {
            printf("Erro na geracao do has files do arquivo %s lina 26 status.c\n", de->d_name);
            free(filesOfDir);
            closedir(dr);
            return;
        }

        strncpy(nfile.hashVersion, hasFile, sizeof(nfile.hashVersion) - 1);
        nfile.hashVersion[sizeof(nfile.hashVersion) - 1] = '\0';
        free(hasFile); 
        
        // printf("hash resgitrado no filesave,parti para adicionar na lista de arquivos\n");
        // adiconar a lista de files
        
        FileSave *temp = realloc(filesOfDir, (numFiles + 1) * sizeof(FileSave));
        if (temp == NULL)
        {
            printf("Erro para alocar memoria para a leitura de arquivos do diretorion\n");
            closedir(dr);
            return;
        }
        // printf("espaço de memoria reservado com sucesso \n");

        filesOfDir = temp;
        filesOfDir[numFiles] = nfile;
        numFiles ++;
        // printf("%s type: %d\n", de->d_name,de->d_type);
    }
    closedir(dr);
    return;
    // pegar arquivos do ultimo commit

    // comparar quais arquivos são novos
    // comparar se arquivso foram modificados
    // verificar se arquivso foram deltados
    // rederiza tudo
}
