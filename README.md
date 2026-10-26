# SHIT — Simple Hash Information Tracker

O **SHIT** (Simple Hash Information Tracker) é um sistema minimalista de controle de versão escrito em **C**, desenvolvido com fins **educacionais**. Ele demonstra, na prática, os conceitos fundamentais por trás de sistemas de versionamento como Git, sem depender de algoritmos complexos ou ferramentas externas.

## Objetivo

Criar uma versão simplificada de um VCS, onde arquivos são identificados por seu hash SHA-1, armazenados em um banco de objetos e organizados por snapshots (commits), seguindo uma lógica enxuta e didática.

## Como funciona

O SHIT utiliza o diretório `.shit/` como repositório local:

- `objects/` — Armazena o conteúdo dos arquivos e commits, nomeados pelo seu hash SHA-1 (garantindo deduplicação natural).
- `index` — Área de preparação (staging). Arquivos listados aqui no formato `caminho<TAB>hash` são os que entrarão no próximo commit.
- `commits/` — Arquivos com os metadados e lista de arquivos de cada commit.
- `HEAD` — Aponta para o hash do commit mais recente. No primeiro commit, guarda `"break"`.

Os commits formam uma **cadeia encadeada**: cada um armazena o hash do commit anterior em `previous`, permitindo percorrer o histórico do mais recente ao mais antigo.

## Comandos

### Implementados

| Comando | Descrição |
| --- | --- |
| `shit init` | Inicializa um repositório criando a estrutura `.shit/` |
| `shit add <arquivo>` | Adiciona um arquivo individual à área de preparação (index) |
| `shit add .` | Adiciona ao index **apenas** arquivos novos ou modificados em relação ao último commit. Usa a lógica compartilhada com `status` para identificar mudanças corretamente |
| `shit register -m "mensagem"` | Cria um snapshot (commit) com a mensagem informada, lendo o que está no index |
| `shit status` | Compara a pasta de trabalho com o último commit, mostrando arquivos novos, modificados e removidos |
| `shit log` | Exibe o histórico de commits percorrendo a cadeia a partir do HEAD |
| `shit revert <commit-id>` | Restaura os arquivos da pasta de trabalho e atualiza o index para o estado exato do commit informado |

### Planejados

| Comando | Descrição |
| --- | --- |
| `shit log` (melhorias) | Aprimorar a visualização do histórico |
| `shit branch/checkout` | Suporte a múltiplas linhas de desenvolvimento |

## Arquitetura

O projeto foi organizado de forma modular em C:

| Arquivo | Responsabilidade |
| --- | --- |
| `main.c` | Interpretação dos comandos e fluxo principal |
| `init.c/h` | Inicialização do repositório |
| `add.c/h` | Adição de arquivos ao index e ao `add .` |
| `status.c/h` | Descoberta de mudanças (`Encontra_Mudancas`) e exibição do status |
| `commit.c/h` | Criação, leitura, log, revert e gerenciamento de commits |
| `sha1.c/h` | Cálculo do hash SHA-1 dos arquivos |
| `file_utils.c/h` | Utilitários para leitura de arquivos |

Uma das principais melhorias foi a **refatoração do `status`**: a lógica de varredura, hashing e comparação foi extraída para `Encontra_Mudancas()`, permitindo que `status` e `add .` compartilhem a **mesma fonte da verdade** sobre o que mudou.

## Compilação

Requer CMake 3.16+ e um compilador C (GCC, Clang ou MSVC).

```bash
cmake -B build
cmake --build build
```

Execute o binário gerado:

- Linux/macOS: `./build/shit`
- Windows: `build\shit.exe` (Ninja) ou `build\Debug\shit.exe` (Visual Studio)

## Instalação

Para usar o comando `shit` em qualquer diretório:

**Linux/macOS:**
```bash
./install.sh                  # instala em ~/.local/bin
./install.sh /usr/local/bin   # ou informe outra pasta
```

**Windows (PowerShell):**
```powershell
powershell -ExecutionPolicy Bypass -File install.ps1
```

## Testes

A suíte de testes fica em [`tests/`](tests/) e verifica o comportamento observável do CLI em diretórios temporários.

```bash
# Via CTest
ctest --test-dir build --output-on-failure

# Diretamente pelo script
./tests/test_cli.sh
```

## Conclusão

O SHIT cumpre seu propósito **didático** ao reproduzir, de forma simplificada, os mecanismos centrais de um sistema de controle de versão: indexação, armazenamento por hash, snapshots encadeados, comparação de arquivos e navegação pelo histórico.

Mais do que apenas "funcionar", o projeto foi desenvolvido com foco em **clareza e aprendizado**. A refatoração para reutilizar a lógica entre `status` e `add .`, a implementação da cadeia de commits para `log` e o `revert` para restaurar estados demonstram como conceitos teóricos de VCS se traduzem em código C prático.

**Simplicidade, entendimento e portabilidade** são os pilares do SHIT.
