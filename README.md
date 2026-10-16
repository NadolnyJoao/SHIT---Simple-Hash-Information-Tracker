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

Requer GCC ou outro compilador C compatível.

```bash
gcc *.c -o shit
```

Execute o programa gerado pelo terminal.

## Filosofia

O SHIT prioriza simplicidade, portabilidade e clareza. Sua arquitetura baseada em snapshots e sua implementação em C oferecem uma introdução prática aos conceitos de controle de versão, sem depender de ferramentas externas de comparação de arquivos.
