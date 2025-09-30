#!/bin/bash
# SPRINT 3 - Script de Build para ESP32-CAM
# =========================================

echo "🚀 SPRINT 3 - Compilando firmware para ESP32-CAM"
echo "================================================="

# Verifica se o PlatformIO está instalado
if ! command -v pio &> /dev/null; then
    echo "❌ ERRO: PlatformIO não encontrado!"
    echo "Instale o PlatformIO: https://platformio.org/install"
    exit 1
fi

# Verifica se o model.h existe
if [ ! -f "src/model.h" ]; then
    echo "⚠️  AVISO: model.h não encontrado!"
    echo "Execute primeiro:"
    echo "  1. cd ../model"
    echo "  2. python export_tflite.py"
    echo "  3. python convert_to_c_array.py model_int8.tflite"
    echo "  4. cp model.h ../firmware/src/"
    echo ""
    read -p "Continuar mesmo assim? (y/N): " -n 1 -r
    echo
    if [[ ! $REPLY =~ ^[Yy]$ ]]; then
        exit 1
    fi
fi

echo "📦 Instalando dependências..."
pio lib install -e camera_test

echo "🔨 Compilando e fazendo upload para o ambiente camera_test..."
pio run -e camera_test --target upload

if [ $? -eq 0 ]; then
    echo "✅ Compilação e upload concluídos com sucesso!"
    echo ""
    echo "📊 Para monitorar a saída serial, use o comando:"
    echo "  pio device monitor -e camera_test"
else
    echo "❌ Falha na compilação ou no upload!"
    exit 1
fi
