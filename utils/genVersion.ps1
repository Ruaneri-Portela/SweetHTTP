# Caminho para o arquivo de versão
$versionFile = "src\SweetHTTP_version.h"

# Verifique se o Git está instalado
if (Get-Command git -ErrorAction SilentlyContinue) {
    # Obtenha o hash e a tag atual do Git
    $GIT_HASH = git rev-parse --short HEAD
    $GIT_TAG = git describe --tags --exact-match 2>$null
    if (-not $GIT_TAG) {
        $GIT_TAG = "untagged"
    }
} else {
    # Defina valores padrão se o Git não estiver instalado
    $GIT_HASH = "Null"
    $GIT_TAG = "Null"
}

# Crie ou sobrescreva o arquivo SweetHTTP_version.h
@"
#ifndef SWEETHTTP_VERSION_H
#define SWEETHTTP_VERSION_H

#define SWEETHTTP_VERSION_HASH "$GIT_HASH"
#define SWEETHTTP_VERSION_TAG "$GIT_TAG"

#endif // SWEETHTTP_VERSION_H
"@ | Set-Content -Path $versionFile
