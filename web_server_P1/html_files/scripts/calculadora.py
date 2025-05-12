#!/usr/bin/python3
# Este script recibe dos números y una operación y devuelve el resultado de la operación
# Hemos utilizado el método get para ello. Por lo que los agumentos se pasan por la url
# Ejemplo: http://localhost:8080/scripts/calculadora.py?num1=10&num2=5&operacion=multiplicacion

import sys
import urllib.parse

# Nos aseguramos que se recibe la cadena de consulta correctamente
# query_string1 = primer número
# query_string2 = segundo número
# query_string3 = operación
query_string1 = sys.argv[1] if len(sys.argv) > 1 else ""
query_string2 = sys.argv[2] if len(sys.argv) > 2 else ""
query_string3 = sys.argv[3] if len(sys.argv) > 3 else ""

# Concatenamos ambos query strings
combined_query_string = query_string1 + "&" + query_string2 + "&" + query_string3 if query_string1 and query_string2 and query_string3 else query_string1 + query_string2 + query_string3

# Parseamos los parámetros de la cadena combinada
params = urllib.parse.parse_qs(combined_query_string)


# Obtener valores correctamente
num1 = float(params["num1"][0]) if "num1" in params else 0
num2 = float(params["num2"][0]) if "num2" in params else 0
operacion = params["operacion"][0] if "operacion" in params else "suma"

# Debug: Imprimir los valores recibidos
print(f"Num1: {num1}")
print(f"Num2: {num2}")
print(f"Operacion: {operacion}")

# Realizar la operación
resultado = 0
if operacion == "suma":
    resultado = num1 + num2
elif operacion == "resta":
    resultado = num1 - num2
elif operacion == "multiplicacion":
    resultado = num1 * num2
elif operacion == "division":
    resultado = num1 / num2 if num2 != 0 else "Error: División por cero"
else:
    resultado = "Operación no soportada"

# Mostrar resultado
print(f"Resultado: {resultado}")
