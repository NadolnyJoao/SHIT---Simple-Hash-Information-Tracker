#ifndef FILE_UTILS_H
#define FILE_UTILS_H

// Le o arquivo inteiro para a memoria e devolve um buffer alocado pelo chamador
// (que deve dar free) e, em *out_size, o numero de bytes lidos.
// Retorna NULL em caso de erro. O buffer NAO termina com '\0', por isso sempre
// use o tamanho retornado junto com os dados.
char *read_file(const char *path, long *out_size);

// Calcula o SHA-1 do conteudo do arquivo e devolve a representacao em
// hexadecimal (40 caracteres + '\0'). O chamador deve liberar com free.
char *calcula_sha1(const char *path) ;

#endif