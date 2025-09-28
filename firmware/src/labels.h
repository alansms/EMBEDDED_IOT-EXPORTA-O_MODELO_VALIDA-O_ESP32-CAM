/*
 * SPRINT 3 - Labels das Classes
 * =============================
 * 
 * Definição das classes de classificação para o modelo
 * de detecção de cartuchos HP.
 * 
 * Autor: Equipe SPRINT 3
 * Data: 2025
 */

#pragma once

// Labels das classes de classificação
static const char* kCategoryLabels[] = {
  "HP_ORIGINAL",    // Classe 0: Cartucho HP original
  "NAO_HP"         // Classe 1: Cartucho não-HP ou falsificado
};

// Número total de classes
static const int kCategoryCount = 2;
