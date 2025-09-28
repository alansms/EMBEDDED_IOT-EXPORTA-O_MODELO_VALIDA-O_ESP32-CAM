#!/bin/bash
# Script para fazer push manual do projeto SPRINT 3

echo "🚀 Fazendo push do projeto SPRINT 3 para GitHub..."
echo "================================================="

# Verifica se estamos no diretório correto
if [ ! -f "README.md" ]; then
    echo "❌ Execute este script no diretório do projeto SPRINT3"
    exit 1
fi

echo "📋 Status atual do repositório:"
git status

echo ""
echo "🔑 Para fazer o push, você precisa de um Personal Access Token do GitHub:"
echo "1. Vá em GitHub → Settings → Developer settings → Personal access tokens"
echo "2. Gere um novo token com permissões de repositório"
echo "3. Use seu username e o token como senha quando solicitado"
echo ""

read -p "Pressione Enter para continuar com o push..."

# Tenta fazer o push
echo "📤 Fazendo push para o repositório..."
git push -u origin main

if [ $? -eq 0 ]; then
    echo ""
    echo "✅ Push realizado com sucesso!"
    echo "🔗 Repositório disponível em:"
    echo "   https://github.com/alansms/EMBEDDED_IOT-EXPORTA-O_MODELO_VALIDA-O_ESP32-CAM"
    echo ""
    echo "📋 Próximos passos:"
    echo "1. Clone o repositório em outro local para testar"
    echo "2. Execute ./setup.sh para configurar"
    echo "3. Siga as instruções do QUICK_START.md"
else
    echo ""
    echo "❌ Falha no push. Possíveis soluções:"
    echo "1. Verifique suas credenciais do GitHub"
    echo "2. Use um Personal Access Token"
    echo "3. Configure SSH keys"
    echo ""
    echo "💡 Alternativa: Faça upload manual via interface web do GitHub"
fi
