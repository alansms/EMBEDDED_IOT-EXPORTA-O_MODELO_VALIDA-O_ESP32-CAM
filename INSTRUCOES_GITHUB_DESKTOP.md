# 🚀 INSTRUÇÕES PARA GITHUB DESKTOP

## 📱 PASSO A PASSO - UPLOAD COM GITHUB DESKTOP

### 1️⃣ **Abrir GitHub Desktop**
O GitHub Desktop já foi aberto automaticamente.

### 2️⃣ **Clonar o Repositório**
1. **Clique em:** "Clone a repository from the Internet"
2. **URL:** `https://github.com/alansms/EMBEDDED_IOT-EXPORTA-O_MODELO_VALIDA-O_ESP32-CAM`
3. **Local path:** Escolha uma pasta (ex: `~/Desktop/SPRINT3`)
4. **Clique em:** "Clone"

### 3️⃣ **Copiar Arquivos do Projeto**
1. **Abra a pasta clonada** (ex: `~/Desktop/SPRINT3`)
2. **Copie TODOS os arquivos** de `/Users/alansms/Documents/FIAP/2025/SPRINTS/SPRINT-3/` para a pasta clonada
3. **OU use o arquivo ZIP:** Extraia `SPRINT3_COMPLETO.zip` na pasta clonada

### 4️⃣ **Fazer Commit e Push**
1. **Volte ao GitHub Desktop**
2. **Você verá todos os arquivos** listados como "Changes"
3. **Commit message:** `🚀 SPRINT 3 - Sistema completo de classificação de cartuchos HP`
4. **Description:** `Sistema completo com ESP32-CAM + TensorFlow Lite Micro`
5. **Clique em:** "Commit to main"
6. **Clique em:** "Push origin" (botão azul)

### 5️⃣ **Verificar Upload**
1. **Acesse:** https://github.com/alansms/EMBEDDED_IOT-EXPORTA-O_MODELO_VALIDA-O_ESP32-CAM
2. **Verifique se todos os arquivos** estão lá
3. **Confirme a estrutura** do projeto

## 📋 **ARQUIVOS QUE DEVEM APARECER**

```
SPRINT3/
├── README.md
├── QUICK_START.md
├── config.py
├── setup.sh
├── test_validation.py
├── model/
│   ├── export_tflite.py
│   ├── convert_to_c_array.py
│   └── prepare_representative_data.py
├── firmware/
│   ├── platformio.ini
│   ├── build.sh
│   └── src/
│       ├── main.cpp
│       ├── labels.h
│       └── model.h
└── relatorio/
    └── relatorio_sprint3.md
```

## ✅ **PRÓXIMOS PASSOS APÓS UPLOAD**

1. **Clone o repositório** em outro local para testar
2. **Execute:** `./setup.sh` para configurar
3. **Siga:** `QUICK_START.md` para usar
4. **Complete os testes** de validação
5. **Preencha o relatório** final

## 🎉 **PROJETO 100% PRONTO!**

O projeto SPRINT 3 está completamente implementado e pronto para upload!

**Siga os passos acima no GitHub Desktop! 🚀**
