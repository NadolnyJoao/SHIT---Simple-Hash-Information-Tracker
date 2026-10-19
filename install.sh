#!/usr/bin/env bash
#
# Instalador do SHIT para Linux e macOS.
#
# Compila o projeto, instala o binario em ~/.local/bin (por padrao) e avisa
# caso essa pasta nao esteja no PATH.
#
# Uso:
#   ./install.sh                 # instala em ~/.local/bin
#   ./install.sh /usr/local/bin  # ou informe outra pasta
#
set -euo pipefail

ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
PREFIX="${1:-$HOME/.local/bin}"
BUILD_DIR="$ROOT/build"

echo "SHIT - instalador (Linux/macOS)"
echo "projeto: $ROOT"

# 1) Compilar
echo "==> Compilando..."
cmake -S "$ROOT" -B "$BUILD_DIR" >/dev/null
cmake --build "$BUILD_DIR" >/dev/null

# 2) Instalar o binario no prefixo escolhido
echo "==> Instalando em $PREFIX..."
mkdir -p "$PREFIX"
cmake --install "$BUILD_DIR" --prefix "$PREFIX" >/dev/null

# 3) Avisar se o prefixo nao esta no PATH
case ":$PATH:" in
  *":$PREFIX:"*)
    echo "==> Pronto! Rode: shit init"
    ;;
  *)
    echo "==> Binario instalado em $PREFIX/shit"
    echo "    AVISO: '$PREFIX' nao esta no seu PATH."
    echo "    Adicione esta linha ao seu ~/.bashrc ou ~/.zshrc:"
    echo "        export PATH=\"$PREFIX:\$PATH\""
    ;;
esac

# 4) Dica extra: link em ~/.local/bin quando instalou em outro lugar
if [ "$PREFIX" != "$HOME/.local/bin" ]; then
  if mkdir -p "$HOME/.local/bin" 2>/dev/null && \
     ln -sf "$PREFIX/shit" "$HOME/.local/bin/shit" 2>/dev/null; then
    echo "    Link criado tambem em ~/.local/bin/shit (se estiver no PATH)."
  fi
fi
