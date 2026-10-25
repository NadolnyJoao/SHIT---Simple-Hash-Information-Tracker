#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifdef _WIN32
#include "compat/dirent.h"
#else
#include <dirent.h>
#endif

#include "sha1.h"
#include "status.h"
#include "commit.h"
#include "file_utils.h"

int Encontra_Mudancas(Mudanca **lista, int *qtd)
{
    *lista = NULL;
    *qtd = 0;

    struct dirent *de;
    DIR *dr = opendir(".");
    if (dr == NULL)
    {
        printf("Erro ao abrir o diretorio para analise\n");
        return 0;
    }

    FileSave *filesOfDir = NULL;
    int numFilesDIR = 0;

    while ((de = readdir(dr)) != NULL)
    {
        if (de->d_type != DT_REG)
        {
            continue;
        }

        FileSave nfile;
        strncpy(nfile.pathFile, de->d_name, sizeof(nfile.pathFile) - 1);
        nfile.pathFile[sizeof(nfile.pathFile) - 1] = '\0';

        char *hasFile = calcula_sha1(de->d_name);
        if (hasFile == NULL)
        {
            printf("Erro na geracao do has files do arquivo %s lina 26 status.c\n", de->d_name);
            free(filesOfDir);
            closedir(dr);
            return 0;
        }

        strncpy(nfile.hashVersion, hasFile, sizeof(nfile.hashVersion) - 1);
        nfile.hashVersion[sizeof(nfile.hashVersion) - 1] = '\0';
        free(hasFile);

        FileSave *temp = realloc(filesOfDir, (numFilesDIR + 1) * sizeof(FileSave));
        if (temp == NULL)
        {
            printf("Erro para alocar memoria para a leitura de arquivos do diretorion\n");
            free(filesOfDir);
            closedir(dr);
            return 0;
        }

        filesOfDir = temp;
        filesOfDir[numFilesDIR] = nfile;
        numFilesDIR++;
    }
    closedir(dr);

    char *hashFile = GetLastCommit();
    if (hashFile == NULL)
    {
        printf("Nenhum commit encontrado no historico.\n");
        free(filesOfDir);
        return 0;
    }
    Commit *c = ReadCommit(hashFile);
    if (c == NULL)
    {
        free(hashFile);
        if (numFilesDIR > 0)
        {
            *lista = (Mudanca *)malloc(numFilesDIR * sizeof(Mudanca));
            if (*lista == NULL)
            {
                free(filesOfDir);
                return 0;
            }
            *qtd = numFilesDIR;
            for (int i = 0; i < numFilesDIR; i++)
            {
                strncpy((*lista)[i].caminho, filesOfDir[i].pathFile, 255);
                (*lista)[i].caminho[255] = '\0';
                (*lista)[i].tipo = MUDANCA_NOVO;
            }
            free(filesOfDir);
            return 1;
        }
        printf("Nenhum commit encontrado no historico.\n");
        free(filesOfDir);
        return 0;
    }

    int matrixCoparation[255][255] = {{0}};
    int ifd = 0;
    int ifc = 0;
    for (; ifd < numFilesDIR; ifd++)
    {
        for (ifc = 0; ifc < c->num_files; ifc++)
        {
            FileSave fd = filesOfDir[ifd];
            FileSave fc = c->files[ifc];
            if (strcmp(fd.pathFile, fc.pathFile) == 0)
            {
                if (strcmp(fd.hashVersion, fc.hashVersion) == 0)
                {
                    matrixCoparation[ifd][ifc] = 2;
                }
                else
                {
                    matrixCoparation[ifd][ifc] = 1;
                }
            }
        }
    }

    for (ifd = 0; ifd < numFilesDIR; ifd++)
    {
        int sumRow = 0;
        for (ifc = 0; ifc < c->num_files; ifc++)
        {
            sumRow += matrixCoparation[ifd][ifc];
        }
        if (sumRow == 0)
        {
            (*qtd)++;
        }
        else if (sumRow == 1)
        {
            (*qtd)++;
        }
    }

    for (ifc = 0; ifc < c->num_files; ifc++)
    {
        int presente = 0;
        for (ifd = 0; ifd < numFilesDIR; ifd++)
        {
            if (matrixCoparation[ifd][ifc] != 0)
            {
                presente = 1;
                break;
            }
        }
        if (!presente)
        {
            (*qtd)++;
        }
    }

    *lista = (Mudanca *)malloc((*qtd) * sizeof(Mudanca));
    if (*lista == NULL)
    {
        free(c->files);
        free(c);
        free(hashFile);
        free(filesOfDir);
        return 0;
    }

    int idx = 0;
    for (ifd = 0; ifd < numFilesDIR; ifd++)
    {
        int sumRow = 0;
        for (ifc = 0; ifc < c->num_files; ifc++)
        {
            sumRow += matrixCoparation[ifd][ifc];
        }
        if (sumRow == 0)
        {
            strncpy((*lista)[idx].caminho, filesOfDir[ifd].pathFile, 255);
            (*lista)[idx].caminho[255] = '\0';
            (*lista)[idx].tipo = MUDANCA_NOVO;
            idx++;
        }
        else if (sumRow == 1)
        {
            strncpy((*lista)[idx].caminho, filesOfDir[ifd].pathFile, 255);
            (*lista)[idx].caminho[255] = '\0';
            (*lista)[idx].tipo = MUDANCA_MODIFICADO;
            idx++;
        }
    }

    for (ifc = 0; ifc < c->num_files; ifc++)
    {
        int presente = 0;
        for (ifd = 0; ifd < numFilesDIR; ifd++)
        {
            if (matrixCoparation[ifd][ifc] != 0)
            {
                presente = 1;
                break;
            }
        }
        if (!presente)
        {
            strncpy((*lista)[idx].caminho, c->files[ifc].pathFile, 255);
            (*lista)[idx].caminho[255] = '\0';
            (*lista)[idx].tipo = MUDANCA_REMOVIDO;
            idx++;
        }
    }

    free(c->files);
    free(c);
    free(hashFile);
    free(filesOfDir);
    return 1;
}

void Status()
{
    Mudanca *lista = NULL;
    int qtd = 0;

    if (!Encontra_Mudancas(&lista, &qtd))
    {
        return;
    }

    int n_modificados = 0, n_novos = 0, n_removidos = 0;
    for (int i = 0; i < qtd; i++)
    {
        if (lista[i].tipo == MUDANCA_MODIFICADO)
            n_modificados++;
        else if (lista[i].tipo == MUDANCA_NOVO)
            n_novos++;
        else if (lista[i].tipo == MUDANCA_REMOVIDO)
            n_removidos++;
    }

    if (n_modificados > 0)
    {
        printf("Arquivos modificados:\n");
        for (int i = 0; i < qtd; i++)
        {
            if (lista[i].tipo == MUDANCA_MODIFICADO)
                printf("\t%s\n", lista[i].caminho);
        }
        printf("\n");
    }
    if (n_novos > 0)
    {
        printf("Novos rquivos:\n");
        for (int i = 0; i < qtd; i++)
        {
            if (lista[i].tipo == MUDANCA_NOVO)
                printf("\t%s\n", lista[i].caminho);
        }
        printf("\n");
    }
    if (n_removidos > 0)
    {
        printf("Arquivos removidos:\n");
        for (int i = 0; i < qtd; i++)
        {
            if (lista[i].tipo == MUDANCA_REMOVIDO)
                printf("\t%s\n", lista[i].caminho);
        }
        printf("\n");
    }

    free(lista);
    return;
}
