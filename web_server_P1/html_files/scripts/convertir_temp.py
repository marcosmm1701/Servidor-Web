#!/usr/bin/python3

import sys
import urllib.parse

# Leer la entrada estándar (para datos enviados en POST sin parámetros de formulario)
post_data = sys.stdin.read().strip()

# Si los datos están en formato 'temperature=25', parsearlos
if "=" in post_data:
    parsed_data = urllib.parse.parse_qs(post_data)
    temperature = parsed_data.get("temperature", [""])[0]  # Extraer el valor
    temperature = temperature.strip()  # Eliminar espacios en blanco
else:
    temperature = post_data  # Si solo es un número en crudo
    
temperature = temperature.strip()  # Eliminar espacios antes y después del valor

# Si se encuentra texto no numérico, lo reemplazamos (opcional)
temperature = ''.join(c for c in temperature if c.isdigit() or c == '.')


try:
    # Convertir a float
    celsius = float(temperature)

    # Convertir a Fahrenheit
    fahrenheit = (celsius * 9/5) + 32

    # Imprimir la respuesta
    print(f"{celsius}C equivale a {fahrenheit}F")
except ValueError:
    print("Error: Entrada no valida")
