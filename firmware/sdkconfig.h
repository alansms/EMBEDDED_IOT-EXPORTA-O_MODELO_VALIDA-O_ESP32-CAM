/*
 * sdkconfig.h - Configuração para Seeed Studio XIAO ESP32S3
 * ================================================
 * 
 * Arquivo de configuração necessário para compilação
 * do ESP32S3 com PlatformIO.
 *
 * Autor: Equipe SPRINT 3
 * Data: 2025
 */

#ifndef SDKCONFIG_H
#define SDKCONFIG_H

// Configurações básicas do ESP32S3 (corrigido de ESP32 para ESP32S3)
#define CONFIG_IDF_TARGET_ESP32S3 1
#define CONFIG_IDF_TARGET "esp32s3"

// Configurações de memória
#define CONFIG_ESP32S3_SPIRAM_SUPPORT 1
#define CONFIG_SPIRAM_USE_MALLOC 1

// Configurações de câmera
#define CONFIG_CAMERA_MODULE_ESP_S3_EYE 1  // Configuração para ESP32S3 com câmera

// Configurações de debug
#define CONFIG_LOG_DEFAULT_LEVEL_INFO 1

// Configurações de WiFi
#define CONFIG_ESP32_WIFI_STATIC_RX_BUFFER_NUM 10
#define CONFIG_ESP32_WIFI_DYNAMIC_RX_BUFFER_NUM 32

// Configurações de FreeRTOS
#define CONFIG_FREERTOS_HZ 1000

// Configurações de heap
#define CONFIG_HEAP_POISONING_COMPREHENSIVE 0

// Configurações para corrigir erros de compilação LwIP
#define CONFIG_TCP_OVERSIZE_MSS 1           // Adiciona a opção CONFIG_TCP_OVERSIZE_xxx faltante
#define CONFIG_LWIP_DHCP_COARSE_TIMER_SECS 60  // Define o tempo do timer DHCP

// Outras configurações necessárias para o ESP32S3
#define CONFIG_ESP32S3_DEFAULT_CPU_FREQ_240 1
#define CONFIG_ESP32S3_DEFAULT_CPU_FREQ_MHZ 240

#endif // SDKCONFIG_H

