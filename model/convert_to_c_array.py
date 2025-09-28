#!/usr/bin/env python3
"""
SPRINT 3 - Conversor de Modelo .tflite para Array C
===================================================

Este script converte o modelo .tflite para um array C que pode ser
incorporado no firmware do ESP32-CAM.

Uso:
    python convert_to_c_array.py model_int8.tflite

Autor: Equipe SPRINT 3
Data: 2025
"""

import os
import sys
import argparse

def convert_tflite_to_c_array(tflite_path, output_path="model.h"):
    """
    Converte um arquivo .tflite para um array C.
    
    Args:
        tflite_path (str): Caminho para o arquivo .tflite
        output_path (str): Caminho para o arquivo .h de saída
    """
    if not os.path.exists(tflite_path):
        print(f"ERRO: Arquivo {tflite_path} não encontrado!")
        return False
    
    try:
        # Lê o arquivo binário
        with open(tflite_path, 'rb') as f:
            data = f.read()
        
        # Converte para array C
        with open(output_path, 'w') as f:
            f.write("#pragma once\n")
            f.write("#include <stdint.h>\n\n")
            f.write("// Modelo TensorFlow Lite quantizado INT8\n")
            f.write("// Gerado automaticamente pelo script convert_to_c_array.py\n\n")
            f.write("const unsigned char g_model[] = {\n")
            
            # Escreve os bytes em formato hexadecimal
            for i, byte in enumerate(data):
                if i % 12 == 0:
                    f.write("  ")
                f.write(f"0x{byte:02x}")
                if i < len(data) - 1:
                    f.write(",")
                if i % 12 == 11:
                    f.write("\n")
                elif i % 12 != 11:
                    f.write(" ")
            
            if len(data) % 12 != 0:
                f.write("\n")
            
            f.write("};\n\n")
            f.write(f"const int g_model_len = {len(data)};\n")
        
        print(f"✅ Modelo convertido com sucesso!")
        print(f"   Arquivo de entrada: {tflite_path}")
        print(f"   Arquivo de saída: {output_path}")
        print(f"   Tamanho: {len(data):,} bytes ({len(data)/1024:.1f} KB)")
        
        return True
        
    except Exception as e:
        print(f"ERRO durante a conversão: {e}")
        return False

def main():
    parser = argparse.ArgumentParser(description="Converte modelo .tflite para array C")
    parser.add_argument("input", help="Caminho para o arquivo .tflite")
    parser.add_argument("-o", "--output", default="model.h", help="Arquivo .h de saída")
    
    args = parser.parse_args()
    
    print("=" * 50)
    print("SPRINT 3 - Conversor .tflite para Array C")
    print("=" * 50)
    
    success = convert_tflite_to_c_array(args.input, args.output)
    
    if success:
        print("\n✅ Conversão concluída com sucesso!")
        print(f"\nPróximos passos:")
        print(f"1. Copie {args.output} para firmware/src/")
        print(f"2. Compile o firmware com PlatformIO")
        print(f"3. Faça upload para o ESP32-CAM")
    else:
        print("\n❌ Falha na conversão.")
        sys.exit(1)

if __name__ == "__main__":
    main()
