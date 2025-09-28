#!/usr/bin/env python3
"""
SPRINT 3 - Arquivo de Configuração
==================================

Centraliza todas as configurações do projeto para facilitar
personalização e manutenção.

Autor: Equipe SPRINT 3
Data: 2025
"""

# =============================================================================
# CONFIGURAÇÕES DO MODELO
# =============================================================================

# Parâmetros da imagem de entrada
IMG_SIZE = 96                    # Tamanho da imagem (quadrado)
CHANNELS = 1                     # Número de canais (1=grayscale, 3=RGB)
INPUT_RANGE = (0, 255)          # Range de valores de entrada

# Configurações de quantização
QUANTIZATION_TYPE = "int8"       # Tipo de quantização
TARGET_SIZE_MB = 1.0            # Tamanho máximo do modelo em MB

# =============================================================================
# CONFIGURAÇÕES DO ESP32-CAM
# =============================================================================

# Configurações da câmera
CAMERA_RESOLUTION = "QQVGA"      # Resolução de captura (160x120)
CAMERA_FORMAT = "RGB565"         # Formato de pixel
CAMERA_FPS = 3                   # Frames por segundo

# Configurações de memória
TENSOR_ARENA_SIZE_KB = 380       # Tamanho da arena de memória em KB
USE_PSRAM = True                 # Usar PSRAM se disponível

# Configurações de processamento
PREPROCESSING_METHOD = "nearest" # Método de redimensionamento
NORMALIZATION = "scale_zero_point" # Método de normalização

# =============================================================================
# CONFIGURAÇÕES DE TESTE
# =============================================================================

# Dataset de validação
TEST_IMAGES_HP = 5               # Número de imagens HP para teste
TEST_IMAGES_NAO_HP = 5          # Número de imagens não-HP para teste
TOTAL_TEST_IMAGES = TEST_IMAGES_HP + TEST_IMAGES_NAO_HP

# Critérios de performance
MIN_ACCURACY = 70.0             # Acurácia mínima esperada (%)
MAX_LATENCY_MS = 500            # Latência máxima aceitável (ms)
MIN_PRECISION = 60.0            # Precisão mínima esperada (%)

# =============================================================================
# CONFIGURAÇÕES DE ARQUIVOS
# =============================================================================

# Caminhos dos arquivos
MODEL_INPUT_PATH = "model_best.keras"     # Modelo da Sprint 1
MODEL_OUTPUT_PATH = "model_int8.tflite"   # Modelo quantizado
MODEL_C_ARRAY_PATH = "model.h"            # Modelo em array C
LABELS_FILE = "labels.txt"                # Arquivo de labels

# Diretórios
REPRESENTATIVE_DATA_DIR = "representative_data"
HP_ORIGINAL_DIR = "representative_data/hp_original"
NAO_HP_DIR = "representative_data/nao_hp"
FIRMWARE_DIR = "firmware"
REPORT_DIR = "relatorio"

# =============================================================================
# CONFIGURAÇÕES DE LABELS
# =============================================================================

# Labels das classes
CLASS_LABELS = ["HP_ORIGINAL", "NAO_HP"]
NUM_CLASSES = len(CLASS_LABELS)

# Mapeamento de classes
CLASS_MAPPING = {
    "HP_ORIGINAL": 0,
    "NAO_HP": 1,
    "HP": 0,           # Alias
    "NAO_HP": 1,       # Alias
    "FALSO": 1,        # Alias
    "FALSIFICADO": 1   # Alias
}

# =============================================================================
# CONFIGURAÇÕES DE LOGGING
# =============================================================================

# Níveis de log
LOG_LEVEL = "INFO"               # DEBUG, INFO, WARNING, ERROR
LOG_FORMAT = "%(asctime)s - %(levelname)s - %(message)s"

# Arquivos de log
LOG_FILE = "sprint3.log"
ERROR_LOG_FILE = "sprint3_errors.log"

# =============================================================================
# CONFIGURAÇÕES DE PLATFORMIO
# =============================================================================

# Configurações do PlatformIO
PLATFORMIO_ENV = "esp32cam"
BOARD = "esp32cam"
FRAMEWORK = "arduino"
PLATFORM = "espressif32"

# Flags de compilação
BUILD_FLAGS = [
    "-DCORE_DEBUG_LEVEL=0",
    "-DCAMERA_MODEL_AI_THINKER=1",
    "-DARDUINO_USB_CDC_ON_BOOT=1"
]

# Bibliotecas necessárias
REQUIRED_LIBRARIES = [
    "espressif/esp32-camera@^2.0.4",
    "tensorflow/lite@^2.13.0"
]

# Configurações de upload
UPLOAD_SPEED = 921600
MONITOR_SPEED = 115200

# =============================================================================
# CONFIGURAÇÕES DE VALIDAÇÃO
# =============================================================================

# Parâmetros de validação
VALIDATION_TIMEOUT_SEC = 30      # Timeout para validação
MAX_RETRIES = 3                   # Máximo de tentativas
RETRY_DELAY_SEC = 1               # Delay entre tentativas

# Critérios de validação
VALIDATION_CRITERIA = {
    "model_size_mb": TARGET_SIZE_MB,
    "min_accuracy": MIN_ACCURACY,
    "max_latency_ms": MAX_LATENCY_MS,
    "min_precision": MIN_PRECISION
}

# =============================================================================
# CONFIGURAÇÕES DE RELATÓRIO
# =============================================================================

# Template do relatório
REPORT_TEMPLATE = "relatorio/relatorio_sprint3.md"
REPORT_OUTPUT = "relatorio/relatorio_sprint3_final.md"

# Seções do relatório
REPORT_SECTIONS = [
    "resumo_executivo",
    "processo_exportacao",
    "implementacao_esp32",
    "resultados_testes",
    "screenshots_evidencias",
    "desafios_solucoes",
    "conclusoes",
    "arquivos_entregues",
    "referencias"
]

# =============================================================================
# FUNÇÕES AUXILIARES
# =============================================================================

def get_model_config():
    """Retorna configurações do modelo"""
    return {
        "img_size": IMG_SIZE,
        "channels": CHANNELS,
        "input_range": INPUT_RANGE,
        "quantization_type": QUANTIZATION_TYPE,
        "target_size_mb": TARGET_SIZE_MB
    }

def get_esp32_config():
    """Retorna configurações do ESP32-CAM"""
    return {
        "camera_resolution": CAMERA_RESOLUTION,
        "camera_format": CAMERA_FORMAT,
        "camera_fps": CAMERA_FPS,
        "tensor_arena_size_kb": TENSOR_ARENA_SIZE_KB,
        "use_psram": USE_PSRAM,
        "preprocessing_method": PREPROCESSING_METHOD,
        "normalization": NORMALIZATION
    }

def get_test_config():
    """Retorna configurações de teste"""
    return {
        "test_images_hp": TEST_IMAGES_HP,
        "test_images_nao_hp": TEST_IMAGES_NAO_HP,
        "total_test_images": TOTAL_TEST_IMAGES,
        "min_accuracy": MIN_ACCURACY,
        "max_latency_ms": MAX_LATENCY_MS,
        "min_precision": MIN_PRECISION
    }

def get_file_paths():
    """Retorna caminhos dos arquivos"""
    return {
        "model_input": MODEL_INPUT_PATH,
        "model_output": MODEL_OUTPUT_PATH,
        "model_c_array": MODEL_C_ARRAY_PATH,
        "labels_file": LABELS_FILE,
        "representative_data_dir": REPRESENTATIVE_DATA_DIR,
        "hp_original_dir": HP_ORIGINAL_DIR,
        "nao_hp_dir": NAO_HP_DIR,
        "firmware_dir": FIRMWARE_DIR,
        "report_dir": REPORT_DIR
    }

def validate_config():
    """Valida as configurações"""
    errors = []
    
    # Validações básicas
    if IMG_SIZE <= 0:
        errors.append("IMG_SIZE deve ser positivo")
    
    if CHANNELS not in [1, 3]:
        errors.append("CHANNELS deve ser 1 ou 3")
    
    if TARGET_SIZE_MB <= 0:
        errors.append("TARGET_SIZE_MB deve ser positivo")
    
    if TOTAL_TEST_IMAGES <= 0:
        errors.append("TOTAL_TEST_IMAGES deve ser positivo")
    
    if len(CLASS_LABELS) != NUM_CLASSES:
        errors.append("NUM_CLASSES deve coincidir com CLASS_LABELS")
    
    return errors

# =============================================================================
# EXECUÇÃO PRINCIPAL
# =============================================================================

if __name__ == "__main__":
    print("🔧 SPRINT 3 - Configurações do Projeto")
    print("=" * 50)
    
    # Valida configurações
    errors = validate_config()
    if errors:
        print("❌ Erros de configuração encontrados:")
        for error in errors:
            print(f"   - {error}")
    else:
        print("✅ Configurações válidas")
    
    # Exibe configurações principais
    print("\n📋 Configurações Principais:")
    print(f"   - Tamanho da imagem: {IMG_SIZE}x{IMG_SIZE}")
    print(f"   - Canais: {CHANNELS}")
    print(f"   - Tamanho máximo do modelo: {TARGET_SIZE_MB}MB")
    print(f"   - Imagens de teste: {TOTAL_TEST_IMAGES}")
    print(f"   - Classes: {CLASS_LABELS}")
    
    print("\n📁 Caminhos dos Arquivos:")
    paths = get_file_paths()
    for key, path in paths.items():
        print(f"   - {key}: {path}")
    
    print("\n✅ Configuração carregada com sucesso!")
