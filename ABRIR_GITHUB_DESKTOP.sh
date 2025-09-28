#!/bin/bash
# 🚀 Script para abrir GitHub Desktop com o projeto SPRINT 3

echo "🚀 SPRINT 3 - Abrindo GitHub Desktop"
echo "===================================="

# Verifica se estamos no diretório correto
if [ ! -f "README.md" ]; then
    echo "❌ Execute no diretório SPRINT3"
    exit 1
fi

echo "📋 Status do repositório:"
git status --short

echo ""
echo "📱 Abrindo GitHub Desktop..."
echo ""

# Abre o GitHub Desktop
open -a "GitHub Desktop"

echo "✅ GitHub Desktop aberto!"
echo ""
echo "📋 PRÓXIMOS PASSOS NO GITHUB DESKTOP:"
echo ""
echo "1️⃣ Se não aparecer o repositório automaticamente:"
echo "   - Clique em 'Add an Existing Repository from your Hard Drive'"
echo "   - Selecione esta pasta: $(pwd)"
echo ""
echo "2️⃣ Se aparecer o repositório:"
echo "   - Você verá todos os arquivos listados"
echo "   - Commit message: '🚀 SPRINT 3 - Sistema completo de classificação de cartuchos HP'"
echo "   - Clique em 'Commit to main'"
echo "   - Clique em 'Push origin'"
echo ""
echo "3️⃣ Verificar upload:"
echo "   - Acesse: https://github.com/alansms/EMBEDDED_IOT-EXPORTA-O_MODELO_VALIDA-O_ESP32-CAM"
echo "   - Confirme se todos os arquivos estão lá"
echo ""
echo "🎉 Projeto pronto para upload! 🚀"
