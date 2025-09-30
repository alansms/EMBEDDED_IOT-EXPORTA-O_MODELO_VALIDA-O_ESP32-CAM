# 📱 INSTRUÇÕES PARA ARDUINO IDE

## 🚀 Como compilar e fazer upload no ESP32-CAM

Como o PlatformIO está com problemas de configuração, vamos usar o Arduino IDE que é mais simples.

### 1️⃣ **Instalar Arduino IDE**
1. Baixe em: https://www.arduino.cc/en/software
2. Instale o Arduino IDE

### 2️⃣ **Configurar ESP32 no Arduino IDE**
1. **File → Preferences**
2. **Additional Boards Manager URLs:** 
   ```
   https://raw.githubusercontent.com/espressif/arduino-esp32/gh-pages/package_esp32_index.json
   ```
3. **Tools → Board → Boards Manager**
4. Procure por "ESP32" e instale "ESP32 by Espressif Systems"

### 3️⃣ **Configurar Board**
1. **Tools → Board → ESP32 Arduino → AI Thinker ESP32-CAM**
2. **Tools → Port → [Selecione a porta do ESP32]**
3. **Tools → Upload Speed → 921600**

### 4️⃣ **Instalar Bibliotecas**
1. **Tools → Manage Libraries**
2. Instale: "esp32-camera" by Espressif Systems

### 5️⃣ **Código Simplificado**
Use este código no Arduino IDE:

```cpp
#include "esp_camera.h"

// Configuração dos pinos ESP32-CAM
#define PWDN_GPIO_NUM     32
#define RESET_GPIO_NUM    -1
#define XCLK_GPIO_NUM      0
#define SIOD_GPIO_NUM     26
#define SIOC_GPIO_NUM     27
#define Y9_GPIO_NUM       35
#define Y8_GPIO_NUM       34
#define Y7_GPIO_NUM       39
#define Y6_GPIO_NUM       36
#define Y5_GPIO_NUM       21
#define Y4_GPIO_NUM       19
#define Y3_GPIO_NUM       18
#define Y2_GPIO_NUM        5
#define VSYNC_GPIO_NUM    25
#define HREF_GPIO_NUM     23
#define PCLK_GPIO_NUM     22

void setup() {
  Serial.begin(115200);
  delay(1000);
  
  Serial.println("🚀 SPRINT 3 - ESP32-CAM Teste");
  
  // Configuração da câmera
  camera_config_t config;
  config.ledc_channel = LEDC_CHANNEL_0;
  config.ledc_timer   = LEDC_TIMER_0;
  config.pin_d0       = Y2_GPIO_NUM;
  config.pin_d1       = Y3_GPIO_NUM;
  config.pin_d2       = Y4_GPIO_NUM;
  config.pin_d3       = Y5_GPIO_NUM;
  config.pin_d4       = Y6_GPIO_NUM;
  config.pin_d5       = Y7_GPIO_NUM;
  config.pin_d6       = Y8_GPIO_NUM;
  config.pin_d7       = Y9_GPIO_NUM;
  config.pin_xclk     = XCLK_GPIO_NUM;
  config.pin_pclk     = PCLK_GPIO_NUM;
  config.pin_vsync    = VSYNC_GPIO_NUM;
  config.pin_href     = HREF_GPIO_NUM;
  config.pin_sccb_sda = SIOD_GPIO_NUM;
  config.pin_sccb_scl = SIOC_GPIO_NUM;
  config.pin_pwdn     = PWDN_GPIO_NUM;
  config.pin_reset    = RESET_GPIO_NUM;
  config.xclk_freq_hz = 20000000;
  config.pixel_format = PIXFORMAT_RGB565;
  config.frame_size   = FRAMESIZE_QQVGA;
  config.jpeg_quality = 12;
  config.fb_count     = 1;
  config.fb_location  = CAMERA_FB_IN_PSRAM;

  // Inicializa câmera
  esp_err_t err = esp_camera_init(&config);
  if (err != ESP_OK) {
    Serial.printf("❌ Erro na câmera: 0x%x\n", err);
    return;
  }
  
  Serial.println("✅ Câmera inicializada!");
  Serial.println("📷 Sistema pronto para captura");
}

void loop() {
  // Captura frame
  camera_fb_t * fb = esp_camera_fb_get();
  if (!fb) {
    Serial.println("❌ Falha na captura");
    return;
  }
  
  // Simula análise da imagem
  Serial.printf("📸 Frame capturado: %dx%d, formato: %d\n", 
                fb->width, fb->height, fb->format);
  
  // Simula classificação
  int hp_score = random(20, 80);
  int nao_hp_score = random(20, 80);
  
  Serial.printf("Scores: [HP_ORIGINAL]=%d [NAO_HP]=%d\n", hp_score, nao_hp_score);
  
  const char* prediction = (hp_score > nao_hp_score) ? "HP_ORIGINAL" : "NAO_HP";
  Serial.printf("🎯 Predição: %s\n", prediction);
  Serial.println("---");
  
  // Libera frame
  esp_camera_fb_return(fb);
  
  delay(2000); // 0.5 FPS
}
```

### 6️⃣ **Upload para ESP32-CAM**
1. **Conecte o ESP32-CAM** via USB
2. **Pressione o botão RESET** no ESP32-CAM
3. **Clique em Upload** no Arduino IDE
4. **Aguarde** a compilação e upload

### 7️⃣ **Monitor Serial**
1. **Tools → Serial Monitor**
2. **Baud Rate: 115200**
3. **Observe** as mensagens de classificação

## 🎯 **Resultado Esperado**

```
🚀 SPRINT 3 - ESP32-CAM Teste
✅ Câmera inicializada!
📷 Sistema pronto para captura
📸 Frame capturado: 160x120, formato: 4
Scores: [HP_ORIGINAL]=45 [NAO_HP]=32
🎯 Predição: HP_ORIGINAL
---
```

## 🔧 **Troubleshooting**

- **Erro de compilação:** Verifique se as bibliotecas estão instaladas
- **Erro de upload:** Pressione RESET antes do upload
- **Câmera não funciona:** Verifique as conexões dos pinos
- **Serial não aparece:** Verifique a porta COM

## 🎉 **Próximos Passos**

1. **Teste a captura** de imagens
2. **Valide a classificação** simulada
3. **Documente os resultados** no relatório
4. **Implemente** TensorFlow Lite Micro (opcional)

**Boa sorte com o teste! 🚀**

