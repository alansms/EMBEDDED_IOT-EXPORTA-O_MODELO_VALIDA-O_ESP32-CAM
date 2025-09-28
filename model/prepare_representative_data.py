#!/usr/bin/env python3
"""
SPRINT 3 - Preparação de Dataset Representativo
================================================

Este script ajuda a preparar o dataset representativo para calibração
da quantização INT8 do modelo TensorFlow Lite.

Funcionalidades:
- Redimensiona imagens para tamanho adequado
- Converte para formato correto (grayscale/RGB)
- Organiza em pastas por classe
- Gera dataset balanceado

Uso:
    python prepare_representative_data.py --input-dir /path/to/images --output-dir representative_data

Autor: Equipe SPRINT 3
Data: 2025
"""

import os
import argparse
import shutil
from pathlib import Path
from PIL import Image
import random

def resize_and_convert_image(input_path, output_path, target_size=(64, 64), channels=1):
    """
    Redimensiona e converte uma imagem para o formato adequado.
    
    Args:
        input_path (str): Caminho da imagem de entrada
        output_path (str): Caminho da imagem de saída
        target_size (tuple): Tamanho alvo (width, height)
        channels (int): Número de canais (1=grayscale, 3=RGB)
    """
    try:
        # Carrega a imagem
        img = Image.open(input_path)
        
        # Converte para o formato correto
        if channels == 1:
            img = img.convert('L')  # Grayscale
        else:
            img = img.convert('RGB')
        
        # Redimensiona mantendo proporção
        img.thumbnail(target_size, Image.Resampling.LANCZOS)
        
        # Cria imagem do tamanho exato (preenche com preto se necessário)
        new_img = Image.new(img.mode, target_size, (0, 0, 0) if channels == 3 else 0)
        new_img.paste(img, ((target_size[0] - img.width) // 2, 
                           (target_size[1] - img.height) // 2))
        
        # Salva a imagem
        new_img.save(output_path, 'JPEG', quality=85)
        return True
        
    except Exception as e:
        print(f"Erro ao processar {input_path}: {e}")
        return False

def prepare_representative_dataset(input_dir, output_dir, target_size=(64, 64), 
                                 channels=1, samples_per_class=50):
    """
    Prepara dataset representativo para calibração.
    
    Args:
        input_dir (str): Diretório com imagens originais
        output_dir (str): Diretório de saída
        target_size (tuple): Tamanho das imagens
        channels (int): Número de canais
        samples_per_class (int): Amostras por classe
    """
    print("🔄 Preparando dataset representativo...")
    print(f"   Tamanho alvo: {target_size}")
    print(f"   Canais: {channels}")
    print(f"   Amostras por classe: {samples_per_class}")
    
    # Cria estrutura de diretórios
    hp_dir = os.path.join(output_dir, "hp_original")
    nao_hp_dir = os.path.join(output_dir, "nao_hp")
    
    os.makedirs(hp_dir, exist_ok=True)
    os.makedirs(nao_hp_dir, exist_ok=True)
    
    # Processa imagens HP
    hp_images = []
    for root, dirs, files in os.walk(input_dir):
        for file in files:
            if file.lower().endswith(('.jpg', '.jpeg', '.png', '.bmp')):
                if 'hp' in file.lower() or 'original' in file.lower():
                    hp_images.append(os.path.join(root, file))
    
    print(f"📁 Encontradas {len(hp_images)} imagens HP")
    
    # Seleciona amostras aleatórias
    selected_hp = random.sample(hp_images, min(samples_per_class, len(hp_images)))
    
    for i, img_path in enumerate(selected_hp):
        output_path = os.path.join(hp_dir, f"hp_{i:03d}.jpg")
        resize_and_convert_image(img_path, output_path, target_size, channels)
    
    # Processa imagens não-HP
    nao_hp_images = []
    for root, dirs, files in os.walk(input_dir):
        for file in files:
            if file.lower().endswith(('.jpg', '.jpeg', '.png', '.bmp')):
                if 'hp' not in file.lower() and 'original' not in file.lower():
                    nao_hp_images.append(os.path.join(root, file))
    
    print(f"📁 Encontradas {len(nao_hp_images)} imagens não-HP")
    
    # Seleciona amostras aleatórias
    selected_nao_hp = random.sample(nao_hp_images, min(samples_per_class, len(nao_hp_images)))
    
    for i, img_path in enumerate(selected_nao_hp):
        output_path = os.path.join(nao_hp_dir, f"nao_hp_{i:03d}.jpg")
        resize_and_convert_image(img_path, output_path, target_size, channels)
    
    print(f"✅ Dataset preparado:")
    print(f"   HP: {len(selected_hp)} imagens")
    print(f"   Não-HP: {len(selected_nao_hp)} imagens")
    print(f"   Total: {len(selected_hp) + len(selected_nao_hp)} imagens")

def create_sample_images(output_dir, num_samples=20):
    """
    Cria imagens de exemplo para teste (quando não há dataset real).
    """
    print("🎨 Criando imagens de exemplo...")
    
    hp_dir = os.path.join(output_dir, "hp_original")
    nao_hp_dir = os.path.join(output_dir, "nao_hp")
    
    os.makedirs(hp_dir, exist_ok=True)
    os.makedirs(nao_hp_dir, exist_ok=True)
    
    # Cria imagens HP (azul/verde)
    for i in range(num_samples // 2):
        img = Image.new('RGB', (64, 64), (0, 100, 200))  # Azul
        img.save(os.path.join(hp_dir, f"hp_sample_{i:03d}.jpg"))
    
    # Cria imagens não-HP (vermelho/laranja)
    for i in range(num_samples // 2):
        img = Image.new('RGB', (64, 64), (200, 50, 0))   # Vermelho
        img.save(os.path.join(nao_hp_dir, f"nao_hp_sample_{i:03d}.jpg"))
    
    print(f"✅ Criadas {num_samples} imagens de exemplo")

def main():
    parser = argparse.ArgumentParser(description="Prepara dataset representativo para calibração")
    parser.add_argument("--input-dir", help="Diretório com imagens originais")
    parser.add_argument("--output-dir", default="representative_data", help="Diretório de saída")
    parser.add_argument("--size", type=int, default=64, help="Tamanho das imagens (quadrado)")
    parser.add_argument("--channels", type=int, default=1, choices=[1, 3], help="Número de canais")
    parser.add_argument("--samples", type=int, default=50, help="Amostras por classe")
    parser.add_argument("--create-samples", action="store_true", help="Criar imagens de exemplo")
    
    args = parser.parse_args()
    
    print("=" * 60)
    print("🔄 SPRINT 3 - Preparação de Dataset Representativo")
    print("=" * 60)
    
    if args.create_samples:
        create_sample_images(args.output_dir, args.samples * 2)
    elif args.input_dir:
        if not os.path.exists(args.input_dir):
            print(f"❌ Diretório {args.input_dir} não encontrado")
            return
        
        prepare_representative_dataset(
            args.input_dir, 
            args.output_dir, 
            (args.size, args.size), 
            args.channels, 
            args.samples
        )
    else:
        print("❌ Especifique --input-dir ou use --create-samples")
        print("\nExemplos de uso:")
        print("  python prepare_representative_data.py --input-dir /path/to/images")
        print("  python prepare_representative_data.py --create-samples --samples 30")
        return
    
    print("\n✅ Dataset representativo preparado com sucesso!")
    print(f"📁 Localização: {args.output_dir}")
    print("\nPróximo passo: execute export_tflite.py")

if __name__ == "__main__":
    main()
