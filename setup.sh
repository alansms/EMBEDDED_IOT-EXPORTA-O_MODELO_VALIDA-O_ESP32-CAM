#!/bin/bash
# SPRINT 3 - Script de Setup Completo
# ====================================

echo "🚀 SPRINT 3 - Setup do Projeto de Classificação de Cartuchos HP"
echo "==============================================================="

# Cores para output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
NC='\033[0m' # No Color

# Função para imprimir com cores
print_status() {
    echo -e "${BLUE}[INFO]${NC} $1"
}

print_success() {
    echo -e "${GREEN}[SUCCESS]${NC} $1"
}

print_warning() {
    echo -e "${YELLOW}[WARNING]${NC} $1"
}

print_error() {
    echo -e "${RED}[ERROR]${NC} $1"
}

# Verifica se está no diretório correto
if [ ! -f "README.md" ]; then
    print_error "Execute este script no diretório raiz do projeto SPRINT3"
    exit 1
fi

print_status "Verificando dependências..."

# Verifica Python
if ! command -v python3 &> /dev/null; then
    print_error "Python 3 não encontrado! Instale Python 3.8+ primeiro."
    exit 1
fi

# Verifica PlatformIO
if ! command -v pio &> /dev/null; then
    print_warning "PlatformIO não encontrado!"
    echo "Instale o PlatformIO: https://platformio.org/install"
    echo "Ou use: pip install platformio"
    read -p "Continuar mesmo assim? (y/N): " -n 1 -r
    echo
    if [[ ! $REPLY =~ ^[Yy]$ ]]; then
        exit 1
    fi
fi

print_status "Criando estrutura de diretórios..."

# Cria diretórios se não existirem
mkdir -p model/representative_data/hp_original
mkdir -p model/representative_data/nao_hp
mkdir -p firmware/src
mkdir -p relatorio

print_success "Estrutura de diretórios criada"

print_status "Verificando arquivos necessários..."

# Verifica se os scripts Python existem
if [ ! -f "model/export_tflite.py" ]; then
    print_error "Script export_tflite.py não encontrado!"
    exit 1
fi

if [ ! -f "model/convert_to_c_array.py" ]; then
    print_error "Script convert_to_c_array.py não encontrado!"
    exit 1
fi

print_success "Scripts Python encontrados"

print_status "Instalando dependências Python..."

# Instala dependências Python
cd model
pip3 install tensorflow pillow numpy

if [ $? -eq 0 ]; then
    print_success "Dependências Python instaladas"
else
    print_warning "Falha ao instalar algumas dependências Python"
fi

cd ..

print_status "Verificando configuração do firmware..."

# Verifica se os arquivos do firmware existem
if [ ! -f "firmware/src/main.cpp" ]; then
    print_error "Arquivo main.cpp não encontrado!"
    exit 1
fi

if [ ! -f "firmware/platformio.ini" ]; then
    print_error "Arquivo platformio.ini não encontrado!"
    exit 1
fi

print_success "Arquivos do firmware encontrados"

print_status "Tornando scripts executáveis..."
chmod +x firmware/build.sh
chmod +x model/export_tflite.py
chmod +x model/convert_to_c_array.py

print_success "Scripts tornados executáveis"

echo ""
echo "==============================================================="
print_success "Setup concluído com sucesso!"
echo "==============================================================="
echo ""
echo "📋 Próximos passos:"
echo ""
echo "1. 📁 Coloque seu modelo da Sprint 1 em:"
echo "   model/model_best.keras"
echo ""
echo "2. 🖼️  Adicione imagens representativas em:"
echo "   model/representative_data/hp_original/"
echo "   model/representative_data/nao_hp/"
echo "   (50-100 imagens pequenas de cada classe)"
echo ""
echo "3. 🔄 Exporte o modelo:"
echo "   cd model"
echo "   python3 export_tflite.py"
echo "   python3 convert_to_c_array.py model_int8.tflite"
echo "   cp model.h ../firmware/src/"
echo ""
echo "4. 🔨 Compile o firmware:"
echo "   cd firmware"
echo "   ./build.sh"
echo ""
echo "5. 📤 Faça upload para ESP32-CAM:"
echo "   pio run --target upload"
echo ""
echo "6. 📊 Monitore o serial:"
echo "   pio device monitor"
echo ""
echo "📖 Para mais detalhes, consulte o README.md"
echo ""
print_success "Boa sorte com seu projeto! 🚀"
