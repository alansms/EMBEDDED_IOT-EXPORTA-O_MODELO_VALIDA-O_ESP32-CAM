# SPRINT 3 - Exportação do Modelo e Validação no ESP32-CAM

Este projeto implementa um sistema de classificação de cartuchos HP em tempo real usando ESP32-CAM e TensorFlow Lite Micro.

## 📋 Visão Geral

O sistema captura imagens com a câmera OV2640, processa em tempo real e classifica se o cartucho é HP original ou não-HP/falsificado usando um modelo de deep learning embarcado.

## 🏗️ Estrutura do Projeto

```
SPRINT3/
├── model/                          # Scripts de exportação do modelo
│   ├── export_tflite.py            # Conversão para TensorFlow Lite INT8
│   ├── convert_to_c_array.py      # Conversão para array C
│   ├── representative_data/        # Dataset para calibração
│   │   ├── hp_original/           # Imagens de cartuchos HP
│   │   └── nao_hp/                # Imagens de cartuchos não-HP
│   └── model_best.keras           # Modelo da Sprint 1 (coloque aqui)
├── firmware/                       # Código do ESP32-CAM
│   ├── src/
│   │   ├── main.cpp               # Código principal
│   │   ├── labels.h               # Labels das classes
│   │   └── model.h                # Modelo em array C (gerado)
│   ├── platformio.ini             # Configuração PlatformIO
│   └── build.sh                   # Script de build
└── relatorio/
    └── relatorio_sprint3.md       # Template do relatório
```

## 🚀 Instruções de Uso

### Pré-requisitos

1. **Python 3.8+** com TensorFlow 2.8+
2. **PlatformIO** instalado
3. **ESP32-CAM** (AI Thinker)
4. **Modelo da Sprint 1** (arquivo .keras ou .h5)

### Passo 1: Preparar o Modelo

1. Coloque o modelo da Sprint 1 em `model/model_best.keras`
2. Prepare o dataset representativo:
   ```bash
   # Crie as pastas se não existirem
   mkdir -p model/representative_data/hp_original
   mkdir -p model/representative_data/nao_hp
   
   # Adicione ~50-100 imagens de cada classe
   # (imagens pequenas, 32-64px são suficientes)
   ```

### Passo 2: Exportar para TensorFlow Lite

```bash
cd model

# Instalar dependências Python
pip install tensorflow pillow numpy

# Exportar modelo para .tflite
python export_tflite.py

# Converter para array C
python convert_to_c_array.py model_int8.tflite

# Copiar model.h para o firmware
cp model.h ../firmware/src/
```

### Passo 3: Compilar e Upload

```bash
cd firmware

# Compilar firmware
./build.sh

# Upload para ESP32-CAM
pio run --target upload

# Monitorar serial
pio device monitor
```

## 📊 Resultados Esperados

### Serial Monitor - Inicialização
```
============================================================
🚀 SPRINT 3 - ESP32-CAM + TensorFlow Lite Micro
📷 Sistema de Classificação de Cartuchos HP
============================================================
✅ Câmera inicializada com sucesso
✅ TensorFlow Lite Micro inicializado com sucesso
🎬 Sistema pronto! Iniciando classificação...
```

### Serial Monitor - Classificações
```
Scores: [HP_ORIGINAL]=45 [NAO_HP]=-12 
🎯 Predição: HP_ORIGINAL (score=45) | ⏱️ Latência=156ms
```

## 🔧 Configurações

### Parâmetros do Modelo
Edite em `model/export_tflite.py`:
```python
IMG_SIZE = 96        # Tamanho da imagem de entrada
CHANNELS = 1         # 1=grayscale, 3=RGB
```

### Configurações do ESP32-CAM
Edite em `firmware/src/main.cpp`:
```cpp
static const int kImgSize = 96;     // Deve coincidir com o modelo
static const int kChannels = 1;     // Deve coincidir com o modelo
constexpr int kArenaSize = 380 * 1024;  // Arena de memória
```

## 🧪 Testes de Validação

### Dataset de Teste
Prepare 10 imagens novas:
- 5 cartuchos HP originais
- 5 cartuchos não-HP ou falsificados

### Processo de Teste
1. Capture cada imagem com o ESP32-CAM
2. Registre a predição e latência
3. Compare com a classificação real
4. Calcule acurácia, precisão e recall

### Exemplo de Tabela de Resultados
| Imagem | Tipo Real | Predição | Correto | Latência (ms) |
|--------|-----------|----------|---------|---------------|
| 1      | HP        | HP       | ✅      | 156           |
| 2      | HP        | Não-HP   | ❌      | 142           |
| ...    | ...       | ...      | ...     | ...           |

## 📝 Relatório

Use o template em `relatorio/relatorio_sprint3.md` para documentar:
- Processo de exportação
- Resultados dos testes
- Screenshots do Serial Monitor
- Análise de performance
- Conclusões

## 🐛 Solução de Problemas

### Erro: "Modelo muito grande"
- Reduza a arquitetura do modelo
- Use quantização mais agressiva
- Considere MobileNetV1/V2 nano

### Erro: "Falha ao alocar tensores"
- Aumente `kArenaSize` em `main.cpp`
- Verifique se o modelo está correto
- Confirme compatibilidade das operações

### Erro: "Câmera não inicializa"
- Verifique conexões dos pinos
- Confirme que é ESP32-CAM (AI Thinker)
- Teste com exemplo básico da câmera

### Performance baixa
- Reduza resolução de captura
- Otimize pré-processamento
- Considere modelo mais simples

## 📚 Referências

- [TensorFlow Lite Micro](https://www.tensorflow.org/lite/microcontrollers)
- [ESP32-CAM Guide](https://randomnerdtutorials.com/esp32-cam-ai-thinker-pinout/)
- [PlatformIO ESP32](https://docs.platformio.org/en/latest/platforms/espressif32.html)

## 👥 Equipe

**SPRINT 3 - Classificação de Cartuchos HP**  
**FIAP - 2025**

---

**Boa sorte com seu projeto! 🚀**
