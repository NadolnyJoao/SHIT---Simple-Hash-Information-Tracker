# Testes

Suíte de testes de linha de comando (black-box) do SHIT. Cada teste executa o
binário em um diretório temporário e verifica o comportamento observável:
arquivos criados, integridade do conteúdo e o hash comparado com o `sha1sum`
do sistema.

## Requisitos

- Um binário `shit` compilado (veja o [README](../README.md)).
- `sha1sum` (coreutils) disponível no `PATH`.

## Como executar

Via CTest (recomendado, a partir da raiz do projeto):

```bash
cmake -B build
cmake --build build
ctest --test-dir build --output-on-failure
```

Diretamente pelo script:

```bash
./tests/test_cli.sh              # usa build/shit automaticamente
./tests/test_cli.sh /caminho/do/shit   # ou informe o binário
```

## O que é coberto

| Grupo | Testes |
| --- | --- |
| `init` | criação de `.shit/`, `objects/`, `commits/`, `HEAD` e idempotência |
| `add` | objeto nomeado pelo SHA-1, integridade do conteúdo, formato do índice (`caminho<TAB>hash`) e mensagem de sucesso |
| Casos de borda | nomes com espaços, arquivo vazio, binário de 70 bytes (cruza o bloco interno do SHA-1) e arquivo de 1 MB |
| Erros | arquivo inexistente, `add` sem argumento, comando desconhecido e uso sem argumentos |
| Bug conhecido | `add` repetido duplica linhas no `.shit/index` (documentado, ainda não corrigido) |

## Resultado esperado

Todas as verificações devem passar:

```
Resultado: 21 passaram, 0 falharam
Todos os testes passaram.
```

> O teste do grupo "bug conhecido" valida o **comportamento atual**, ou seja,
> confirma a duplicação de linhas no índice. Ele deve ser atualizado quando o
> filtro de duplicatas for implementado no `commit`.

## Estrutura

```
tests/
├── test_cli.sh   # script de teste (bash)
└── README.md     # este arquivo
```

O script cria todo o ambiente de teste em um diretório temporário e o remove
ao final, portanto não deixa resíduos no repositório.
