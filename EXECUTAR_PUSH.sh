#!/bin/bash
# 🚀 SCRIPT FINAL PARA PUSH DO SPRINT 3

echo "🚀 SPRINT 3 - Push Final para GitHub"
echo "===================================="

# Verifica se estamos no diretório correto
if [ ! -f "README.md" ]; then
    echo "❌ Execute no diretório SPRINT3"
    exit 1
fi

echo "📋 Status do repositório:"
git status --short

echo ""
echo "🔑 CONFIGURAÇÃO NECESSÁRIA:"
echo "1. Vá em: https://github.com/settings/tokens"
echo "2. Clique em 'Generate new token (classic)'"
echo "3. Selecione 'repo' (Full control)"
echo "4. Copie o token gerado"
echo ""

# Configura credenciais
echo "⚙️ Configurando credenciais..."
git config --global credential.helper store
git config --global user.name "alansms"
git config --global user.email "alansms@github.com"

echo ""
echo "📤 EXECUTANDO PUSH..."
echo "Quando solicitado:"
echo "  Username: alansms"
echo "  Password: [cole o token aqui]"
echo ""

# Executa o push
git push -u origin main

# Verifica resultado
if [ $? -eq 0 ]; then
    echo ""
    echo "🎉 SUCESSO! Projeto enviado para GitHub!"
    echo "🔗 Acesse: https://github.com/alansms/EMBEDDED_IOT-EXPORTA-O_MODELO_VALIDA-O_ESP32-CAM"
    echo ""
    echo "📋 PRÓXIMOS PASSOS:"
    echo "1. Clone o repositório em outro local"
    echo "2. Execute: ./setup.sh"
    echo "3. Siga: QUICK_START.md"
    echo "4. Complete os testes de validação"
else
    echo ""
    echo "❌ FALHA NO PUSH"
    echo ""
    echo "💡 SOLUÇÕES ALTERNATIVAS:"
    echo "1. Use GitHub Desktop"
    echo "2. Faça upload manual via web"
    echo "3. Configure SSH keys"
    echo "4. Use GitHub CLI: gh auth login"
fi
