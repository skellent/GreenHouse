# Skell's GreenHouse V3.0 — Informe Técnico ORC
**Olimpiadas de Robótica Creativa · Competencia Ingenio Creativo**

---

## PORTADA

**Olimpiadas de Robótica Creativa (ORC)**
**COMPETENCIA INGENIO CREATIVO**
**INFORME TÉCNICO**

*(Logo de Skellent)*

**Skellent**
**Skell's GreenHouse V3.0**

**Integrantes:**
- Robert Rodríguez, V-32.942.787, 17 Años.
- Christopher Ramírez, V-33.499.939, 16 Años.
- Fabiana Hernández, V-34.648.529, 14 Años.

**Tutor:** Jesús Pérez, V-30.759.313

Barquisimeto, Mayo 2026

---

## ÍNDICE

- RESÚMEN DE LA IDEA DE PROYECTO
- INTEGRANTES DEL EQUIPO DE PROYECTO
- IMPLEMENTACIÓN DEL PROYECTO
  - INICIO
  - DESARROLLO
    - PRINCIPALES DIFICULTADES Y SOLUCIONES
    - MATERIALES DEFINITIVOS
    - PROGRAMACIÓN
    - ALICIA — INTELIGENCIA ARTIFICIAL
    - ASPECTOS MECÁNICOS Y FUNCIONAMIENTO
  - ALGORITMO DE CONTROL
  - ESTRUCTURA DEL CÓDIGO
  - EJECUCIÓN
    - EVALUACIÓN DEL DESEMPEÑO
    - FORTALEZAS DEL SISTEMA
    - OPORTUNIDADES DE MEJORA
- EVALUACIÓN Y ANÁLISIS DEL PROYECTO
  - LOGROS OBTENIDOS
  - APRENDIZAJES
  - ESCENARIOS DE ESCALADO
- APLICACIÓN MÓVIL
- ESQUEMA ELECTRÓNICO
- ESQUEMA DE FLUJO ELÉCTRICO
- ANEXOS DE CÓDIGO
- REFERENCIAS BIBLIOGRÁFICAS
- FOTOGRAFÍAS

---

## RESÚMEN DE LA IDEA DE PROYECTO

Skell's GreenHouse es un sistema de invernadero autónomo, portátil y modular, diseñado para facilitar el cultivo y la adaptación de plantas sensibles en condiciones ambientales desfavorables. Su propósito central es reproducir el entorno ideal de cualquier planta — independientemente del clima externo — para inducir de forma gradual su adaptación al ambiente real, aumentando la producción agrícola sin depender de temporadas ni zonas clásicas.

El corazón del sistema es Alicia, una red neuronal perceptrón multicapa (MLP) entrenada en Python con TensorFlow y desplegada mediante TensorFlow Lite Micro sobre un ESP32-S3 con 8 MB de PSRAM Octal. Alicia procesa seis variables (temperatura del ambiente, temperatura del agua, humedad del ambiente, humedad del suelo, intensidad de luz y perfil de planta) y decide automáticamente la intensidad de los LEDs UV y si se debe activar el riego. La arquitectura distribuida usa un ESP32-S3 como cerebro (IA + API HTTP + cámara OV2640) y un ESP32 común como nodo de sensores y actuadores; ambos se comunican mediante UART con protocolo JSON. Una pantalla LCD 16x2 I2C muestra en tiempo real los datos de los sensores y las decisiones de la IA.

Las macetas están equipadas individualmente con sensores FC-28, lo que permite intercambiarlas fácilmente sin recableado. El sistema funciona con 5 V / 3 A, con conversión interna a 3.3 V. Una aplicación móvil desarrollada en JavaScript se conecta por WiFi a la IP local del ESP32-S3 y permite monitorear todos los parámetros, ver la cámara en tiempo real y configurar el perfil de planta.

El proyecto se enmarca en los Motores Agroalimentario y Ciudades Inteligentes. En el motor agroalimentario, GreenHouse permite cultivar hortalizas en cualquier zona climática mediante la recreación del entorno ideal y la posterior adaptación gradual, eliminando la dependencia de temporadas e impulsando la soberanía alimentaria. En el motor de Ciudades Inteligentes, representa una solución accesible para ciudadanos que desean sembrar en casa sin conocimientos agronómicos ni tiempo para el cuidado manual. Las pruebas realizadas con cebollín en el hogar de Robert Rodríguez demostraron crecimiento continuo, riego preciso y monitoreo confiable.

---

## INTEGRANTES DEL EQUIPO DE PROYECTO

El equipo Skellent se formó a partir de la amistad y el interés compartido por la robótica y la electrónica. Robert y Christopher se conocen de proyectos previos; Robert y Fabiana estudian en el mismo colegio. Al identificar la problemática del cuidado de plantas en sus hogares, decidieron unir sus habilidades para crear una solución tecnológica real. Cada integrante asumió roles específicos basados en sus fortalezas:

- **Robert Rodríguez (17 años):** Programación del ESP32-S3 y ESP32 Nervius, entrenamiento de la red neuronal Alicia en Python, desarrollo de la aplicación móvil e integración de TFLite Micro.
- **Christopher Ramírez (16 años):** Diseño mecánico, electrónico y estructural. Encargado del diseño de la carcasa, el sistema modular de macetas y la impresión 3D de componentes.
- **Fabiana Hernández (14 años):** Electrónica y documentación. Supervisó el conexionado de sensores y actuadores, gestionó las pruebas y el registro fotográfico.

---

## IMPLEMENTACIÓN DEL PROYECTO

### INICIO

La idea de Skell's GreenHouse nació de un proyecto anterior a menor escala: un medidor de datos de maceta. Al observar que plantas en sus hogares morían por falta de atención o por condiciones desfavorables, los integrantes ampliaron el concepto hacia un sistema autónomo completo. Antes de iniciar, investigaron invernaderos comerciales — costosos, no portátiles, sin IA ni monitoreo remoto — y trabajos académicos sobre agricultura de precisión con ESP32, lo que confirmó la viabilidad del enfoque.

Al inicio los materiales eran limitados: una placa ESP32, sensores básicos, una bomba y cartón para el primer boceto. La evolución hasta V3.0 requirió agregar el ESP32-S3 con cámara, distribuir la arquitectura en dos microcontroladores y diseñar el sistema de macetas modulares con FC-28 integrado.

### DESARROLLO

El desarrollo se organizó en fases:

- Definición de requerimientos.
- Diseño conceptual y selección de componentes.
- Prototipado electrónico.
- Construcción mecánica.
- Programación y pruebas.
- Generación del dataset y entrenamiento de Alicia.
- Iteración final e integración.

Se realizaron dos iteraciones estructurales: una versión con cartón para validar medidas y la versión final con PVC expandido y marcos de aluminio. La tercera gran iteración (V3.0) introdujo la arquitectura de dos ESP32, la red neuronal Alicia y las macetas con FC-28 integrado.

#### PRINCIPALES DIFICULTADES Y SOLUCIONES

- **Sincronización UART:** Coordinar el ciclo de lectura del Nervius con el ciclo de inferencia del Core requirió definir un protocolo JSON delimitado por salto de línea y buffers protegidos por mutex FreeRTOS.
- **Memoria en el ESP32-S3:** TFLite Micro, la cámara OV2640 y el servidor HTTP comparten la PSRAM. Se asignó el tensor arena de 16 KB en PSRAM (MALLOC_CAP_SPIRAM) mediante heap_caps_malloc(), liberando la RAM interna para el servidor.
- **Configuración del TFLite:** Se utilizó la librería TensorFlow Lite Micro para ESP32. Requirió múltiples pruebas para estabilizar la resolución del resolver de operadores y lograr la inferencia correcta.
- **Suministro de Agua:** Inicialmente la bomba no succionaba bien. Se reubicó en la parte baja del tanque y se aumentó la altura de la manguera.
- **Modularidad:** Las macetas llevan el FC-28 integrado, lo que permite intercambiarlas sin desconectar cables internos. El diseño mecánico y electrónico prioriza el fácil acceso y reemplazo de componentes.

#### MATERIALES DEFINITIVOS

- **Estructura:** Láminas de PVC expandido, marcos de aluminio, piezas impresas en 3D (PLA-PETG).
- **Electrónica:** ESP32-S3 + OV2640 (8 MB PSRAM Octal), ESP32 común (Nervius), sensor DHT22 (temperatura y humedad del aire), FC-28 ×2 (humedad de suelo), HC-SR04 ultrasónico (altura de planta), fotoresistencia (luz exterior), bomba de agua DC 5 V, LEDs UV ×2, pantalla LCD 16x2 I2C (0x27), fuente conmutada 5 V 3 A, módulo reductor LM2596 (3.3 V), botón, baquelita, resistencias, transistores.
- **Conexiones:** Cables DuPont, conectores terminales para cargas de alto consumo.

#### PROGRAMACIÓN

Se utilizó Arduino C++ en el Arduino IDE tanto para el ESP32-S3 (core.ino) como para el ESP32 Nervius (nervius.ino), aprovechando las librerías WiFi, WebServer, ArduinoJson, LiquidCrystal_I2C y TensorFlow Lite Micro para Espressif. FreeRTOS gestiona las dos tareas paralelas: el bucle principal en núcleo 1 (lectura UART + inferencia IA) y el servidor HTTP en núcleo 0.

La red neuronal Alicia se creó con Python usando TensorFlow/Keras: un script genera el dataset sintético (generar_datos.py), otro entrena el modelo (entrenar_alicia.py) y un tercero lo convierte a TFLite e incluye el array de bytes como cabecera C (convertir_alicia.py). La aplicación móvil se desarrolló en JavaScript, consumiendo la API JSON del ESP32-S3.

#### ALICIA — INTELIGENCIA ARTIFICIAL

Alicia es una red neuronal perceptrón multicapa (MLP) diseñada para correr en microcontrolador con recursos limitados. Su arquitectura es:

- **Capa de entrada:** 6 neuronas — temp_ambiente, temp_agua, hum_ambiente, hum_suelo, intensidad_luz, perfil_planta (todas normalizadas a [0, 1]).
- **Capa oculta 1:** 16 neuronas, activación ReLU.
- **Capa oculta 2:** 8 neuronas, activación ReLU.
- **Capa de salida:** 2 neuronas, activación Sigmoid — salida[0] × 255 = intensidad UV (uint8), salida[1] > 0.5 = activar riego (bool).
- **Total de parámetros:** 266 (pesos + sesgos).

El entrenamiento se realizó con 5.000 muestras sintéticas generadas por generar_datos.py con lógica determinista: "mientras más luz exterior, mayor intensidad UV para forzar fototropismo desde arriba"; el riego depende de umbrales de humedad de suelo y ambiente según el perfil de planta (desértico, regular, alpino). Se añade ruido gaussiano para robustez.

Configuración de entrenamiento: optimizador Adam (lr = 0.001), pérdida MSE, máximo 300 épocas, EarlyStopping (paciencia 15) y ReduceLROnPlateau (factor 0.5, paciencia 8). División 80/20 entrenamiento/validación.

Para el despliegue, el modelo se convierte a formato TFLite con cuantización dinámica (Optimize.DEFAULT), reduciéndolo a ~4–8 KB. Se exporta como array de bytes en alicia_model.h. En el ESP32-S3, TFLite Micro carga el modelo usando un tensor arena de 16 KB reservado en PSRAM, registrando solo las operaciones necesarias (FullyConnected, Relu, Logistic) para minimizar el uso de memoria.

#### ASPECTOS MECÁNICOS Y FUNCIONAMIENTO

El sistema se divide electrónicamente en dos nodos:

- **ESP32-S3 (Core):** Cerebro del sistema. Corre la IA Alicia en núcleo 1, sirve la API HTTP y la cámara en núcleo 0, gestiona la pantalla LCD 16x2 y recibe el botón de configuración.
- **ESP32 Común (Nervius):** Nodo de sensores y actuadores. Lee DHT22, FC-28 ×2, HC-SR04 y fotoresistencia; recibe las órdenes de UV (PWM LEDC) y riego (digital) y las ejecuta físicamente.

Mecánica-estructuralmente se divide en:

- **Marco de aluminio:** Estructura base que da fuerza y estabilidad.
- **Placas de PVC expandido:** Aíslan los componentes y la planta del exterior, protegiéndolos del agua, luz y calor.
- Puerta de observación con sistema de imanes.
- **Macetas modulares:** Cada maceta lleva integrado su FC-28, permitiendo intercambiarla sin recableado.

### ALGORITMO DE CONTROL

**El Nervius (ESP32 común) cada 2 segundos:**
- Lee sensores (DHT22, FC-28 ×2, HC-SR04, fotoresistencia).
- Envía JSON por UART: `[temp_amb, temp_agua, hum_amb, hum_suelo, luz, ultrasonido]`.
- Recibe JSON de respuesta: `[uv(0-255), riego(bool), ventilacion(0)]`.
- Ejecuta actuadores: `ledcWrite(UV_LEDC_CANAL, uv)` y `digitalWrite(RIEGO_PIN, riego)`.

**El Core (ESP32-S3, Núcleo 1) en su bucle principal:**
- Lee el JSON entrante por UART y actualiza la estructura Sensores (con mutex).
- Llama a `Alicia::inferir()` con los datos del sensor.
- **Control de Actuadores:** Escala la salida[0] × 255 = uv_out (uint8), salida[1] > 0.5 = riego_out. Envía resultados al Nervius por UART.
- Actualiza la pantalla LCD con los sensores y las decisiones de la IA.

**El API HTTP (Núcleo 0) en paralelo:**
- Gestiona la conexión WiFi de forma independiente.
- Expone `GET /api/datos` (JSON), `GET /api/foto` (JPEG OV2640), `POST /api/perfil` y `POST /api/wifi`.

### ESTRUCTURA DEL CÓDIGO

- **core.ino:**
  - `setup()`: inicializa LCD, pines, UART, WiFi, cámara, Alicia y delega ServidorWeb() a núcleo 0 con xTaskCreatePinnedToCore.
  - `loop()`: lee JSON UART del Nervius, llama Alicia::inferir(), responde por UART y actualiza LCD.
  - `servidorWeb()`: bucle infinito en núcleo 0, maneja peticiones HTTP y sirve JSON de sensores y captura de cámara.
- **nervius.ino:**
  - Configura LEDC (PWM para LEDs UV) y pines de actuadores.
  - Lee sensores cada 2 s, envía JSON al Core y ejecuta los actuadores según la respuesta.
- **alicia.h:**
  - Encapsula `Alicia::init()` (carga del modelo en PSRAM) y `Alicia::inferir()` (normalización + Invoke() + post-procesado).
- **Scripts Python (directorio alicia/):**
  - `generar_datos.py` → `entrenar_alicia.py` → `convertir_alicia.py`. Flujo completo de generación de dataset, entrenamiento y exportación a `alicia_model.h`.

### EJECUCIÓN

El prototipo V3.0 se probó en el hogar de Robert Rodríguez. Se instaló una maceta con cebollín en crecimiento. La red neuronal Alicia tomó el control automático: ajustó la intensidad UV en función de la luz exterior y activó el riego según la humedad del suelo y el perfil configurado. Los resultados mostraron crecimiento continuo y saludable, con el cebollín desarrollando ramas más gruesas durante su crecimiento, consistente con la mutación de adaptación buscada.

#### EVALUACIÓN DEL DESEMPEÑO

El sistema se consideró óptimo cuando se cumplieron tres criterios:

- **Crecimiento Continuo:** la planta demostró un crecimiento continuo, saludable y con desarrollo mecánico más robusto que en condiciones naturales.
- **Precisión en UV y Riego:** Alicia activó la bomba solo cuando la humedad del suelo descendió por debajo del umbral del perfil activo, sin inundaciones ni sequías, y reguló los LEDs UV con intensidad proporcional a la luz exterior.
- **Confiabilidad de la aplicación:** los datos mostrados en la app coincidían con mediciones manuales realizadas con instrumentos de referencia.

#### FORTALEZAS DEL SISTEMA

- **Autonomía Total mediante la IA:** no requiere intervención humana para mantener las condiciones adecuadas (solo recarga ocasional del tanque de agua).
- **Perfiles configurables:** el usuario puede seleccionar el perfil de planta (desértico, regular, alpino) desde la app o el botón, y Alicia adapta su comportamiento automáticamente.
- **Modularidad:** permite cambiar de cultivo fácilmente; diseñado electrónica y estructuralmente para ser reparable y escalable.
- **Monitoreo Remoto:** la app facilita el monitoreo desde cualquier dispositivo en la misma red WiFi.
- **Portabilidad:** tamaño y peso permiten trasladarlo sin dificultad.

#### OPORTUNIDADES DE MEJORA

- Implementar sensores de pH y nutrientes para decisiones más finas.
- Añadir un sistema de recolección de agua mediante placas Peltier.
- Integrar el DS18B20 para medir temperatura real del agua.
- Desarrollar una base de datos de plantas con parámetros predefinidos.
- Optimizar el consumo energético mediante modos de suspensión del ESP32.

---

## EVALUACIÓN Y ANÁLISIS DEL PROYECTO

Skell's GreenHouse atiende simultáneamente dos motores productivos. En el Motor Agroalimentario, la capacidad del sistema para recrear cualquier entorno ideal permite cultivar hortalizas y plantas sensibles en zonas climáticamente inapropiadas, reduciendo la dependencia de temporadas y ampliando la superficie cultivable efectiva sin requerir hectáreas de tierra. Al arriesgar un brote para conseguir una línea adaptada, se obtiene material genético resistente al entorno local, con impacto directo en la seguridad alimentaria del estado Lara.

En el Motor de Ciudades Inteligentes, GreenHouse es una solución accesible para ciudadanos comunes que desean cultivar en casa sin conocimientos agronómicos ni tiempo disponible para el cuidado manual. Su diseño compacto, modular y autónomo encaja en apartamentos, oficinas y espacios educativos, fomentando la alfabetización tecnológica y agrícola desde el nivel doméstico.

### LOGROS OBTENIDOS

- Se consolidó un equipo interdisciplinario con roles claros y comunicación efectiva entre programación, electrónica y mecánica.
- Se logró un prototipo completamente funcional: dos ESP32 coordinados por UART, IA corriendo en tiempo real en PSRAM y aplicación móvil operativa.
- Alicia aprendió satisfactoriamente la lógica de control UV/riego y la generaliza ante entradas reales no vistas durante el entrenamiento.
- Se documentó el proceso completo, incluyendo guía de entrenamiento (GUIA_ALICIA.md) y scripts de Python.

### APRENDIZAJES

- La modularidad es crítica: facilita pruebas, mantenimiento y escalabilidad futura.
- El manejo de memoria en sistemas embebidos requiere planificación: la separación de tareas entre PSRAM y RAM interna fue clave para que cámara, IA y servidor coexistan.
- El entrenamiento sintético con lógica determinista permite crear datasets etiquetados sin necesidad de experimentos reales, acelerando el ciclo de desarrollo.
- La separación en dos microcontroladores simplifica el código de cada uno y hace el sistema más robusto ante fallos individuales.

### ESCENARIOS DE ESCALADO

- **Escala educativa:** versión en kit de componentes para escuelas, con manuales didácticos.
- **Escala comunal:** módulo más grande para cultivo de múltiples macetas en espacios comunitarios.
- **Escala comercial:** reducción de costos mediante diseño optimizado para producción en serie, manteniendo la modularidad.

---

## APLICACIÓN MÓVIL

Se desarrolló una aplicación móvil en JavaScript que se conecta mediante WiFi local a la IP del ESP32-S3. La aplicación permite:

- Monitorear en tiempo real todos los sensores (temperatura, humedad, luz, ultrasonido, estado de la cámara).
- Ver las decisiones actuales de Alicia (intensidad UV y estado del riego).
- Visualizar capturas de la cámara OV2640 en tiempo real.
- Configurar el perfil de planta (1=Desértico, 2=Regular, 3=Alpino) mediante `POST /api/perfil`.
- Cambiar las credenciales WiFi del ESP32-S3 mediante `POST /api/wifi`.

---

## ESQUEMA ELECTRÓNICO

*(Insertar imagen del esquema electrónico — image2.jpg)*

---

## ESQUEMA DE FLUJO ELÉCTRICO

*(Insertar imagen del esquema de flujo — image3.jpg)*

---

## ANEXOS DE CÓDIGO

### Anexo A — `loop()` en core.ino (ESP32-S3, Núcleo 1)

Bucle principal que recibe datos por UART del Nervius, ejecuta Alicia y devuelve las decisiones:

```cpp
void loop() {
  neopixelWrite(PIN_RGB, 0, 0, 255); // Indicador de iteración
  actualizarLCD();
  bufferEntrante = "";

  // Lee JSON entrante por UART (enviado por el Nervius)
  while (nervios.available()) {
    char c = nervios.read();
    if (c == '\n') {
      JsonDocument datos;
      if (!deserializeJson(datos, bufferEntrante)) {
        if (xSemaphoreTake(mutexDatos, pdMS_TO_TICKS(100)) == pdTRUE) {
          sensores.sen_temperatura_ambiente = datos[0];
          sensores.sen_temperatura_agua     = datos[1];
          sensores.sen_humedad_ambiente     = datos[2];
          sensores.sen_humedad_suelo        = datos[3];
          sensores.sen_intensidad_luz       = datos[4];
          sensores.sen_ultrasonido          = datos[5];
          xSemaphoreGive(mutexDatos);
        }
        // ── Ejecución de Alicia ──
        int  uv_ia = 0; bool riego_ia = false;
        if (Alicia::inferir(sensores, uv_ia, riego_ia)) {
          sensores.ia_intensidad_uv = uv_ia;
          sensores.ia_riego        = riego_ia ? 1 : 0;
          ejecutarActuadores(uv_ia, riego_ia, 0);
        } else {
          ejecutarActuadores(0, false, 0); // fallback seguro
        }
      }
      bufferEntrante = "";
    } else { bufferEntrante += c; }
  }
  vTaskDelay(pdMS_TO_TICKS(200));
}
```

### Anexo B — `inferir()` en alicia.h

Función que normaliza las entradas, ejecuta el intérprete TFLite y post-procesa las salidas:

```cpp
bool inferir(const Sensores& s, int& uv_out, bool& riego_out) {
  if (!_listo) return false;
  // Normalizar las 6 entradas a [0, 1]
  _input->data.f[0] = _norm(s.sen_temperatura_ambiente, -10.0f, 50.0f);
  _input->data.f[1] = _norm(s.sen_temperatura_agua,      5.0f, 45.0f);
  _input->data.f[2] = _norm(s.sen_humedad_ambiente,      0.0f,100.0f);
  _input->data.f[3] = _norm(s.sen_humedad_suelo,         0.0f,100.0f);
  _input->data.f[4] = _norm(s.sen_intensidad_luz,        0.0f,100.0f);
  _input->data.f[5] = _norm((float)s.ia_perfilplanta,    1.0f,  3.0f);
  // Ejecutar la inferencia
  if (_interpreter->Invoke() != kTfLiteOk) return false;
  // Escalar salidas: [0]=UV (float→uint8), [1]=riego (sigmoid > 0.5)
  uv_out    = (int)(_output->data.f[0] * 255.0f);
  riego_out = _output->data.f[1] > 0.5f;
  return true;
}
```

### Anexo C — Lógica determinista en generar_datos.py (Python)

Funciones que definen las reglas que Alicia aprende durante el entrenamiento:

```python
def calcular_uv(luz, perfil, temp_amb):
    multiplicadores = {1: 1.15, 2: 1.0, 3: 0.75}
    uv = luz * 2.55 * multiplicadores[perfil]
    if temp_amb > 35.0:
        uv *= 0.85                       # alivio térmico
    if luz < 15.0:                       # mínimo nocturno
        minimos = {1: 40, 2: 30, 3: 20}
        uv = max(uv, float(minimos[perfil]))
    return int(np.clip(uv, 0, 255))

def calcular_riego(temp_amb, temp_agua, hum_amb, hum_suelo, perfil):
    if temp_agua < 8.0 or temp_agua > 40.0:
        return False                     # proteger raíces
    config = {
        1: {'seco':25, 'humedo':50, 'amb_seco':25, 'temp_calor':35},
        2: {'seco':40, 'humedo':65, 'amb_seco':40, 'temp_calor':30},
        3: {'seco':55, 'humedo':80, 'amb_seco':55, 'temp_calor':25},
    }[perfil]
    if hum_suelo < config['seco']:        return True
    if hum_suelo > config['humedo']:      return False
    if hum_amb   < config['amb_seco']:    return True
    if temp_amb  > config['temp_calor']:  return True
    return False
```

---

## REFERENCIAS BIBLIOGRÁFICAS

### Fundamentos de Invernaderos Inteligentes y Agricultura de Precisión

Heuvelink, E., Hemming, S., & Marcelis, L. F. M. (2025). Some recent developments in controlled-environment agriculture. *The Journal of Horticultural Science and Biotechnology*, 100(5), 604–614. https://doi.org/10.1080/14620316.2024.2440592

Kim, J., Lee, H., & Park, S. (2025). Multi-Sensor Monitoring, Intelligent Control, and Data Processing for Smart Greenhouse Environment Management. *Sensors*, 25(19), 6134. https://doi.org/10.3390/s25196134

### Internet de las Cosas (IoT) y Monitoreo Remoto

Kim, J., Lee, H., & Park, S. (2025). Multi-Sensor Monitoring, Intelligent Control, and Data Processing for Smart Greenhouse Environment Management. *Sensors*, 25(19), 6134. https://doi.org/10.3390/s25196134 (Discute protocolos WiFi y plataformas IoT para automatización.)

---

## FOTOGRAFÍAS

*(Insertar fotografías del proyecto)*
