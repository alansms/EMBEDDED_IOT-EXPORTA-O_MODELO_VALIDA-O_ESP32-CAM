# SPRINT 3 - Sistema de Classificação de Cartuchos HP com ESP32-CAM

Sistema completo de classificação de cartuchos HP em tempo real usando ESP32-CAM e TensorFlow Lite Micro.

## 🚀 Funcionalidades

- **Captura de imagem** com câmera OV2640
- **Pré-processamento** otimizado (RGB565 → INT8)
- **Inferência** com TensorFlow Lite Micro
- **Classificação** em tempo real
- **Sistema de validação** com métricas de performance

## 📁 Estrutura do Projeto

```
SPRINT3/
├── README.md                    # Este arquivo
├── QUICK_START.md              # Guia de início rápido
├── config.py                   # Configurações centralizadas
├── setup.sh                    # Script de setup automático
├── test_validation.py          # Análise de resultados
├── model/                      # Scripts de exportação
│   ├── export_tflite.py        # Conversão para TFLite INT8
│   ├── convert_to_c_array.py   # Conversão para array C
│   └── prepare_representative_data.py
├── firmware/                   # Código ESP32-CAM
│   ├── platformio.ini         # Configuração PlatformIO
│   ├── build.sh               # Script de build
│   └── src/
│       ├── main.cpp            # Código principal
│       ├── labels.h            # Labels das classes
│       └── model.h             # Modelo em array C
└── relatorio/
    └── relatorio_sprint3.md    # Template do relatório
```

## ⚡ Início Rápido

### 1. Setup
```bash
./setup.sh
```

### 2. Exportar Modelo
```bash
cd model
python3 export_tflite.py
python3 convert_to_c_array.py model_int8.tflite
cp model.h ../firmware/src/
```

### 3. Compilar Firmware
```bash
cd firmware
./build.sh
pio run --target upload
```

### 4. Testar
```bash
pio device monitor
```

## 🔧 Configurações

- **Tamanho da imagem:** 96x96 pixels
- **Canais:** 1 (grayscale) ou 3 (RGB)
- **Quantização:** INT8 completa
- **Arena de memória:** 380KB
- **Performance:** ~3 FPS

## 📊 Validação

Use o script de validação para analisar resultados:
```bash
python3 test_validation.py
```

## 📝 Relatório

Preencha o template em `relatorio/relatorio_sprint3.md` com:
- Resultados dos testes
- Screenshots do Serial Monitor
- Análise de performance
- Conclusões

## 🎯 Classes

- **HP_ORIGINAL:** Cartucho HP original
- **NAO_HP:** Cartucho não-HP ou falsificado

## 📚 Dependências

- Python 3.8+
- TensorFlow 2.8+
- PlatformIO
- ESP32-CAM (AI Thinker)

## 🚀 Uso

1. Coloque seu modelo da Sprint 1 em `model/model_best.keras`
2. Prepare dataset representativo em `model/representative_data/`
3. Execute os scripts de exportação
4. Compile e faça upload do firmware
5. Teste com imagens reais
6. Analise os resultados

---

**SPRINT 3 - FIAP 2025** 🎓