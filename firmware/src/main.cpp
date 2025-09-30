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
#include <WiFi.h>
#include <WebServer.h>
#include <ArduinoJson.h>

// No XIAO ESP32S3, Serial0 é a porta USB
#define Serial Serial0

// Configurações WiFi
const char* ssid = "SEU_WIFI_SSID";
const char* password = "SUA_SENHA_WIFI";

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

// Servidor web
WebServer server(80);

// Dados globais para a interface web
struct ClassificationData {
  String prediction;
  float confidence;
  float hp_score;
  float nao_hp_score;
  float avg_r, avg_g, avg_b;
  unsigned long total_inferences;
  float avg_time;
} current_data;

// Função auxiliar para imprimir caracteres repetidos
void printRepeatChar(char c, int n) {
  for (int i = 0; i < n; i++) Serial.print(c);
  Serial.println();
}

// =============================================================================
// FUNÇÕES DO SERVIDOR WEB
// =============================================================================

String getIndexHTML() {
  return R"rawliteral(
<!DOCTYPE html>
<html>
<head>
    <title>ESP32-CAM HP Detector</title>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1">
    <style>
        body { font-family: Arial; margin: 20px; background: #f0f0f0; }
        .container { max-width: 600px; margin: 0 auto; background: white; padding: 20px; border-radius: 10px; }
        h1 { color: #0066cc; text-align: center; }
        .camera-box { border: 2px solid #ccc; padding: 10px; margin: 20px 0; text-align: center; }
        #camera { max-width: 100%; height: auto; }
        .data { background: #f9f9f9; padding: 15px; margin: 10px 0; border-radius: 5px; }
        .result { font-size: 20px; font-weight: bold; padding: 15px; margin: 10px 0; border-radius: 5px; text-align: center; }
        .hp { background: #d4edda; color: #155724; border: 2px solid #28a745; }
        .nao-hp { background: #f8d7da; color: #721c24; border: 2px solid #dc3545; }
        button { background: #0066cc; color: white; border: none; padding: 10px 15px; margin: 5px; border-radius: 5px; cursor: pointer; }
        button:hover { background: #0052a3; }
        .stats { display: flex; justify-content: space-between; margin: 10px 0; }
        .stat { text-align: center; }
        .value { font-size: 18px; font-weight: bold; color: #0066cc; }
    </style>
</head>
<body>
    <div class="container">
        <h1>🚀 ESP32-CAM Classificador HP</h1>

        <div class="camera-box">
            <h3>📷 Câmera ao Vivo</h3>
            <img id="camera" src="/capture.jpg" alt="Loading camera..." />
            <br><br>
            <button onclick="updateCamera()">🔄 Atualizar Câmera</button>
            <button onclick="testConnection()">🧪 Testar Conexão</button>
        </div>

        <div id="result" class="result">🔍 Aguardando análise...</div>

        <div class="data">
            <h3>📊 Resultados</h3>
            <div class="stats">
                <div class="stat">
                    <div>HP Original</div>
                    <div class="value" id="hp">0%</div>
                </div>
                <div class="stat">
                    <div>Não HP</div>
                    <div class="value" id="nao-hp">0%</div>
                </div>
                <div class="stat">
                    <div>Confiança</div>
                    <div class="value" id="conf">0%</div>
                </div>
            </div>

            <p><strong>RGB:</strong> R=<span id="r">-</span>, G=<span id="g">-</span>, B=<span id="b">-</span></p>
            <p><strong>Análises:</strong> <span id="total">0</span> | <strong>Tempo:</strong> <span id="tempo">0ms</span></p>

            <button onclick="updateData()">📊 Atualizar Dados</button>
            <button onclick="autoUpdate()">⚡ Auto-Update</button>
        </div>

        <div class="data">
            <h3>🔧 Diagnóstico</h3>
            <p id="status">Carregando...</p>
            <button onclick="diagnosis()">🩺 Diagnóstico</button>
        </div>
    </div>

    <script>
        let autoUpdateActive = false;
        let updateInterval;

        function updateCamera() {
            console.log('Atualizando câmera...');
            const img = document.getElementById('camera');
            const timestamp = Date.now();
            img.src = '/capture.jpg?t=' + timestamp;
        }

        function updateData() {
            console.log('Buscando dados...');
            fetch('/status')
                .then(response => response.json())
                .then(data => {
                    document.getElementById('hp').textContent = data.scores.hp_original.toFixed(1) + '%';
                    document.getElementById('nao-hp').textContent = data.scores.nao_hp.toFixed(1) + '%';
                    document.getElementById('conf').textContent = data.confidence.toFixed(1) + '%';
                    document.getElementById('r').textContent = data.features.r.toFixed(0);
                    document.getElementById('g').textContent = data.features.g.toFixed(0);
                    document.getElementById('b').textContent = data.features.b.toFixed(0);
                    document.getElementById('total').textContent = data.stats.total_inferences;
                    document.getElementById('tempo').textContent = data.stats.avg_time.toFixed(1) + 'ms';

                    const result = document.getElementById('result');
                    if (data.prediction === 'HP_ORIGINAL') {
                        result.className = 'result hp';
                        result.textContent = '✅ HP ORIGINAL (' + data.confidence.toFixed(1) + '%)';
                    } else {
                        result.className = 'result nao-hp';
                        result.textContent = '❌ NÃO HP (' + data.confidence.toFixed(1) + '%)';
                    }
                })
                .catch(error => {
                    console.error('Erro:', error);
                    document.getElementById('status').textContent = 'Erro na comunicação: ' + error.message;
                });
        }

        function testConnection() {
            console.log('Testando conexão...');
            fetch('/test')
                .then(response => response.text())
                .then(data => {
                    document.getElementById('status').textContent = 'Teste OK: ' + data;
                })
                .catch(error => {
                    document.getElementById('status').textContent = 'Erro no teste: ' + error.message;
                });
        }

        function diagnosis() {
            console.log('Executando diagnóstico...');
            fetch('/health')
                .then(response => response.text())
                .then(data => {
                    document.getElementById('status').textContent = 'Diagnóstico: ' + data;
                })
                .catch(error => {
                    document.getElementById('status').textContent = 'Erro no diagnóstico: ' + error.message;
                });
        }

        function autoUpdate() {
            if (autoUpdateActive) {
                clearInterval(updateInterval);
                autoUpdateActive = false;
                document.getElementById('status').textContent = 'Auto-update DESATIVADO';
            } else {
                updateInterval = setInterval(() => {
                    updateCamera();
                    updateData();
                }, 3000);
                autoUpdateActive = true;
                document.getElementById('status').textContent = 'Auto-update ATIVADO (3s)';
            }
        }

        // Inicialização
        window.onload = function() {
            console.log('Página carregada');
            testConnection();
            setTimeout(() => {
                updateCamera();
                updateData();
            }, 1000);
        };

        // Tratamento de erros de imagem
        document.getElementById('camera').onerror = function() {
            console.log('Erro ao carregar imagem');
            this.alt = 'Erro na câmera - clique em Atualizar Câmera';
            document.getElementById('status').textContent = 'Erro: Imagem da câmera não carregou';
        };

        document.getElementById('camera').onload = function() {
            console.log('Imagem carregada com sucesso');
            document.getElementById('status').textContent = 'Câmera funcionando';
        };
    </script>
</body>
</html>
)rawliteral";
}

void handleRoot() {
  server.send(200, "text/html", getIndexHTML());
}

void handleCapture() {
  camera_fb_t* fb = esp_camera_fb_get();
  if (!fb) {
    server.send(500, "text/plain", "Erro ao capturar imagem");
    return;
  }
  
  server.send_P(200, "image/jpeg", (const char*)fb->buf, fb->len);
  esp_camera_fb_return(fb);
}

void handleStatus() {
  JsonDocument doc;
  
  doc["prediction"] = current_data.prediction;
  doc["confidence"] = current_data.confidence;
  doc["scores"]["hp_original"] = current_data.hp_score;
  doc["scores"]["nao_hp"] = current_data.nao_hp_score;
  doc["features"]["r"] = current_data.avg_r;
  doc["features"]["g"] = current_data.avg_g;
  doc["features"]["b"] = current_data.avg_b;
  doc["stats"]["total_inferences"] = current_data.total_inferences;
  doc["stats"]["avg_time"] = current_data.avg_time;
  
  String response;
  serializeJson(doc, response);
  server.send(200, "application/json", response);
}

void handleTest() {
  server.send(200, "text/plain", "ESP32-CAM funcionando! " + String(millis()));
}

void handleHealth() {
  String health = "Sistema OK - Câmera: ";
  health += (esp_camera_fb_get() != NULL) ? "OK" : "ERRO";
  health += " | WiFi: ";
  health += (WiFi.status() == WL_CONNECTED) ? "Conectado" : "Desconectado";
  health += " | Uptime: " + String(millis() / 1000) + "s";
  
  server.send(200, "text/plain", health);
}

String getIndexHTML() {
  return R"rawliteral(
<!DOCTYPE html>
<html>
<head>
    <title>ESP32-CAM HP Detector</title>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1">
    <style>
        body { font-family: Arial; margin: 20px; background: #f0f0f0; }
        .container { max-width: 600px; margin: 0 auto; background: white; padding: 20px; border-radius: 10px; }
        h1 { color: #0066cc; text-align: center; }
        .camera-box { border: 2px solid #ccc; padding: 10px; margin: 20px 0; text-align: center; }
        #camera { max-width: 100%; height: auto; }
        .data { background: #f9f9f9; padding: 15px; margin: 10px 0; border-radius: 5px; }
        .result { font-size: 20px; font-weight: bold; padding: 15px; margin: 10px 0; border-radius: 5px; text-align: center; }
        .hp { background: #d4edda; color: #155724; border: 2px solid #28a745; }
        .nao-hp { background: #f8d7da; color: #721c24; border: 2px solid #dc3545; }
        button { background: #0066cc; color: white; border: none; padding: 10px 15px; margin: 5px; border-radius: 5px; cursor: pointer; }
        button:hover { background: #0052a3; }
        .stats { display: flex; justify-content: space-between; margin: 10px 0; }
        .stat { text-align: center; }
        .value { font-size: 18px; font-weight: bold; color: #0066cc; }
    </style>
</head>
<body>
    <div class="container">
        <h1>🚀 ESP32-CAM Classificador HP</h1>

        <div class="camera-box">
            <h3>📷 Câmera ao Vivo</h3>
            <img id="camera" src="/capture.jpg" alt="Loading camera..." />
            <br><br>
            <button onclick="updateCamera()">🔄 Atualizar Câmera</button>
            <button onclick="testConnection()">🧪 Testar Conexão</button>
        </div>

        <div id="result" class="result">🔍 Aguardando análise...</div>

        <div class="data">
            <h3>📊 Resultados</h3>
            <div class="stats">
                <div class="stat">
                    <div>HP Original</div>
                    <div class="value" id="hp">0%</div>
                </div>
                <div class="stat">
                    <div>Não HP</div>
                    <div class="value" id="nao-hp">0%</div>
                </div>
                <div class="stat">
                    <div>Confiança</div>
                    <div class="value" id="conf">0%</div>
                </div>
            </div>

            <p><strong>RGB:</strong> R=<span id="r">-</span>, G=<span id="g">-</span>, B=<span id="b">-</span></p>
            <p><strong>Análises:</strong> <span id="total">0</span> | <strong>Tempo:</strong> <span id="tempo">0ms</span></p>

            <button onclick="updateData()">📊 Atualizar Dados</button>
            <button onclick="autoUpdate()">⚡ Auto-Update</button>
        </div>

        <div class="data">
            <h3>🔧 Diagnóstico</h3>
            <p id="status">Carregando...</p>
            <button onclick="diagnosis()">🩺 Diagnóstico</button>
        </div>
    </div>

    <script>
        let autoUpdateActive = false;
        let updateInterval;

        function updateCamera() {
            console.log('Atualizando câmera...');
            const img = document.getElementById('camera');
            const timestamp = Date.now();
            img.src = '/capture.jpg?t=' + timestamp;
        }

        function updateData() {
            console.log('Buscando dados...');
            fetch('/status')
                .then(response => response.json())
                .then(data => {
                    document.getElementById('hp').textContent = data.scores.hp_original.toFixed(1) + '%';
                    document.getElementById('nao-hp').textContent = data.scores.nao_hp.toFixed(1) + '%';
                    document.getElementById('conf').textContent = data.confidence.toFixed(1) + '%';
                    document.getElementById('r').textContent = data.features.r.toFixed(0);
                    document.getElementById('g').textContent = data.features.g.toFixed(0);
                    document.getElementById('b').textContent = data.features.b.toFixed(0);
                    document.getElementById('total').textContent = data.stats.total_inferences;
                    document.getElementById('tempo').textContent = data.stats.avg_time.toFixed(1) + 'ms';

                    const result = document.getElementById('result');
                    if (data.prediction === 'HP_ORIGINAL') {
                        result.className = 'result hp';
                        result.textContent = '✅ HP ORIGINAL (' + data.confidence.toFixed(1) + '%)';
                    } else {
                        result.className = 'result nao-hp';
                        result.textContent = '❌ NÃO HP (' + data.confidence.toFixed(1) + '%)';
                    }
                })
                .catch(error => {
                    console.error('Erro:', error);
                    document.getElementById('status').textContent = 'Erro na comunicação: ' + error.message;
                });
        }

        function testConnection() {
            console.log('Testando conexão...');
            fetch('/test')
                .then(response => response.text())
                .then(data => {
                    document.getElementById('status').textContent = 'Teste OK: ' + data;
                })
                .catch(error => {
                    document.getElementById('status').textContent = 'Erro no teste: ' + error.message;
                });
        }

        function diagnosis() {
            console.log('Executando diagnóstico...');
            fetch('/health')
                .then(response => response.text())
                .then(data => {
                    document.getElementById('status').textContent = 'Diagnóstico: ' + data;
                })
                .catch(error => {
                    document.getElementById('status').textContent = 'Erro no diagnóstico: ' + error.message;
                });
        }

        function autoUpdate() {
            if (autoUpdateActive) {
                clearInterval(updateInterval);
                autoUpdateActive = false;
                document.getElementById('status').textContent = 'Auto-update DESATIVADO';
            } else {
                updateInterval = setInterval(() => {
                    updateCamera();
                    updateData();
                }, 3000);
                autoUpdateActive = true;
                document.getElementById('status').textContent = 'Auto-update ATIVADO (3s)';
            }
        }

        // Inicialização
        window.onload = function() {
            console.log('Página carregada');
            testConnection();
            setTimeout(() => {
                updateCamera();
                updateData();
            }, 1000);
        };

        // Tratamento de erros de imagem
        document.getElementById('camera').onerror = function() {
            console.log('Erro ao carregar imagem');
            this.alt = 'Erro na câmera - clique em Atualizar Câmera';
            document.getElementById('status').textContent = 'Erro: Imagem da câmera não carregou';
        };

        document.getElementById('camera').onload = function() {
            console.log('Imagem carregada com sucesso');
            document.getElementById('status').textContent = 'Câmera funcionando';
        };
    </script>
</body>
</html>
)rawliteral";
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
  
  // Atualiza dados globais para a interface web
  current_data.prediction = String(prediction);
  current_data.confidence = confidence;
  current_data.hp_score = hp_score;
  current_data.nao_hp_score = nao_hp_score;
  current_data.avg_r = avg_r;
  current_data.avg_g = avg_g;
  current_data.avg_b = avg_b;
  current_data.total_inferences = total_inferences;
  current_data.avg_time = (float)total_time_ms / total_inferences;
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
  Serial.println("📷 Sistema de Classificação com Interface Web");
  printRepeatChar('=', 60);

  // Conecta ao WiFi
  Serial.println("📶 Conectando ao WiFi...");
  WiFi.begin(ssid, password);
  
  int attempts = 0;
  while (WiFi.status() != WL_CONNECTED && attempts < 20) {
    delay(500);
    Serial.print(".");
    attempts++;
  }
  
  if (WiFi.status() == WL_CONNECTED) {
    Serial.println("\n✅ WiFi conectado!");
    Serial.printf("📡 IP: %s\n", WiFi.localIP().toString().c_str());
    Serial.printf("🌐 Acesse: http://%s\n", WiFi.localIP().toString().c_str());
  } else {
    Serial.println("\n❌ Falha ao conectar WiFi - Modo AP");
    WiFi.softAP("ESP32-CAM-HP", "12345678");
    Serial.printf("📡 AP criado: http://%s\n", WiFi.softAPIP().toString().c_str());
  }

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

  // Configura servidor web
  server.on("/", handleRoot);
  server.on("/capture.jpg", HTTP_GET, handleCapture);
  server.on("/status", HTTP_GET, handleStatus);
  server.on("/test", HTTP_GET, handleTest);
  server.on("/health", HTTP_GET, handleHealth);
  
  server.begin();
  Serial.println("🌐 Servidor web iniciado!");

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
  server.handleClient();
  run_simulation();
  delay(1000);  // 1 FPS para demonstração
}
