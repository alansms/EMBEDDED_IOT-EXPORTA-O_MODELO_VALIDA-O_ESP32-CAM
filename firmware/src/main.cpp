/*
 * SPRINT 3 - ESP32-CAM com TensorFlow Lite Micro
 * ==============================================
 * 
 * Sistema de classificação de cartuchos HP em tempo real
 * usando ESP32-CAM e TensorFlow Lite Micro.
 * 
 * Funcionalidades:
 * - Captura de imagem com câmera OV2640
 * - Pré-processamento (redimensionamento + quantização INT8)
 * - Inferência com modelo embarcado
 * - Exibição de resultados no Serial Monitor
 * 
 * Hardware: ESP32-CAM (AI Thinker)
 * Câmera: OV2640
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
// INCLUSÕES DO TENSORFLOW LITE MICRO
// =============================================================================

#include "model.h"     // g_model, g_model_len (gerado pelo convert_to_c_array.py)
#include "labels.h"    // kCategoryLabels, kCategoryCount

// TensorFlow Lite Micro
#include "tensorflow/lite/micro/all_ops_resolver.h"
#include "tensorflow/lite/micro/micro_error_reporter.h"
#include "tensorflow/lite/micro/micro_interpreter.h"
#include "tensorflow/lite/schema/schema_generated.h"
#include "tensorflow/lite/version.h"

// =============================================================================
// CONFIGURAÇÕES DO MODELO
// =============================================================================

static const int kImgSize   = 96;    // Tamanho da imagem de entrada
static const int kChannels  = 1;       // Número de canais (1=grayscale, 3=RGB)
static const bool kUseGray = (kChannels == 1);

// Arena de memória para TensorFlow Lite Micro (ajuste conforme necessário)
constexpr int kArenaSize = 380 * 1024;  // ~380KB
alignas(16) static uint8_t tensor_arena[kArenaSize];

// =============================================================================
// VARIÁVEIS GLOBAIS
// =============================================================================

tflite::MicroErrorReporter micro_error_reporter;
tflite::ErrorReporter* error_reporter = &micro_error_reporter;
tflite::AllOpsResolver resolver;

const tflite::Model* model = nullptr;
tflite::MicroInterpreter* interpreter = nullptr;
TfLiteTensor* input = nullptr;
TfLiteTensor* output = nullptr;

// Buffer para pré-processamento (INT8)
static int8_t input_buffer[kImgSize * kImgSize * kChannels];

// Contadores para estatísticas
static unsigned long total_inferences = 0;
static unsigned long total_time_ms = 0;

// =============================================================================
// FUNÇÕES DE PRÉ-PROCESSAMENTO
// =============================================================================

/**
 * Redimensiona imagem RGB565 para grayscale INT8 usando interpolação nearest-neighbor
 */
static void resize_nn_rgb565_to_gray_int8(const uint16_t* src, int sw, int sh,
                                          int8_t* dst, int dw, int dh,
                                          float scale, int zero_point) {
  for (int y = 0; y < dh; ++y) {
    int sy = y * sh / dh;
    for (int x = 0; x < dw; ++x) {
      int sx = x * sw / dw;
      const uint16_t pix = src[sy * sw + sx];
      
      // Converte RGB565 para canais 8-bit
      uint8_t r = ((pix >> 11) & 0x1F) * 255 / 31;
      uint8_t g = ((pix >> 5)  & 0x3F) * 255 / 63;
      uint8_t b = ( pix        & 0x1F) * 255 / 31;
      
      // Converte para grayscale usando pesos padrão
      uint8_t gray = (uint8_t)(0.299f*r + 0.587f*g + 0.114f*b);
      
      // Aplica quantização INT8
      int val = (int)roundf(((float)gray - zero_point) / scale);
      if (val < -128) val = -128;
      if (val > 127)  val = 127;
      
      dst[y*dw + x] = (int8_t)val;
    }
  }
}

/**
 * Redimensiona imagem RGB565 para RGB INT8 usando interpolação nearest-neighbor
 */
static void resize_nn_rgb565_to_rgb_int8(const uint16_t* src, int sw, int sh,
                                         int8_t* dst, int dw, int dh,
                                         float scale, int zero_point) {
  for (int y = 0; y < dh; ++y) {
    int sy = y * sh / dh;
    for (int x = 0; x < dw; ++x) {
      int sx = x * sw / dw;
      const uint16_t pix = src[sy * sw + sx];
      
      // Converte RGB565 para canais 8-bit
      uint8_t r = ((pix >> 11) & 0x1F) * 255 / 31;
      uint8_t g = ((pix >> 5)  & 0x3F) * 255 / 63;
      uint8_t b = ( pix        & 0x1F) * 255 / 31;

      // Armazena os 3 canais consecutivamente
      int idx = (y*dw + x) * 3;
      
      // Aplica quantização INT8 para cada canal
      int vr = (int)roundf(((float)r - zero_point) / scale);
      int vg = (int)roundf(((float)g - zero_point) / scale);
      int vb = (int)roundf(((float)b - zero_point) / scale);
      
      // Clampa os valores para o range INT8
      if (vr < -128) vr = -128; if (vr > 127) vr = 127;
      if (vg < -128) vg = -128; if (vg > 127) vg = 127;
      if (vb < -128) vb = -128; if (vb > 127) vb = 127;
      
      dst[idx+0] = (int8_t)vr;
      dst[idx+1] = (int8_t)vg;
      dst[idx+2] = (int8_t)vb;
    }
  }
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
  config.pixel_format = PIXFORMAT_RGB565;  // Formato descomprimido para fácil processamento
  
  // Resolução QQVGA (160x120) - suficiente para downscale para 96x96
  config.frame_size   = FRAMESIZE_QQVGA;
  config.jpeg_quality = 12;
  config.fb_count     = 1;
  config.fb_location  = CAMERA_FB_IN_PSRAM;

  esp_err_t err = esp_camera_init(&config);
  return (err == ESP_OK);
}

// =============================================================================
// INICIALIZAÇÃO DO TENSORFLOW LITE MICRO
// =============================================================================

bool init_tflite() {
  // Carrega o modelo
  model = tflite::GetModel(g_model);
  if (model->version() != TFLITE_SCHEMA_VERSION) {
    Serial.printf("ERRO: Versão de schema incompatível. Esperado: %d, Encontrado: %d\n", 
                  TFLITE_SCHEMA_VERSION, model->version());
    return false;
  }
  
  Serial.println("✅ Modelo carregado com sucesso");

  // Cria o interpretador
  interpreter = new tflite::MicroInterpreter(model, resolver, tensor_arena, kArenaSize, error_reporter);
  
  // Aloca tensores
  if (interpreter->AllocateTensors() != kTfLiteOk) {
    Serial.println("ERRO: Falha ao alocar tensores");
    return false;
  }
  
  Serial.println("✅ Tensores alocados com sucesso");

  // Obtém referências para input e output
  input  = interpreter->input(0);
  output = interpreter->output(0);

  // Exibe informações dos tensores
  Serial.printf("Input: type=%d, dims=[", input->type);
  for (int i = 0; i < input->dims->size; i++) {
    Serial.printf("%d", input->dims->data[i]);
    if (i < input->dims->size - 1) Serial.print(" ");
  }
  Serial.println("]");
  
  Serial.printf("Output: type=%d, classes=%d\n", output->type, output->dims->data[output->dims->size-1]);
  
  return true;
}

// =============================================================================
// FUNÇÃO DE INFERÊNCIA
// =============================================================================

void run_inference() {
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

  // Obtém parâmetros de quantização do input tensor
  float scale = 1.0f;
  int zero_point = 128;
  
  if (input->quantization.type == kTfLiteAffineQuantization) {
    auto *q = (TfLiteAffineQuantization*)input->quantization.params;
    if (q->scale && q->scale->size > 0) scale = q->scale->data[0];
    if (q->zero_point && q->zero_point->size > 0) zero_point = q->zero_point->data[0];
  }

  // Pré-processamento da imagem
  if (kUseGray) {
    resize_nn_rgb565_to_gray_int8((const uint16_t*)fb->buf, fb->width, fb->height,
                                  input_buffer, kImgSize, kImgSize, scale, zero_point);
  } else {
    resize_nn_rgb565_to_rgb_int8((const uint16_t*)fb->buf, fb->width, fb->height,
                                 input_buffer, kImgSize, kImgSize, scale, zero_point);
  }

  // Libera o frame buffer
  esp_camera_fb_return(fb);

  // Copia dados processados para o tensor de entrada
  memcpy(input->data.int8, input_buffer, kImgSize * kImgSize * kChannels);

  // Executa inferência
  unsigned long t0 = millis();
  TfLiteStatus invoke_status = interpreter->Invoke();
  unsigned long t1 = millis();
  
  if (invoke_status != kTfLiteOk) {
    Serial.println("ERRO: Falha na inferência");
    return;
  }

  // Processa resultados
  int classes = output->dims->data[output->dims->size - 1];
  int best_idx = -1;
  int best_val = -9999;
  
  Serial.print("Scores: ");
  for (int i = 0; i < classes; i++) {
    int8_t score = output->data.int8[i];
    Serial.printf("[%s]=%d ", kCategoryLabels[i], (int)score);
    if (score > best_val) {
      best_val = score;
      best_idx = i;
    }
  }
  Serial.println();

  // Exibe resultado final
  Serial.printf("🎯 Predição: %s (score=%d) | ⏱️ Latência=%lums\n",
                kCategoryLabels[best_idx], best_val, (t1 - t0));

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
  delay(1200);
  
  Serial.println("\n" + String("=").repeat(60));
  Serial.println("🚀 SPRINT 3 - ESP32-CAM + TensorFlow Lite Micro");
  Serial.println("📷 Sistema de Classificação de Cartuchos HP");
  Serial.println(String("=").repeat(60));

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

  // Inicializa TensorFlow Lite Micro
  Serial.println("🧠 Inicializando TensorFlow Lite Micro...");
  if (!init_tflite()) {
    Serial.println("❌ ERRO: Falha ao inicializar TensorFlow Lite");
    while (true) {
      delay(1000);
      Serial.println("Tentando novamente...");
    }
  }
  Serial.println("✅ TensorFlow Lite Micro inicializado com sucesso");

  // Exibe informações do sistema
  Serial.println("\n📋 Configurações do Sistema:");
  Serial.printf("   - Tamanho da imagem: %dx%d\n", kImgSize, kImgSize);
  Serial.printf("   - Canais: %d (%s)\n", kChannels, kUseGray ? "Grayscale" : "RGB");
  Serial.printf("   - Arena de memória: %d KB\n", kArenaSize / 1024);
  Serial.printf("   - Classes: %d\n", kCategoryCount);
  
  for (int i = 0; i < kCategoryCount; i++) {
    Serial.printf("     [%d] %s\n", i, kCategoryLabels[i]);
  }

  Serial.println("\n🎬 Sistema pronto! Iniciando classificação...");
  Serial.println("📝 Pressione RESET para reiniciar");
  Serial.println(String("-").repeat(60));
}

void loop() {
  run_inference();
  delay(300);  // ~3 FPS para demonstração
}
