/*
 * SPRINT 3 - ESP32-CAM Configuração SDK Mínima
 * ===========================================
 *
 * Este arquivo contém as definições mínimas necessárias para o ESP-IDF.
 *
 * Autor: Equipe SPRINT 3
 * Data: 2025
 */

#pragma once

// Configurações do FreeRTOS
#define CONFIG_FREERTOS_HZ 1000
#define CONFIG_FREERTOS_MAX_TASK_NAME_LEN 16

// USB CDC
#define CONFIG_TINYUSB_CDC_ENABLED 1

// Configurações necessárias para memória
#define CONFIG_ESP32_SPIRAM_SUPPORT 1
