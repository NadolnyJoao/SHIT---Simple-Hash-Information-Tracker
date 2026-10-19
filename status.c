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
    int numFilesDIR = 0;

    while ((de = readdir(dr)) != NULL)
    {
        // ignorar algsn arquivos, como o ., .. ,
        if (de->d_type != DT_REG)
        {
            continue;
        }

        // printf("trabalhado com o arquivo %s\n",de->d_name);

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

        FileSave *temp = realloc(filesOfDir, (numFilesDIR + 1) * sizeof(FileSave));
        if (temp == NULL)
        {
            printf("Erro para alocar memoria para a leitura de arquivos do diretorion\n");
            closedir(dr);
            return;
        }
        // printf("espaço de memoria reservado com sucesso \n");

        filesOfDir = temp;
        filesOfDir[numFilesDIR] = nfile;
        numFilesDIR++;
        // printf("%s type: %d\n", de->d_name,de->d_type);
    }
    closedir(dr);
    // pegar arquivos do ultimo commit
    char *hashFile = GetLastCommit();
    if (hashFile == NULL)
    {
        printf("Erro ao carregar o hash file: hash null linhas 74 status.c\n");
        free(hashFile);
        return;
    }
    Commit *c = ReadCommit(hashFile);

    printf("o ultimo commit possui %d arquivos projeto atual possui %d\n", c->num_files, numFilesDIR);

    
    // logica de compração
    /*grid copração
    0 arquivos com nome diferente
    1 arquivos com nome igual mas codigos diferentes
    2 arquivos com nome igual e codigo igual tambem
    regras
    lilnha vazia            = novo arquivo
    linha com somatoria 1   = arquivo modificado
    linha com somatoria 2   = arquivos iguais
    coluna vazia            = arquivo removido 
    */
    int matrixCoparation[255][255] = {0};
    const char * modificado[255];
    const char * novos[255];
    const char * removidos[255];
    int n_novos = 0, n_modificados = 0, n_removidos = 0;

    int ifd = 0; // id file direitorio
    int ifc = 0; // id file commit
    // create matriz de comparação 
    for (; ifd < numFilesDIR; ifd++)
    {
        for (ifc = 0; ifc < c->num_files; ifc++)
        {
            FileSave fd = filesOfDir[ifd];
            FileSave fc = c->files[ifc];
            // macth de name
            if (strcmp(fd.pathFile, fc.pathFile) == 0)
            {
                // compara hash
                if (strcmp(fd.hashVersion, fc.hashVersion) == 0)
                {
                    // arquivo igual
                    matrixCoparation[ifd][ifc] = 2;
                }
                else
                {
                    // arquivo modificado
                    matrixCoparation[ifd][ifc] = 1;
                }
            }
        }
    }
    // renderiza comparação
    printf("\n");
    // biuscar por arquios novos e modificado 
    for (ifd = 0; ifd < numFilesDIR; ifd++)
    {
        int sumRow = 0;
        int sumCol = 0;
        // printf("%s\t\t\t", filesOfDir[ifd].pathFile);
        for (ifc = 0; ifc < c->num_files; ifc++)
        {
            // printf("%d ", matrixCoparation[ifd][ifc]);
            sumRow+= matrixCoparation[ifd][ifc];
            sumCol+= matrixCoparation[ifc][ifd];
        }
        if(sumRow == 0){
            // printf("novo arquivo");
            novos[n_novos] = filesOfDir[ifd].pathFile;
            n_novos++;
        }else if(sumRow == 1){
            // printf("arquivo modificado");
            modificado[n_modificados] = filesOfDir[ifd].pathFile;
            n_modificados++;
        }
        if( sumCol ==0 ){
            removidos[n_removidos] = c->files[ifc].pathFile;
            n_removidos++;
        }
        // printf("\n");
    }
    free(c);
    free(hashFile);
    // comparar quais arquivos são novos
    // comparar se arquivso foram modificados
    // verificar se arquivso foram deltados
    // rederiza tudo
    if(n_modificados>0){

        printf("Arquivos modificados:\n");
        for(int i =0; i < n_modificados; i++){
            printf("\t%s\n",modificado[i]);
        }
        printf("\n");
    }
    if(n_novos>0){

        printf("Novos rquivos:\n");
        for(int i =0; i < n_novos; i++){
            printf("\t%s\n",novos[i]);
        }
        printf("\n");
    }
    if(n_removidos>0){
        
        printf("Arquivos removidos:\n");
        for(int i =0; i < n_removidos; i++){
            printf("\t%s\n",removidos[i]);
        }
        printf("\n");
    }
    return;

}
