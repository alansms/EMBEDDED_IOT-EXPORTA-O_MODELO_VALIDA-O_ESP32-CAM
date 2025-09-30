#!/bin/bash

echo "🚀 Tentando fazer push para o GitHub..."
echo "📁 Diretório atual: $(pwd)"
echo "🔍 Status do git:"
git status --short

echo ""
echo "📊 Commits locais:"
git log --oneline -3

echo ""
echo "🌐 Remote configurado:"
git remote -v

echo ""
echo "🔐 Tentando fazer push..."
git push origin main

if [ $? -eq 0 ]; then
    echo "✅ Push realizado com sucesso!"
else
    echo "❌ Falha no push. Tentando alternativas..."
    echo ""
    echo "💡 Soluções possíveis:"
    echo "1. Abra o GitHub Desktop e faça login"
    echo "2. Use o GitHub Desktop para fazer push"
    echo "3. Configure um Personal Access Token"
    echo "4. Verifique as permissões do repositório"
    echo ""
    echo "🔗 Repositório: https://github.com/alansms/EMBEDDED_IOT-EXPORTA-O_MODELO_VALIDA-O_ESP32-CAM"
fi
