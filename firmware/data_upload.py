#!/usr/bin/env python3
"""
Script para preparar e fazer upload dos arquivos da pasta data/ para SPIFFS
do ESP32S3 usando a ferramenta esptool.
"""

import os
import sys
import subprocess
import glob
import shutil
import time  # Adicionado import time para permitir pausas

# Constantes
FIRMWARE_DIR = os.path.dirname(os.path.abspath(__file__))
DATA_DIR = os.path.join(FIRMWARE_DIR, 'data')
BUILD_DIR = os.path.join(FIRMWARE_DIR, '.pio', 'build', 'seeed_xiao_esp32s3')
SPIFFS_BIN = os.path.join(BUILD_DIR, 'spiffs.bin')
SPIFFS_SIZE = 0x100000  # 1MB

# Constante para a porta serial (macOS)
SERIAL_PORT = '/dev/cu.usbmodem11401'  # Porta detectada no seu sistema

def check_platformio():
    """Verifica se o PlatformIO está instalado"""
    try:
        subprocess.run(['pio', '--version'],
                       stdout=subprocess.PIPE,
                       stderr=subprocess.PIPE,
                       check=True)
        return True
    except (subprocess.CalledProcessError, FileNotFoundError):
        return False

def check_files():
    """Verifica se existem arquivos na pasta data/"""
    files = glob.glob(os.path.join(DATA_DIR, '*.*'))
    if not files:
        print(f"❌ Erro: Não foram encontrados arquivos na pasta {DATA_DIR}")
        return False

    print(f"📁 Encontrados {len(files)} arquivo(s) na pasta data/:")
    total_size = 0
    for f in files:
        size = os.path.getsize(f)
        total_size += size
        print(f"   - {os.path.basename(f)} ({size} bytes)")

    print(f"📊 Tamanho total: {total_size} bytes")

    if total_size > SPIFFS_SIZE:
        print(f"❌ Erro: Tamanho total dos arquivos ({total_size} bytes) excede o limite de {SPIFFS_SIZE} bytes")
        return False

    return True

def build_spiffs():
    """Constrói a imagem SPIFFS"""
    print("🔧 Construindo imagem SPIFFS...")

    # Garante que o diretório de build existe
    os.makedirs(BUILD_DIR, exist_ok=True)

    try:
        result = subprocess.run(
            ['pio', 'run', '--target', 'buildfs'],
            cwd=FIRMWARE_DIR,
            capture_output=True,
            text=True
        )

        if result.returncode != 0:
            print(f"❌ Erro ao construir o SPIFFS: {result.stderr}")
            return False

        print("✅ Imagem SPIFFS construída com sucesso")
        return True
    except Exception as e:
        print(f"❌ Erro ao construir o SPIFFS: {str(e)}")
        return False

def upload_spiffs():
    """Faz o upload da imagem SPIFFS para o dispositivo"""
    print("📤 Fazendo upload da imagem SPIFFS para o dispositivo...")

    # Tentativa de resolver o problema de bloqueio da porta
    print("🔄 Tentando liberar a porta serial...")
    try:
        # Em macOS, podemos tentar desbloquear a porta
        if os.path.exists(SERIAL_PORT):
            # Aguarda um pouco para garantir que a porta esteja pronta
            time.sleep(2)
            print(f"ℹ️ Aguardando que a porta {SERIAL_PORT} esteja disponível...")
    except Exception as e:
        print(f"⚠️ Aviso ao tentar liberar a porta: {str(e)}")

    # Tentativas máximas de upload
    max_attempts = 3
    current_attempt = 1

    while current_attempt <= max_attempts:
        print(f"🔄 Tentativa {current_attempt} de {max_attempts}...")

        try:
            # Comando direto usando esptool para mais controle (alternativa ao PlatformIO)
            if current_attempt > 1:
                print("🔄 Usando comando alternativo com esptool...")
                # Usar a ferramenta esptool diretamente pode resolver problemas de acesso à porta
                # Este comando é equivalente ao que o PlatformIO executaria
                result = subprocess.run(
                    ['python3', '-m', 'esptool', '--chip', 'esp32s3', '--port', SERIAL_PORT,
                     '--baud', '921600', 'write_flash', '0x310000', SPIFFS_BIN],
                    cwd=FIRMWARE_DIR,
                    capture_output=True,
                    text=True,
                    timeout=60
                )
            else:
                # Primeira tentativa com PlatformIO
                result = subprocess.run(
                    ['pio', 'run', '--target', 'uploadfs'],
                    cwd=FIRMWARE_DIR,
                    capture_output=True,
                    text=True,
                    timeout=60
                )

            if result.returncode == 0:
                print("✅ Upload concluído com sucesso")
                return True
            else:
                print(f"⚠️ Erro na tentativa {current_attempt}: {result.stderr}")
                # Aguarda antes da próxima tentativa
                time.sleep(3)

        except subprocess.TimeoutExpired:
            print(f"⚠️ Timeout na tentativa {current_attempt}")
        except Exception as e:
            print(f"⚠️ Erro na tentativa {current_attempt}: {str(e)}")

        current_attempt += 1

    print("❌ Todas as tentativas de upload falharam.")

    # Sugestões para resolução manual
    print("\n🔧 Sugestões para resolver o problema:")
    print("1. Desconecte e reconecte o ESP32S3")
    print("2. Pressione o botão de reset no ESP32S3")
    print("3. Feche qualquer programa que possa estar usando a porta serial")
    print("4. Reinicie seu computador")
    print(f"5. Tente executar manualmente: pio run --target uploadfs ou esptool.py --chip esp32s3 --port {SERIAL_PORT} write_flash 0x310000 {SPIFFS_BIN}")

    return False

def main():
    """Função principal"""
    print("🚀 ESP32S3 SPIFFS Uploader - SPRINT 3")
    print("=====================================")

    # Verifica se o PlatformIO está instalado
    if not check_platformio():
        print("❌ Erro: PlatformIO CLI não encontrado. Por favor, instale-o.")
        sys.exit(1)

    # Verifica os arquivos
    if not check_files():
        sys.exit(1)

    # Constrói a imagem SPIFFS
    if not build_spiffs():
        sys.exit(1)

    # Faz o upload
    if not upload_spiffs():
        sys.exit(1)

    print("\n🎉 Processo concluído com sucesso!")
    print("Agora você pode acessar a interface web conectando ao WiFi 'ESP32S3_HP_Detector'")
    print("e acessando http://192.168.4.1 no seu navegador")

if __name__ == "__main__":
    main()
