#!/bin/bash
# Script de compilação e upload com saída detalhada

echo "=== Iniciando compilação e upload do teste de câmera ==="
echo "Diretório atual: $(pwd)"

# Limpa compilações anteriores
echo "Limpando projeto..."
platformio run --target clean

# Compilação com saída verbosa
echo "Compilando o projeto..."
platformio run -e camera_test -v

# Se a compilação for bem-sucedida, tenta o upload
if [ $? -eq 0 ]; then
  echo "Compilação bem-sucedida. Fazendo upload..."
  platformio run -e camera_test --target upload -v

  if [ $? -eq 0 ]; then
    echo "Upload concluído com sucesso!"
    echo "Para monitorar a saída serial, use: platformio device monitor"
  else
    echo "Falha no upload. Verifique se o dispositivo está conectado corretamente."
  fi
else
  echo "Falha na compilação. Verifique os erros acima."
fi
