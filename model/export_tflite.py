#!/usr/bin/env python3
"""
SPRINT 3 - Script de Exportação do Modelo para TensorFlow Lite INT8
====================================================================

Este script converte o modelo treinado da Sprint 1 para o formato TensorFlow Lite
com quantização INT8 completa, otimizado para execução no ESP32-CAM.

Requisitos:
- Modelo da Sprint 1 salvo como .keras ou .h5
- Dataset representativo para calibração da quantização
- TensorFlow >= 2.8.0

Uso:
    python export_tflite.py

Autor: Equipe SPRINT 3
Data: 2025
"""

import os
import glob
import numpy as np
import tensorflow as tf
from PIL import Image
import argparse
from pathlib import Path

# =============================================================================
# CONFIGURAÇÕES DO MODELO
# =============================================================================

# Parâmetros do modelo (ajuste conforme sua arquitetura)
IMG_SIZE = 96
CHANNELS = 1            # 1 (grayscale) ou 3 (RGB)
INPUT_RANGE = (0, 255)  # TFLM INT8 espera uint8->int8 com scale/zero-point

# Caminhos dos arquivos
MODEL_PATH = "model_best.keras"  # ou .h5 - ajuste conforme necessário
OUT_TFLITE = "model_int8.tflite"
LABELS_FILE = "labels.txt"

# Diretórios do dataset representativo
REP_DIRS = [
    "representative_data/hp_original",
    "representative_data/nao_hp",
]

# Labels das classes
CLASS_LABELS = ["HP_ORIGINAL", "NAO_HP"]

# =============================================================================
# FUNÇÕES AUXILIARES
# =============================================================================

def load_and_preprocess(path):
    """
    Carrega e pré-processa uma imagem para o formato esperado pelo modelo.
    
    Args:
        path (str): Caminho para a imagem
        
    Returns:
        np.ndarray: Imagem pré-processada no formato (IMG_SIZE, IMG_SIZE, CHANNELS)
    """
    try:
        # Carrega a imagem
        im = Image.open(path)
        
        # Converte para grayscale se necessário
        if CHANNELS == 1:
            im = im.convert("L")
        else:
            im = im.convert("RGB")
        
        # Redimensiona para o tamanho esperado
        im = im.resize((IMG_SIZE, IMG_SIZE), Image.BILINEAR)
        
        # Converte para array numpy
        arr = np.array(im, dtype=np.uint8)  # 0..255
        
        # Ajusta dimensões se necessário
        if CHANNELS == 1:
            arr = arr.reshape(IMG_SIZE, IMG_SIZE, 1)
            
        return arr
        
    except Exception as e:
        print(f"Erro ao processar {path}: {e}")
        return None

def representative_dataset():
    """
    Gera dataset representativo para calibração da quantização INT8.
    
    Yields:
        np.ndarray: Batch de dados representativos
    """
    print("Carregando dataset representativo...")
    
    # Coleta todos os arquivos de imagem
    files = []
    for d in REP_DIRS:
        if os.path.exists(d):
            pattern = os.path.join(d, "*")
            files.extend(sorted(glob.glob(pattern)))
        else:
            print(f"Aviso: Diretório {d} não encontrado")
    
    print(f"Encontrados {len(files)} arquivos para calibração")
    
    # Processa até 300 amostras (recomendado: 100-300)
    processed = 0
    for path in files[:300]:
        arr = load_and_preprocess(path)
        if arr is not None:
            # Converte para float32 e normaliza para 0..1
            arr = arr.astype(np.float32) / 255.0
            
            # Adiciona dimensão de batch
            arr = np.expand_dims(arr, axis=0)
            
            yield [arr]
            processed += 1
            
            if processed % 50 == 0:
                print(f"Processadas {processed} amostras...")
    
    print(f"Total de amostras processadas: {processed}")

def validate_model_size(tflite_path, max_size_mb=1.0):
    """
    Valida se o modelo está dentro do tamanho limite.
    
    Args:
        tflite_path (str): Caminho para o modelo .tflite
        max_size_mb (float): Tamanho máximo em MB
        
    Returns:
        bool: True se o modelo está dentro do limite
    """
    size_bytes = os.path.getsize(tflite_path)
    size_mb = size_bytes / (1024 * 1024)
    
    print(f"Tamanho do modelo: {size_mb:.2f} MB")
    
    if size_mb > max_size_mb:
        print(f"AVISO: Modelo excede o limite de {max_size_mb} MB!")
        return False
    
    return True

def save_labels():
    """Salva as labels das classes em arquivo de texto."""
    with open(LABELS_FILE, 'w') as f:
        for i, label in enumerate(CLASS_LABELS):
            f.write(f"{i} {label}\n")
    print(f"Labels salvas em {LABELS_FILE}")

# =============================================================================
# FUNÇÃO PRINCIPAL DE EXPORTAÇÃO
# =============================================================================

def export_model():
    """
    Função principal para exportar o modelo para TensorFlow Lite INT8.
    """
    print("=" * 60)
    print("SPRINT 3 - Exportação do Modelo para TensorFlow Lite INT8")
    print("=" * 60)
    
    # Verifica se o modelo existe
    if not os.path.exists(MODEL_PATH):
        print(f"ERRO: Modelo {MODEL_PATH} não encontrado!")
        print("Certifique-se de que o modelo da Sprint 1 está no diretório correto.")
        return False
    
    try:
        # 1. Carrega o modelo Keras
        print(f"Carregando modelo: {MODEL_PATH}")
        model = tf.keras.models.load_model(MODEL_PATH)
        
        # Exibe informações do modelo
        print(f"Arquitetura do modelo:")
        print(f"  - Input shape: {model.input_shape}")
        print(f"  - Output shape: {model.output_shape}")
        print(f"  - Parâmetros: {model.count_params():,}")
        
        # 2. Configura o conversor TensorFlow Lite
        print("\nConfigurando conversor TensorFlow Lite...")
        converter = tf.lite.TFLiteConverter.from_keras_model(model)
        
        # Otimizações para INT8
        converter.optimizations = [tf.lite.Optimize.DEFAULT]
        converter.representative_dataset = representative_dataset
        
        # Quantização INT8 completa (entrada e saída)
        converter.target_spec.supported_ops = [tf.lite.OpsSet.TFLITE_BUILTINS_INT8]
        converter.inference_input_type = tf.int8
        converter.inference_output_type = tf.int8
        
        # 3. Converte o modelo
        print("Convertendo modelo para TensorFlow Lite INT8...")
        tflite_model = converter.convert()
        
        # 4. Salva o modelo
        with open(OUT_TFLITE, "wb") as f:
            f.write(tflite_model)
        
        print(f"Modelo salvo: {OUT_TFLITE}")
        
        # 5. Valida o tamanho
        if not validate_model_size(OUT_TFLITE):
            print("Considere reduzir a arquitetura do modelo se necessário.")
        
        # 6. Salva as labels
        save_labels()
        
        print("\n" + "=" * 60)
        print("EXPORTAÇÃO CONCLUÍDA COM SUCESSO!")
        print("=" * 60)
        print(f"Arquivos gerados:")
        print(f"  - {OUT_TFLITE} (modelo quantizado)")
        print(f"  - {LABELS_FILE} (labels das classes)")
        print("\nPróximos passos:")
        print("1. Execute o script convert_to_c_array.py para gerar model.h")
        print("2. Compile o firmware para ESP32-CAM")
        print("3. Teste com imagens reais")
        
        return True
        
    except Exception as e:
        print(f"ERRO durante a exportação: {e}")
        print("\nPossíveis soluções:")
        print("1. Verifique se o modelo da Sprint 1 está correto")
        print("2. Confirme se o dataset representativo existe")
        print("3. Ajuste os parâmetros IMG_SIZE e CHANNELS se necessário")
        return False

# =============================================================================
# SCRIPT PRINCIPAL
# =============================================================================

if __name__ == "__main__":
    parser = argparse.ArgumentParser(description="Exporta modelo para TensorFlow Lite INT8")
    parser.add_argument("--model", default=MODEL_PATH, help="Caminho para o modelo Keras")
    parser.add_argument("--output", default=OUT_TFLITE, help="Nome do arquivo .tflite de saída")
    parser.add_argument("--img-size", type=int, default=IMG_SIZE, help="Tamanho da imagem de entrada")
    parser.add_argument("--channels", type=int, default=CHANNELS, help="Número de canais (1=grayscale, 3=RGB)")
    
    args = parser.parse_args()
    
    # Atualiza configurações globais
    MODEL_PATH = args.model
    OUT_TFLITE = args.output
    IMG_SIZE = args.img_size
    CHANNELS = args.channels
    
    # Executa a exportação
    success = export_model()
    
    if success:
        print("\n✅ Exportação concluída com sucesso!")
    else:
        print("\n❌ Falha na exportação. Verifique os erros acima.")
        exit(1)
