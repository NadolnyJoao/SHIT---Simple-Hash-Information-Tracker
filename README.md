# SHIT — Simple Hash Information Tracker

O SHIT é um sistema minimalista de controle de versão escrito em C, desenvolvido para fins educacionais. Utiliza snapshots para registrar o histórico de um projeto, sem depender de algoritmos complexos de comparação de arquivos ou ferramentas externas.

## Comandos

Implementados:

- `init` — Inicializa um repositório.
- `add <arquivo>` — Adiciona um arquivo à área de preparação.
- `add .` — Adiciona ao índice apenas arquivos novos ou modificados em relação ao último commit.
- `register -m "mensagem"` — Registra um snapshot com uma mensagem.
- `status` — Compara a pasta de trabalho com o último commit.

Planejados:

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

## Instalação

Para usar o comando `shit` em qualquer diretório, sem informar o caminho do
binário, use um dos instaladores. Eles compilam o projeto, instalam o binário
e configuram o `PATH`.

Linux/macOS:

```bash
./install.sh                  # instala em ~/.local/bin
./install.sh /usr/local/bin   # ou informe outra pasta
```

Windows (PowerShell):

```powershell
powershell -ExecutionPolicy Bypass -File install.ps1
```

No Windows o binário vai para `%LOCALAPPDATA%\Programs\shit`, que é adicionado
ao `PATH` do usuário — abra um **novo** terminal depois de instalar.

> No Linux/macOS, se `~/.local/bin` não estiver no `PATH`, o instalador mostra
> a linha para adicionar no `~/.bashrc` ou `~/.zshrc`.

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
