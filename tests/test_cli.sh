#!/usr/bin/env bash
#
# Suite de testes de linha de comando (black-box) do SHIT.
#
# Cada teste executa o binario em um diretorio temporario e verifica o
# comportamento observavel (arquivos criados, conteudo, mensagens e o hash
# comparado com o `sha1sum` do sistema).
#
# Uso:
#   ./tests/test_cli.sh [caminho/para/o/binario]
#
# Se nenhum caminho for informado, usa ../build/shit relativo a este script.
#
set -u

# --- Localizacao do binario -------------------------------------------------
SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
ROOT_DIR="$(cd "$SCRIPT_DIR/.." && pwd)"
BIN="${1:-${SHIT_BIN:-$ROOT_DIR/build/shit}}"

if [ ! -x "$BIN" ]; then
  echo "erro: binario nao encontrado ou nao executavel: $BIN" >&2
  echo "      compile primeiro com: cmake -B build && cmake --build build" >&2
  exit 2
fi

# --- Cores (somente se a saida for um terminal) -----------------------------
if [ -t 1 ]; then
  GREEN=$'\033[32m'; RED=$'\033[31m'; BOLD=$'\033[1m'; RESET=$'\033[0m'
else
  GREEN=""; RED=""; BOLD=""; RESET=""
fi

PASS=0
FAIL=0
FAILED_NAMES=()

# check <nome> <esperado> <obtido>
check() {
  if [ "$2" = "$3" ]; then
    printf "  ${GREEN}PASS${RESET} %s\n" "$1"
    PASS=$((PASS + 1))
  else
    printf "  ${RED}FAIL${RESET} %s\n" "$1"
    printf "        esperado: [%s]\n" "$2"
    printf "        obtido:   [%s]\n" "$3"
    FAIL=$((FAIL + 1))
    FAILED_NAMES+=("$1")
  fi
}

section() { printf "\n${BOLD}%s${RESET}\n" "$1"; }

sha1_of() { sha1sum "$1" | cut -d' ' -f1; }

# --- Ambiente isolado -------------------------------------------------------
WORK="$(mktemp -d)"
trap 'rm -rf "$WORK"' EXIT
cd "$WORK" || exit 2

printf "${BOLD}SHIT - suite de testes${RESET}\n"
printf "binario: %s\n" "$BIN"

# ---------------------------------------------------------------------------
section "1. init"
# ---------------------------------------------------------------------------
"$BIN" init >/dev/null 2>&1
check "cria o diretorio .shit/"     "1" "$([ -d .shit ] && echo 1 || echo 0)"
check "cria .shit/objects/"         "1" "$([ -d .shit/objects ] && echo 1 || echo 0)"
check "cria .shit/commits/"         "1" "$([ -d .shit/commits ] && echo 1 || echo 0)"
check "cria o arquivo .shit/HEAD"   "1" "$([ -f .shit/HEAD ] && echo 1 || echo 0)"

OUT="$("$BIN" init 2>&1)"
check "init repetido avisa o usuario" "Repositorio SHIT ja inicializado em '.shit/'." "$OUT"

# ---------------------------------------------------------------------------
section "2. add: integridade do objeto"
# ---------------------------------------------------------------------------
printf 'conteudo de teste\nlinha 2\n' > a.txt
"$BIN" add a.txt >/dev/null 2>&1
HASH="$(sha1_of a.txt)"

check "objeto nomeado pelo hash sha1" "1" "$([ -f ".shit/objects/$HASH" ] && echo 1 || echo 0)"
check "conteudo do objeto == original" "0" "$(cmp -s ".shit/objects/$HASH" a.txt; echo $?)"
check "index no formato caminho<TAB>hash" "$(printf 'a.txt\t%s' "$HASH")" "$(cat .shit/index)"
check "mensagem de sucesso do add" "adicionado: a.txt ($HASH)" "$("$BIN" add a.txt 2>&1 | tail -1)"

# ---------------------------------------------------------------------------
section "3. add: casos de borda"
# ---------------------------------------------------------------------------
printf 'abc' > "meu trabalho.txt"
"$BIN" add "meu trabalho.txt" >/dev/null 2>&1
H2="$(sha1_of "meu trabalho.txt")"
check "nome com espacos: objeto correto" "1" "$([ -f ".shit/objects/$H2" ] && echo 1 || echo 0)"
check "nome com espacos: linha no index" "1" "$(grep -Fq "$(printf 'meu trabalho.txt\t%s' "$H2")" .shit/index && echo 1 || echo 0)"

: > vazio.txt
"$BIN" add vazio.txt >/dev/null 2>&1
check "arquivo vazio (sha1 conhecido)" "1" "$([ -f .shit/objects/da39a3ee5e6b4b0d3255bfef95601890afd80709 ] && echo 1 || echo 0)"

head -c 70 /dev/urandom > bin.dat
"$BIN" add bin.dat >/dev/null 2>&1
HB="$(sha1_of bin.dat)"
check "binario de 70 bytes: hash"     "1" "$([ -f ".shit/objects/$HB" ] && echo 1 || echo 0)"
check "binario de 70 bytes: integro"  "0" "$(cmp -s ".shit/objects/$HB" bin.dat; echo $?)"

head -c 1048576 /dev/urandom > big.dat
"$BIN" add big.dat >/dev/null 2>&1
HG="$(sha1_of big.dat)"
check "arquivo de 1 MB: hash"     "1" "$([ -f ".shit/objects/$HG" ] && echo 1 || echo 0)"
check "arquivo de 1 MB: integro"  "0" "$(cmp -s ".shit/objects/$HG" big.dat; echo $?)"

# ---------------------------------------------------------------------------
section "4. tratamento de erros"
# ---------------------------------------------------------------------------
ERR1="$("$BIN" add nao_existe.txt 2>&1)"
check "add de arquivo inexistente: mensagem" "1" "$(echo "$ERR1" | grep -Fq "nao foi possivel abrir ou ler" && echo 1 || echo 0)"

"$BIN" add >/dev/null 2>&1; RC_ADD=$?
check "add sem argumento: codigo de saida 1" "1" "$([ "$RC_ADD" -ne 0 ] && echo 1 || echo 0)"

"$BIN" comando_invalido >/dev/null 2>&1; RC_BAD=$?
check "comando desconhecido: codigo de saida 1" "1" "$([ "$RC_BAD" -ne 0 ] && echo 1 || echo 0)"

"$BIN" >/dev/null 2>&1; RC_NONE=$?
check "sem argumentos: mostra uso e sai 0" "0" "$RC_NONE"

# ---------------------------------------------------------------------------
section "5. bug conhecido (documentado, nao corrigido)"
# ---------------------------------------------------------------------------
"$BIN" init >/dev/null 2>&1
printf 'x\n' > dup.txt
"$BIN" add dup.txt >/dev/null 2>&1
BEFORE="$(wc -l < .shit/index)"
"$BIN" add dup.txt >/dev/null 2>&1
"$BIN" add dup.txt >/dev/null 2>&1
AFTER="$(wc -l < .shit/index)"
check "add repetido duplica linhas no index" "2" "$((AFTER - BEFORE))"

# ---------------------------------------------------------------------------
printf "\n${BOLD}Resultado:${RESET} ${GREEN}%d passaram${RESET}, ${RED}%d falharam${RESET}\n" "$PASS" "$FAIL"

if [ "$FAIL" -gt 0 ]; then
  printf "${RED}Falhas:${RESET}\n"
  for n in "${FAILED_NAMES[@]}"; do printf "  - %s\n" "$n"; done
  exit 1
fi

printf "${GREEN}Todos os testes passaram.${RESET}\n"
exit 0
