# Memoria de la Práctica 1 - Servidor Web

## Introducción

Este documento presenta el desarrollo de la **Práctica 1** de la materia **Redes 2**, donde se ha creado un **servidor web operativo** que puede manejar solicitudes **HTTP/1.0** y **HTTP/1.1**. El principal objetivo de esta práctica es profundizar en los conocimientos sobre **protocolos de comunicación en redes** y la **concurrencia en servidores**, aplicando conceptos fundamentales de la **arquitectura cliente-servidor**. La implementación del servidor es capaz de gestionar solicitudes **GET, POST y OPTIONS**, así como ofrecer **archivos estáticos** y ejecutar **scripts dinámicos**. Asimismo, el servidor ha sido diseñado para manejar **concurrencia a través de hilos**, permitiendo que **múltiples clientes se conecten al mismo tiempo** y respondiendo de manera adecuada a **errores comunes en HTTP**.

---

## Desarrollo Técnico

### Decisiones de Diseño

- **Lenguaje de programación:** Se optó por **C** por su eficiencia y control sobre los recursos del sistema, lo cual es crucial para un servidor de alto rendimiento.
- **Manejo de concurrencia:** Se eligió crear **hilos (pthread)** para gestionar la conexión de varios clientes de forma simultánea, lo que facilita una mejor distribución de la carga.
- **Procesamiento de peticiones HTTP:** Se desarrolló un **parser de peticiones HTTP** que identifica diferentes versiones y maneja correctamente cabeceras relevantes como "Connection" y "Content-Length".
- **Elección del modelo de servidor:** Se optó por implementar un servidor concurrente reactivo, por su flexibilidad y escalabilidad, aparte de su óptimo rendimiento y gran capacidad de procesamiento.

### Estructura modular

La implementación se organizó en varios módulos:

- `server.c`: Se encarga de aceptar conexiones y manejar a los clientes.
- `connections.c`: Gestiona los sockets y las conexiones con los clientes.
- `parser.c`: Se ocupa de analizar las peticiones HTTP.
- `response.c`: Genera las respuestas y gestiona archivos estáticos.
- `scripts.c`: Ejecuta scripts dinámicos (Python/PHP).
- `config.c`: Carga y administra las configuraciones del servidor.
- `client.c`: Testa diversas funcionalidades desde el punto de vista del cliente.

### Descripción Funcional

- **Aceptación de conexiones:** Se utiliza `accept()` para recibir a los clientes, asignando un hilo independiente para cada uno.
- **Manejo de solicitudes:** Se lee la petición HTTP y se identifica el tipo de recurso que se solicita.
- **Servicio de archivos estáticos:** Si la petición es **GET** y el archivo está disponible, se devuelve su contenido junto a su tipo MIME correspondiente.
- **Ejecución de scripts:** Cuando se solicita un **script Python o PHP**, se ejecuta utilizando los parámetros proporcionados y se devuelve el resultado. Hemos implementado **4** scripts adicionales para ofrecer una mejor exposición del proyecto.
- **Gestor de errores:** En caso de que la petición sea inválida o el archivo no se encuentre, se envía el código de error HTTP apropiado (como `404`, `405`, etc.).
- **Cierre seguro:** El servidor gestiona la señal **SIGINT** para cerrar adecuadamente todas las conexiones y liberar los recursos utilizados.

---

## Conclusiones Técnicas

- La implementación en **C** resultó en un servidor eficiente, aunque requirió prestar mucha atención al manejo de memoria y a la sincronización entre hilos.
- El uso de **pthreads** fue crucial para manejar la concurrencia, aunque presentó retos relacionados con la gestión de recursos compartidos.
- La **modularización** del código hizo que el mantenimiento y la expansión del proyecto fueran mucho más sencillos.
- Se identificaron áreas de mejora, como:
  - Soporte para **HTTPS**
  - **Logging avanzado**
  - **Caché de respuestas**

---

## Conclusiones Personales

- Se han fortalecido las habilidades en **programación en C**, especialmente en el uso de **sockets y concurrencia**.
- La implementación de un **servidor web real** ayudó a comprender mejor el funcionamiento de **HTTP** y su interacción con los clientes.
- Se encontraron desafíos en la **gestión de memoria dinámica** y la **sincronización de hilos**, que se lograron superar a través de **pruebas** y **depuración**.
- Como recomendación para futuras prácticas, sería útil incluir una **fase de benchmarking** para evaluar el rendimiento del servidor bajo diferentes niveles de carga.
