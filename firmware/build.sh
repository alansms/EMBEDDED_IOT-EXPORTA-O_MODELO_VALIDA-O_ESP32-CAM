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
pio lib install

echo "🔨 Compilando firmware..."
pio run

if [ $? -eq 0 ]; then
    echo "✅ Compilação concluída com sucesso!"
    echo ""
    echo "📤 Para fazer upload:"
    echo "  pio run --target upload"
    echo ""
    echo "📊 Para monitorar serial:"
    echo "  pio device monitor"
else
    echo "❌ Falha na compilação!"
    exit 1
fi
