<#
.SYNOPSIS
  Instalador do SHIT para Windows.

.DESCRIPTION
  Compila o projeto com CMake, instala shit.exe em
  %LOCALAPPDATA%\Programs\shit e adiciona essa pasta ao PATH do usuario
  (sem duplicar), para que "shit" funcione em qualquer terminal.

.EXAMPLE
  powershell -ExecutionPolicy Bypass -File install.ps1
#>
$ErrorActionPreference = "Stop"

$Root      = Split-Path -Parent $MyInvocation.MyCommand.Path
$BuildDir  = Join-Path $Root "build"
$Prefix    = Join-Path $env:LOCALAPPDATA "Programs\shit"

Write-Host "SHIT - instalador (Windows)"
Write-Host "projeto: $Root"

# 1) Compilar
Write-Host "==> Compilando..."
cmake -S $Root -B $BuildDir | Out-Null
cmake --build $BuildDir | Out-Null

# 2) Instalar shit.exe
Write-Host "==> Instalando em $Prefix..."
New-Item -ItemType Directory -Force -Path $Prefix | Out-Null
cmake --install $BuildDir --prefix $Prefix | Out-Null

# 3) Adicionar ao PATH do usuario (sem duplicar)
$userPath = [Environment]::GetEnvironmentVariable("Path", "User")
if ($null -eq $userPath) { $userPath = "" }

if (($userPath -split ";") -notcontains $Prefix) {
  $newPath = if ($userPath.Trim() -eq "") { $Prefix } else { "$userPath;$Prefix" }
  [Environment]::SetEnvironmentVariable("Path", $newPath, "User")
  Write-Host "==> Pasta adicionada ao PATH do usuario."
  Write-Host "    Abra um NOVO terminal e rode: shit init"
} else {
  Write-Host "==> Pasta ja estava no PATH."
  Write-Host "    Rode: shit init"
}
