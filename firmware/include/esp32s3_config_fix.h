// esp32s3_config_fix.h
// Este arquivo define configurações críticas necessárias para compilar o projeto
// para ESP32-S3 no PlatformIO com macOS

#ifndef ESP32S3_CONFIG_FIX_H
#define ESP32S3_CONFIG_FIX_H

#include "driver/gpio.h"

// Definições para resolver problemas de compilação do lwIP
#define CONFIG_TCP_OVERSIZE_MSS 1
#define LWIP_TCP_OVERSIZE_MSS 1
#define CONFIG_LWIP_DHCP_COARSE_TIMER_SECS 60
#define CONFIG_LWIP_TCP_OVERSIZE_MSS 1

// Definições básicas do ESP32S3 - simplificadas para garantir conectividade
#define CONFIG_IDF_TARGET_ESP32S3 1
#define CONFIG_IDF_TARGET "esp32s3"
#define CONFIG_ESP32S3_DEFAULT_CPU_FREQ_MHZ 240

// Configurações básicas para câmera - removendo configurações conflitantes
#define CONFIG_CAMERA_SUPPORTS_PSRAM 1

#endif // ESP32S3_CONFIG_FIX_H
