#!/usr/bin/python3

import sys
import urllib.parse

#Aclaracion Marcos: en CGI sys.argv[0] es el nombre del script, los argumentos empiezan en sys.argv[1]

query_string = sys.argv[1] if len(sys.argv) > 1 else ""

# Parseamos los parámetros de la URL
params = urllib.parse.parse_qs(query_string)
nombre = params.get("nombre", ["desconocido"])[0]

# Enviamos la respuesta HTTP
print(f"Hola {nombre}!")
