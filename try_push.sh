#!/bin/bash

echo "🚀 Tentando fazer push para o GitHub..."
echo "📁 Diretório: $(pwd)"
echo ""

# Verificar se estamos no repositório correto
if [ ! -d ".git" ]; then
    echo "❌ Não é um repositório git!"
    exit 1
fi

echo "✅ Repositório git detectado"
echo ""

# Verificar status
echo "📊 Status atual:"
git status --short
echo ""

# Verificar commits locais
echo "📝 Commits locais:"
git log --oneline -3
echo ""

# Verificar remote
echo "🌐 Remote configurado:"
git remote -v
echo ""

# Tentar push
echo "🔐 Tentando fazer push..."
if git push origin main; then
    echo "✅ Push realizado com sucesso!"
    echo "🔗 Repositório: https://github.com/alansms/EMBEDDED_IOT-EXPORTA-O_MODELO_VALIDA-O_ESP32-CAM"
else
    echo "❌ Falha no push via terminal"
    echo ""
    echo "💡 Soluções alternativas:"
    echo "1. Use o GitHub Desktop (já aberto)"
    echo "2. Configure um Personal Access Token"
    echo "3. Verifique as permissões do repositório"
    echo ""
    echo "🔗 Repositório: https://github.com/alansms/EMBEDDED_IOT-EXPORTA-O_MODELO_VALIDA-O_ESP32-CAM"
    echo ""
    echo "📱 GitHub Desktop está rodando - use a interface gráfica para fazer push"
fi
