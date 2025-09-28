# 🚀 SPRINT 3 - Guia de Início Rápido

## ⚡ Execução em 5 Passos

### 1️⃣ Setup Inicial
```bash
# Execute o script de setup
./setup.sh
```

### 2️⃣ Preparar Modelo
```bash
# Coloque seu modelo da Sprint 1 em:
cp /caminho/para/seu/modelo.keras model/model_best.keras

# Prepare dataset representativo (opcional - cria exemplos)
cd model
python3 prepare_representative_data.py --create-samples --samples 30
```

### 3️⃣ Exportar para TensorFlow Lite
```bash
cd model
python3 export_tflite.py
python3 convert_to_c_array.py model_int8.tflite
cp model.h ../firmware/src/
```

### 4️⃣ Compilar e Upload
```bash
cd firmware
./build.sh
pio run --target upload
```

### 5️⃣ Testar e Validar
```bash
# Monitorar serial
pio device monitor

# Analisar resultados
cd ..
python3 test_validation.py
```

## 📋 Checklist de Entrega

- [ ] Modelo `model_int8.tflite` exportado
- [ ] Firmware compilado e funcionando
- [ ] 10 imagens testadas (5 HP + 5 não-HP)
- [ ] Relatório preenchido com resultados
- [ ] Screenshots do Serial Monitor
- [ ] Análise de performance concluída

## 🆘 Problemas Comuns

| Problema | Solução |
|----------|---------|
| Modelo muito grande | Reduza arquitetura ou use quantização mais agressiva |
| Erro de compilação | Verifique se PlatformIO está instalado |
| Câmera não funciona | Confirme pinos e conexões do ESP32-CAM |
| Latência alta | Reduza resolução ou otimize modelo |

## 📞 Suporte

Consulte o `README.md` completo para detalhes técnicos.
