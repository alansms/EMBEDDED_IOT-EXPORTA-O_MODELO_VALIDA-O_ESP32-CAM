#!/usr/bin/env python3
"""
SPRINT 3 - Script de Validação dos Resultados
=============================================

Este script ajuda a analisar os resultados dos testes realizados
no ESP32-CAM e gerar estatísticas de performance.

Uso:
    python test_validation.py

Autor: Equipe SPRINT 3
Data: 2025
"""

import json
import argparse
from datetime import datetime
from pathlib import Path

class ValidationResults:
    def __init__(self):
        self.results = []
        self.total_tests = 0
        self.correct_predictions = 0
        self.hp_correct = 0
        self.hp_total = 0
        self.non_hp_correct = 0
        self.non_hp_total = 0
        self.latencies = []
        
    def add_result(self, image_id, real_class, predicted_class, latency_ms, hp_score, non_hp_score):
        """Adiciona um resultado de teste"""
        is_correct = (real_class == predicted_class)
        
        result = {
            'image_id': image_id,
            'real_class': real_class,
            'predicted_class': predicted_class,
            'correct': is_correct,
            'latency_ms': latency_ms,
            'hp_score': hp_score,
            'non_hp_score': non_hp_score
        }
        
        self.results.append(result)
        self.total_tests += 1
        
        if is_correct:
            self.correct_predictions += 1
            
        if real_class == 'HP':
            self.hp_total += 1
            if is_correct:
                self.hp_correct += 1
        else:
            self.non_hp_total += 1
            if is_correct:
                self.non_hp_correct += 1
                
        self.latencies.append(latency_ms)
    
    def calculate_metrics(self):
        """Calcula métricas de performance"""
        if self.total_tests == 0:
            return {}
            
        accuracy = (self.correct_predictions / self.total_tests) * 100
        
        precision_hp = (self.hp_correct / self.hp_total * 100) if self.hp_total > 0 else 0
        recall_hp = (self.hp_correct / self.hp_total * 100) if self.hp_total > 0 else 0
        
        precision_non_hp = (self.non_hp_correct / self.non_hp_total * 100) if self.non_hp_total > 0 else 0
        recall_non_hp = (self.non_hp_correct / self.non_hp_total * 100) if self.non_hp_total > 0 else 0
        
        avg_latency = sum(self.latencies) / len(self.latencies) if self.latencies else 0
        min_latency = min(self.latencies) if self.latencies else 0
        max_latency = max(self.latencies) if self.latencies else 0
        
        return {
            'accuracy': accuracy,
            'precision_hp': precision_hp,
            'recall_hp': recall_hp,
            'precision_non_hp': precision_non_hp,
            'recall_non_hp': recall_non_hp,
            'avg_latency_ms': avg_latency,
            'min_latency_ms': min_latency,
            'max_latency_ms': max_latency,
            'total_tests': self.total_tests,
            'correct_predictions': self.correct_predictions
        }
    
    def print_summary(self):
        """Imprime resumo dos resultados"""
        metrics = self.calculate_metrics()
        
        print("=" * 60)
        print("📊 RESULTADOS DA VALIDAÇÃO - SPRINT 3")
        print("=" * 60)
        print(f"📅 Data: {datetime.now().strftime('%d/%m/%Y %H:%M:%S')}")
        print(f"🧪 Total de testes: {metrics['total_tests']}")
        print(f"✅ Predições corretas: {metrics['correct_predictions']}")
        print()
        
        print("📈 MÉTRICAS DE PERFORMANCE:")
        print(f"   🎯 Acurácia Total: {metrics['accuracy']:.1f}%")
        print(f"   📊 Precisão HP: {metrics['precision_hp']:.1f}%")
        print(f"   📊 Recall HP: {metrics['recall_hp']:.1f}%")
        print(f"   📊 Precisão Não-HP: {metrics['precision_non_hp']:.1f}%")
        print(f"   📊 Recall Não-HP: {metrics['recall_non_hp']:.1f}%")
        print()
        
        print("⏱️ LATÊNCIA:")
        print(f"   📊 Média: {metrics['avg_latency_ms']:.1f}ms")
        print(f"   📊 Mínima: {metrics['min_latency_ms']:.1f}ms")
        print(f"   📊 Máxima: {metrics['max_latency_ms']:.1f}ms")
        print()
        
        print("📋 DETALHES POR IMAGEM:")
        print("| ID | Real | Predito | Correto | HP Score | Não-HP Score | Latência |")
        print("|----|------|---------|---------|----------|--------------|----------|")
        
        for result in self.results:
            status = "✅" if result['correct'] else "❌"
            print(f"| {result['image_id']:2d} | {result['real_class']:4s} | {result['predicted_class']:7s} | {status:7s} | {result['hp_score']:8d} | {result['non_hp_score']:12d} | {result['latency_ms']:8.1f} |")
    
    def save_to_json(self, filename="validation_results.json"):
        """Salva resultados em arquivo JSON"""
        data = {
            'timestamp': datetime.now().isoformat(),
            'results': self.results,
            'metrics': self.calculate_metrics()
        }
        
        with open(filename, 'w') as f:
            json.dump(data, f, indent=2)
        
        print(f"💾 Resultados salvos em {filename}")

def interactive_input():
    """Interface interativa para entrada de dados"""
    print("🧪 SPRINT 3 - Entrada de Dados de Validação")
    print("=" * 50)
    print("Digite os resultados dos testes realizados no ESP32-CAM")
    print("(Pressione Enter com campos vazios para finalizar)")
    print()
    
    validation = ValidationResults()
    image_id = 1
    
    while True:
        print(f"\n📸 Imagem {image_id}:")
        
        real_class = input("Tipo real (HP/NAO_HP): ").strip().upper()
        if not real_class:
            break
            
        predicted_class = input("Predição (HP/NAO_HP): ").strip().upper()
        if not predicted_class:
            break
            
        try:
            latency = float(input("Latência (ms): "))
            hp_score = int(input("Score HP: "))
            non_hp_score = int(input("Score Não-HP: "))
        except ValueError:
            print("❌ Valores inválidos, pulando esta imagem...")
            continue
        
        validation.add_result(image_id, real_class, predicted_class, latency, hp_score, non_hp_score)
        image_id += 1
    
    return validation

def load_from_file(filename):
    """Carrega resultados de arquivo JSON"""
    try:
        with open(filename, 'r') as f:
            data = json.load(f)
        
        validation = ValidationResults()
        for result in data['results']:
            validation.add_result(
                result['image_id'],
                result['real_class'],
                result['predicted_class'],
                result['latency_ms'],
                result['hp_score'],
                result['non_hp_score']
            )
        
        return validation
    except FileNotFoundError:
        print(f"❌ Arquivo {filename} não encontrado")
        return None
    except Exception as e:
        print(f"❌ Erro ao carregar arquivo: {e}")
        return None

def main():
    parser = argparse.ArgumentParser(description="Analisa resultados de validação do SPRINT 3")
    parser.add_argument("--load", help="Carregar resultados de arquivo JSON")
    parser.add_argument("--save", default="validation_results.json", help="Arquivo para salvar resultados")
    
    args = parser.parse_args()
    
    if args.load:
        validation = load_from_file(args.load)
        if validation is None:
            return
    else:
        validation = interactive_input()
    
    if validation.total_tests == 0:
        print("❌ Nenhum resultado encontrado")
        return
    
    validation.print_summary()
    validation.save_to_json(args.save)

if __name__ == "__main__":
    main()
