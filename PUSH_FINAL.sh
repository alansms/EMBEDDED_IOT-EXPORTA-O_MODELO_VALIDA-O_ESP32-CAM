#!/bin/bash
# 🚀 SCRIPT FINAL PARA PUSH DO SPRINT 3

echo "🚀 SPRINT 3 - Push Final para GitHub"
echo "===================================="

# Verifica se estamos no diretório correto
if [ ! -f "README.md" ]; then
    echo "❌ Execute no diretório SPRINT3"
    exit 1
fi

echo "📋 Status atual:"
git log --oneline -5

echo ""
echo "🔑 SOLUÇÕES PARA PUSH:"
echo ""
echo "1️⃣ GITHUB CLI (Recomendado):"
echo "   brew install gh"
echo "   gh auth login"
echo "   git push -u origin main"
echo ""
echo "2️⃣ PERSONAL ACCESS TOKEN:"
echo "   Vá em: https://github.com/settings/tokens"
echo "   Gere um novo token com permissões 'repo'"
echo "   Execute: git push -u origin main"
echo "   Username: alansms"
echo "   Password: [cole o token]"
echo ""
echo "3️⃣ UPLOAD MANUAL:"
echo "   Use o arquivo: ../SPRINT3_COMPLETO.zip"
echo "   Faça upload via interface web do GitHub"
echo ""

read -p "Pressione Enter para tentar push automático..."

# Tenta fazer o push
echo "📤 Tentando push automático..."
git push -u origin main

if [ $? -eq 0 ]; then
    echo ""
    echo "🎉 SUCESSO! Projeto enviado para GitHub!"
    echo "🔗 Acesse: https://github.com/alansms/EMBEDDED_IOT-EXPORTA-O_MODELO_VALIDA-O_ESP32-CAM"
else
    echo ""
    echo "❌ FALHA NO PUSH AUTOMÁTICO"
    echo ""
    echo "💡 SOLUÇÕES:"
    echo "1. Instale GitHub CLI: brew install gh"
    echo "2. Gere novo token em: https://github.com/settings/tokens"
    echo "3. Use upload manual com: ../SPRINT3_COMPLETO.zip"
    echo ""
    echo "📁 Arquivo ZIP criado em:"
    echo "   /Users/alansms/Documents/FIAP/2025/SPRINTS/SPRINT3_COMPLETO.zip"
fi
