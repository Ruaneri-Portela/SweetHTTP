#!/bin/bash

# Verifique se o Git está instalado
if command -v git >/dev/null 2>&1; then
    # Obtenha o hash e a tag atual
    GIT_HASH=$(git rev-parse --short HEAD)
    GIT_TAG=$(git describe --tags --exact-match 2>/dev/null || echo "untagged")
else
    # Defina valores padrão se o Git não estiver instalado
    GIT_HASH="NULL"
    GIT_TAG="NULL"
fi

# Crie ou sobrescreva o arquivo SweetHTTP_version.h
cat <<EOL > src/SweetHTTP_version.h
#ifndef SWEETHTTP_VERSION_H
#define SWEETHTTP_VERSION_H

#define SWEETHTTP_VERSION_HASH "${GIT_HASH}"
#define SWEETHTTP_VERSION_TAG "${GIT_TAG}"

#endif // SWEETHTTP_VERSION_H
EOL