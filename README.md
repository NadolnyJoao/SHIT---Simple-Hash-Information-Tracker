# SHIT — Simple Hash Information Tracker

O SHIT é um sistema minimalista de controle de versão escrito em C, desenvolvido para fins educacionais. Utiliza snapshots para registrar o histórico de um projeto, sem depender de algoritmos complexos de comparação de arquivos ou ferramentas externas.

## Comandos

- `init` — Inicializa um repositório.
- `add <arquivo>` — Adiciona um arquivo à área de preparação.
- `commit -m "mensagem"` — Registra um snapshot com uma mensagem.
- `log` — Exibe o histórico de commits.
- `reset <commit-id>` — Restaura os arquivos para um snapshot anterior.

## Estrutura

Os dados do repositório são armazenados no diretório `.shit/`:

- `objects/` — Armazena versões dos arquivos e registros de commits.
- `index` — Mantém a lista de arquivos preparados.
- `HEAD` — Aponta para o commit mais recente.

Cada commit registra seu commit pai, sua mensagem e os arquivos incluídos.

## Compilação

Requer CMake 3.16+ e um compilador C (GCC, Clang ou MSVC).

```bash
cmake -B build
cmake --build build
```

Execute o binário gerado:

- Linux/macOS: `./build/shit`
- Windows: `build\shit.exe` (Ninja) ou `build\Debug\shit.exe` (Visual Studio)

Compilação rápida sem CMake (Linux/macOS):

```bash
gcc -std=c11 -Wall -Wextra -I. *.c -o shit
```

## Testes

A suíte de testes de linha de comando fica em [`tests/`](tests/). Ela executa o
binário em um diretório temporário e verifica o comportamento observável:

- criação da estrutura pelo `init`;
- integridade dos objetos e do índice gerados pelo `add` (compara os hashes
  com o `sha1sum`);
- casos de borda (nomes com espaços, arquivo vazio, binário e arquivo grande);
- tratamento de erros e códigos de saída.

Via CTest (recomendado):

```bash
ctest --test-dir build --output-on-failure
```

Diretamente pelo script:

```bash
./tests/test_cli.sh
```

Consulte [`tests/README.md`](tests/README.md) para a cobertura completa e para
informar um binário específico.

## Filosofia

O SHIT prioriza simplicidade, portabilidade e clareza. Sua arquitetura baseada em snapshots e sua implementação em C oferecem uma introdução prática aos conceitos de controle de versão, sem depender de ferramentas externas de comparação de arquivos.
