#!/bin/bash
# Script para compilar e instalar Escrity (Linux)
# Uso: ./install.sh [ruta]
BUILD_DIR="build"
INSTALL_PREFIX="/usr/local"

if [ "$#" -ge 1 ]; then
    PROJECT_DIR="$1"
else
    PROJECT_DIR="."
fi

if [ ! -d "$PROJECT_DIR" ]; then
    echo "Error: El directorio $PROJECT_DIR no existe."
    exit 1
fi
cd "$PROJECT_DIR" || exit
echo "Verificando requisitos..."
if ! command -v cmake &> /dev/null; then
    echo "Error: CMake no está instalado. Por favor instálalo con:"
    echo "sudo apt install cmake  # Debian/Ubuntu"
    echo "sudo dnf install cmake # Fedora"
    echo "sudo pacman -S cmake   # Arch"
    exit 1
fi
if ! command -v make &> /dev/null; then
    echo "Error: make no está instalado. Por favor instálalo con:"
    echo "sudo apt install build-essential  # Debian/Ubuntu"
    echo "sudo dnf groupinstall 'Development Tools' # Fedora"
    echo "sudo pacman -S base-devel   # Arch"
    exit 1
fi
echo "Configurando el proyecto en $BUILD_DIR..."
if [ -d "$BUILD_DIR" ]; then
    echo "Advertencia: El directorio $BUILD_DIR ya existe. Se esta vaciando..."
    rm -rf "$BUILD_DIR"
fi

mkdir -p "$BUILD_DIR"
cd "$BUILD_DIR" || exit

cmake -DCMAKE_BUILD_TYPE=Release -DCMAKE_INSTALL_PREFIX="$INSTALL_PREFIX" ..

if [ $? -ne 0 ]; then
    echo "Error en la configuración con CMake"
    exit 1
fi

echo "Compilando Escrity."
make -j$(nproc)

if [ $? -ne 0 ]; then
    echo "Ocurrio un error al compilar Escrity."
    exit 1
fi

echo "Se esta instalando en $INSTALL_PREFIX..."
if [[ "$INSTALL_PREFIX" == /usr* ]]; then
    if [ "$(id -u)" -ne 0 ]; then
        echo "Se requieren privilegios de superusuario para instalar en $INSTALL_PREFIX"
        sudo make install
    else
        make install
    fi
else
    make install
fi

if [ $? -ne 0 ]; then
    echo "Error durante la instalacion"
    exit 1
fi
echo "¡Se instalo correctamente Escrity!"
echo "Escrity se instalo en $INSTALL_PREFIX"
