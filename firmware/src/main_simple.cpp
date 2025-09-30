/*
 * SPRINT 3 - ESP32-CAM Simulação de Classificação
 * ================================================
 * 
 * Versão simplificada para teste sem TensorFlow Lite Micro.
 * Simula a classificação de cartuchos HP.
 * 
 * Autor: Equipe SPRINT 3
 * Data: 2025
 */

#include <Arduino.h>
#include "esp_camera.h"

// =============================================================================
// CONFIGURAÇÕES DA CÂMERA (AI THINKER ESP32-CAM)
// =============================================================================

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

// =============================================================================
// CONFIGURAÇÕES DO SISTEMA
// =============================================================================

static const int kImgSize = 96;
static const int kChannels = 1;

// Labels das classes
static const char* kCategoryLabels[] = {
  "HP_ORIGINAL",
  "NAO_HP"
};

// Contadores para estatísticas
static unsigned long total_inferences = 0;
static unsigned long total_time_ms = 0;

// Função auxiliar para imprimir caracteres repetidos
void printRepeatChar(char c, int n) {
  for (int i = 0; i < n; i++) Serial.print(c);
  Serial.println();
}

// =============================================================================
// FUNÇÕES DE SIMULAÇÃO
// =============================================================================

/**
 * Simula análise de imagem para classificação
 */
void simulate_image_analysis(camera_fb_t* fb) {
  // Simula análise baseada em características da imagem
  int width = fb->width;
  int height = fb->height;
  
  // Calcula algumas estatísticas básicas da imagem
  uint16_t* pixels = (uint16_t*)fb->buf;
  int total_pixels = width * height;
  
  long sum_r = 0, sum_g = 0, sum_b = 0;
  int bright_pixels = 0;
  
  for (int i = 0; i < total_pixels; i++) {
    uint16_t pixel = pixels[i];
    
    // Extrai componentes RGB565
    uint8_t r = ((pixel >> 11) & 0x1F) * 255 / 31;
    uint8_t g = ((pixel >> 5)  & 0x3F) * 255 / 63;
    uint8_t b = ( pixel        & 0x1F) * 255 / 31;
    
    sum_r += r;
    sum_g += g;
    sum_b += b;
    
    if (r > 200 || g > 200 || b > 200) {
      bright_pixels++;
    }
  }
  
  // Calcula médias
  float avg_r = (float)sum_r / total_pixels;
  float avg_g = (float)sum_g / total_pixels;
  float avg_b = (float)sum_b / total_pixels;
  float brightness_ratio = (float)bright_pixels / total_pixels;
  
  // Simula classificação baseada em características
  int hp_score = 0;
  int nao_hp_score = 0;
  
  // Critérios simples de classificação
  if (avg_r > 100 && avg_g > 100) {  // Tons mais quentes
    hp_score += 30;
  }
  
  if (brightness_ratio > 0.3) {  // Imagem mais brilhante
    hp_score += 20;
  }
  
  if (avg_b > avg_r && avg_b > avg_g) {  // Predominância azul
    nao_hp_score += 25;
  }
  
  // Adiciona aleatoriedade para simular variação
  hp_score += random(-20, 30);
  nao_hp_score += random(-20, 30);
  
  // Garante que os scores sejam diferentes
  if (abs(hp_score - nao_hp_score) < 10) {
    hp_score += random(10, 20);
  }
  
  // Exibe resultados
  Serial.printf("Scores: [%s]=%d [%s]=%d\n", 
                kCategoryLabels[0], hp_score, kCategoryLabels[1], nao_hp_score);
  
  // Determina classe vencedora
  const char* prediction = (hp_score > nao_hp_score) ? kCategoryLabels[0] : kCategoryLabels[1];
  int confidence = abs(hp_score - nao_hp_score);
  
  Serial.printf("🎯 Predição: %s (confiança=%d)\n", prediction, confidence);
  
  // Exibe características da imagem
  Serial.printf("📊 Características: R=%.1f G=%.1f B=%.1f Brilho=%.2f\n", 
                avg_r, avg_g, avg_b, brightness_ratio);
}

// =============================================================================
// INICIALIZAÇÃO DA CÂMERA
// =============================================================================

bool init_camera() {
  camera_config_t config;
  
  // Configuração dos pinos
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
  
  // Configurações de clock e formato
  config.xclk_freq_hz = 20000000;
  config.pixel_format = PIXFORMAT_RGB565;
  
  // Resolução QQVGA (160x120)
  config.frame_size   = FRAMESIZE_QQVGA;
  config.jpeg_quality = 12;
  config.fb_count     = 1;
  config.fb_location  = CAMERA_FB_IN_PSRAM;

  esp_err_t err = esp_camera_init(&config);
  return (err == ESP_OK);
}

// =============================================================================
// FUNÇÃO DE INFERÊNCIA SIMULADA
// =============================================================================

void run_simulation() {
  // Captura frame da câmera
  camera_fb_t* fb = esp_camera_fb_get();
  if (!fb) {
    Serial.println("ERRO: Falha ao capturar frame");
    return;
  }

  // Verifica formato da imagem
  if (fb->format != PIXFORMAT_RGB565) {
    Serial.println("ERRO: Formato de pixel inesperado");
    esp_camera_fb_return(fb);
    return;
  }

  // Simula tempo de processamento
  unsigned long t0 = millis();
  delay(50 + random(0, 100));  // Simula latência de 50-150ms
  unsigned long t1 = millis();

  // Simula análise da imagem
  simulate_image_analysis(fb);

  // Libera o frame buffer
  esp_camera_fb_return(fb);

  // Exibe latência
  Serial.printf("⏱️ Latência=%lums\n", (t1 - t0));

  // Atualiza estatísticas
  total_inferences++;
  total_time_ms += (t1 - t0);
  
  if (total_inferences % 10 == 0) {
    float avg_time = (float)total_time_ms / total_inferences;
    Serial.printf("📊 Estatísticas: %lu inferências, tempo médio: %.1fms\n", 
                  total_inferences, avg_time);
  }
}

// =============================================================================
// SETUP E LOOP PRINCIPAIS
// =============================================================================

void setup() {
  Serial.begin(115200);
  delay(1000);

  Serial.println("\n");
  printRepeatChar('=', 60);
  Serial.println("🚀 SPRINT 3 - ESP32-CAM Classificação de Cartuchos HP");
  Serial.println("📷 Sistema de Classificação com Modelo TFLite (Simulação)");
  printRepeatChar('=', 60);

  // Inicializa câmera
  Serial.println("📷 Inicializando câmera...");
  if (!init_camera()) {
    Serial.println("❌ ERRO: Falha ao inicializar câmera");
    while (true) {
      delay(1000);
      Serial.println("Tentando novamente...");
    }
  }
  Serial.println("✅ Câmera inicializada com sucesso");

  // Exibe informações do sistema
  Serial.println("\n📋 Configurações do Sistema:");
  Serial.printf("   - Tamanho da imagem: %dx%d\n", kImgSize, kImgSize);
  Serial.printf("   - Canais: %d (Grayscale)\n", kChannels);
  Serial.printf("   - Modo: SIMULAÇÃO (sem TensorFlow Lite)\n");
  Serial.printf("   - Classes: 2\n");
  
  for (int i = 0; i < 2; i++) {
    Serial.printf("     [%d] %s\n", i, kCategoryLabels[i]);
  }

  Serial.println("\n🎬 Sistema pronto! Iniciando classificação...");
  Serial.println("📝 Aponte a câmera para cartuchos HP e não-HP para teste");
  Serial.println("📝 Pressione RESET para reiniciar");
  printRepeatChar('-', 60);
}

void loop() {
  run_simulation();
  delay(1000);  // 1 FPS para demonstração
}
