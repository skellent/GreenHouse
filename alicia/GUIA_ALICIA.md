# ALICIA — Guía Completa de la Red Neuronal
## Skell's GreenHouse V3.0

---

## ¿Qué es Alicia?

Alicia es la inteligencia artificial del invernadero. Es una **red neuronal artificial de tipo Perceptrón Multicapa (MLP — Multi-Layer Perceptron)**, un modelo de aprendizaje automático supervisado que aprende patrones a partir de ejemplos. En este caso, aprende las reglas de control del invernadero (cuándo regar, qué intensidad de UV usar) a partir de datos de sensores.

El nombre "Alicia" fue elegido por el dueño del proyecto y es como se llamará a esta IA en todo el código y la documentación.

---

## Arquitectura de Alicia — Explicación Completa

### ¿Qué es un Perceptrón Multicapa?

Un MLP es una red de **neuronas artificiales** organizadas en capas. Cada neurona:
1. Recibe números de entrada.
2. Los multiplica por **pesos** (valores que la red aprende).
3. Suma los resultados más un **sesgo** (bias).
4. Aplica una **función de activación** que decide cuánto "se activa".

Las capas se conectan en cascada: la salida de una capa es la entrada de la siguiente.

---

### Estructura de Capas

```
        ENTRADA          OCULTA 1         OCULTA 2         SALIDA
      ┌──────────┐     ┌──────────┐     ┌──────────┐     ┌──────────┐
      │ Temp Amb │────▶│          │     │          │     │    UV    │
      │ Temp H₂O│────▶│  16 neur │────▶│  8 neur  │────▶│  (0–255) │
      │ Hum Amb  │────▶│  (ReLU)  │     │  (ReLU)  │     ├──────────┤
      │ Hum Suelo│────▶│          │     │          │     │  Riego   │
      │ Luz Ext  │────▶│          │     │          │     │ (bool)   │
      │ Perfil   │────▶│          │     │          │     └──────────┘
      └──────────┘     └──────────┘     └──────────┘
         6 nodos          16 nodos         8 nodos          2 nodos
```

---

### Capa de Entrada — 6 neuronas

Cada neurona corresponde a un sensor o parámetro:

| # | Variable              | Tipo original | Rango físico   | Normalizado |
|---|-----------------------|---------------|----------------|-------------|
| 0 | Temperatura ambiente  | float °C      | -10 a +50      | 0.0 – 1.0   |
| 1 | Temperatura del agua  | float °C      | 5 a 45         | 0.0 – 1.0   |
| 2 | Humedad ambiente      | float %       | 0 a 100        | 0.0 – 1.0   |
| 3 | Humedad del suelo     | float %       | 0 a 100        | 0.0 – 1.0   |
| 4 | Intensidad de luz     | int (0–100)   | 0 a 100        | 0.0 – 1.0   |
| 5 | Perfil de planta      | int (1/2/3)   | 1=desért, 3=alp| 0.0 – 1.0   |

**¿Por qué normalizar?** Las redes neuronales funcionan mejor cuando todos los valores de entrada están en el mismo rango (0 a 1). Sin normalización, una temperatura de 30°C dominaría sobre una humedad de 0.5 simplemente por diferencia de escala.

**Fórmula de normalización:**
```
valor_normalizado = (valor_real - mínimo) / (máximo - mínimo)
```

**Ejemplo:** Temperatura de 25°C con rango [-10, 50]:
```
(25 - (-10)) / (50 - (-10)) = 35 / 60 = 0.583
```

---

### Capa Oculta 1 — 16 neuronas, activación ReLU

Esta capa recibe los 6 valores normalizados y los transforma en 16 valores intermedios. Cada una de las 16 neuronas está conectada a las 6 entradas, lo que da:

```
Pesos: 6 entradas × 16 neuronas = 96 pesos
Sesgos: 16 (uno por neurona)
Total: 112 parámetros aprendibles
```

**¿Por qué 16 neuronas?** Es suficientemente grande para capturar las relaciones no lineales entre sensores (por ejemplo: "si hay mucha luz Y el suelo está seco Y el perfil es desértico → UV alto y no regar"), pero pequeño para ejecutarse eficientemente en el ESP32-S3.

**Función de activación ReLU (Rectified Linear Unit):**
```
f(x) = max(0, x)
```
Si la suma ponderada es negativa, la neurona devuelve 0 (no se activa). Si es positiva, devuelve el valor tal cual. Es la activación más usada en capas ocultas porque es simple y evita el problema de gradientes desvanecientes.

---

### Capa Oculta 2 — 8 neuronas, activación ReLU

Esta segunda capa toma los 16 valores de la capa anterior y los comprime a 8. Funciona como un "destilador" de información: elimina lo irrelevante y refuerza los patrones más importantes.

```
Pesos: 16 × 8 = 128 pesos
Sesgos: 8
Total: 136 parámetros aprendibles
```

**¿Por qué tener dos capas ocultas?** Una sola capa puede aprender relaciones simples. Con dos capas, la red puede aprender combinaciones de combinaciones, lo que permite capturar reglas más complejas como: "si el perfil es alpino Y la temperatura es alta Y la humedad del aire es baja, entonces regar incluso con suelo relativamente húmedo".

---

### Capa de Salida — 2 neuronas, activación Sigmoid

La última capa produce dos valores, uno por cada actuador:

| # | Salida             | Rango de salida | Uso en el ESP32-S3                    |
|---|--------------------|-----------------|---------------------------------------|
| 0 | Intensidad UV      | 0.0 – 1.0       | × 255 → uint8 para PWM (0–255)        |
| 1 | Riego              | 0.0 – 1.0       | > 0.5 → true (encender), ≤ 0.5 → false |

```
Pesos: 8 × 2 = 16 pesos
Sesgos: 2
Total: 18 parámetros aprendibles
```

**Función de activación Sigmoid:**
```
f(x) = 1 / (1 + e^(-x))
```
Comprime cualquier valor real al rango (0, 1). Perfecta para la capa de salida porque:
- Para UV: 0 = apagado, 1 = máxima intensidad.
- Para Riego: se interpreta como probabilidad, se umbraliza a 0.5.

---

### Resumen de Parámetros

```
Capa          Pesos    Sesgos   Subtotal
──────────────────────────────────────────
Oculta 1       96        16       112
Oculta 2      128         8       136
Salida         16         2        18
──────────────────────────────────────────
TOTAL                             266
```

266 parámetros es una red **extremadamente pequeña**. Para comparar, GPT-4 tiene ~1.8 billones de parámetros. La de Alicia cabe perfectamente en la PSRAM del ESP32-S3 con margen enorme.

---

## Lógica que Alicia Aprende

### Lógica UV

La regla central es: **más luz exterior = más UV** para forzar a la planta a crecer derecho (fototropismo controlado desde arriba).

| Condición                    | Comportamiento UV                               |
|------------------------------|-------------------------------------------------|
| Luz exterior alta (80–100%)  | UV alto (~200–255) — fuerza crecimiento recto   |
| Luz exterior media (40–80%)  | UV medio (~100–200)                             |
| Luz exterior baja (0–15%)    | UV mínimo por perfil (alpino=20, desért=40)     |
| Temperatura > 35°C           | UV reducido 15% para evitar estrés térmico      |
| Perfil desértico (×1.15)     | UV más intenso (estas plantas toleran más UV)   |
| Perfil regular (×1.00)       | UV estándar                                     |
| Perfil alpino (×0.75)        | UV reducido (plantas alpinas prefieren difuso)  |

### Lógica de Riego

| Condición                    | Decisión de Riego                                      |
|------------------------------|--------------------------------------------------------|
| Agua muy fría (<8°C)         | NO — choque térmico en raíces                          |
| Agua muy caliente (>40°C)    | NO — riesgo de quemadura radical                       |
| Suelo bajo umbral_seco       | SÍ siempre (desért:<25%, regular:<40%, alpino:<55%)    |
| Suelo sobre umbral_húmedo    | NO siempre (desért:>50%, regular:>65%, alpino:>80%)    |
| Zona intermedia + aire seco  | SÍ (mucha evaporación desde el follaje)                |
| Zona intermedia + mucho calor| SÍ (mucha evapotranspiración)                          |
| Zona intermedia + normal     | NO                                                     |

---

## Flujo de Desarrollo — Paso a Paso

### Prerrequisitos

```bash
# Crear entorno virtual (recomendado)
python -m venv venv
venv\Scripts\activate          # Windows
# source venv/bin/activate     # Linux/Mac

# Instalar dependencias
pip install -r requirements.txt
```

---

### Paso 1: Generar los Datos de Entrenamiento

```bash
cd alicia
python generar_datos.py
```

El script te preguntará los rangos de temperatura y humedad de tu invernadero. Usa los valores reales de tu ambiente para generar datos representativos.

**¿Qué genera?** Un archivo `datos_entrenamiento.csv` con N filas. Cada fila tiene 6 entradas (valores de sensores con algo de ruido gaussiano para realismo) y 2 salidas (UV e indicador de riego) calculadas con la lógica determinística.

**¿Por qué ruido gaussiano?** Los sensores reales no son perfectos. Añadir ruido pequeño (σ ≈ 0.4°C en temperatura, σ ≈ 0.8% en humedad) obliga a la red a aprender la regla en lugar de memorizar valores exactos, mejorando la generalización en uso real.

---

### Paso 2: Entrenar a Alicia

```bash
python entrenar_alicia.py
```

**¿Qué hace internamente?**

1. **Carga y normaliza** el CSV → tensores numpy.
2. **Divide** los datos: 80% entrenamiento, 20% validación (semilla fija=42 para reproducibilidad).
3. **Construye** el modelo Keras (6→16→8→2).
4. **Compila** con:
   - Optimizador: **Adam** (lr=0.001) — Adam ajusta automáticamente la tasa de aprendizaje durante el entrenamiento, convergiendo más rápido que SGD clásico.
   - Función de pérdida: **MSE (Error Cuadrático Medio)** — apropiada porque ambas salidas están normalizadas en [0,1]. Mide cuánto se equivoca la red en promedio.
5. **Entrena** hasta 300 épocas con dos callbacks:
   - **EarlyStopping**: detiene si `val_loss` no mejora en 15 épocas consecutivas y restaura los mejores pesos.
   - **ReduceLROnPlateau**: reduce la tasa de aprendizaje a la mitad si no mejora en 8 épocas.
6. **Evalúa** en el conjunto de validación y muestra métricas.
7. **Guarda** el modelo en `alicia_saved/` (formato SavedModel) y `alicia.keras`.

**¿Qué significa una época?** Una pasada completa por todos los datos de entrenamiento. Con 8000 muestras y batch_size=64, cada época ejecuta 125 actualizaciones de pesos.

**¿Qué esperar al terminar?**
- `val_loss` debería estar por debajo de 0.005
- Error promedio UV: menos de 5 puntos en escala 0–255
- Precisión de riego: >98%

---

### Paso 3: Convertir a TFLite

```bash
python convertir_alicia.py
```

**¿Por qué TFLite?** TensorFlow completo no cabe en un microcontrolador. TensorFlow Lite es una versión optimizada para dispositivos con recursos limitados. "Lite Micro" (TFLM) es la versión aún más reducida para MCUs sin sistema operativo.

**¿Qué hace el converter?**
1. Carga el SavedModel.
2. Aplica **cuantización dinámica** (`Optimize.DEFAULT`): comprime los pesos de float32 (4 bytes) a int8 (1 byte), reduciendo el tamaño del modelo ~75% sin requerir calibración. Las activaciones siguen siendo float32 en tiempo de ejecución.
3. Genera `alicia.tflite` — el modelo binario comprimido.
4. Genera `alicia_model.h` — el mismo contenido en formato C como un array de bytes.
5. Copia automáticamente el header a `../core/alicia_model.h`.

**¿Por qué un archivo .h?** El ESP32-S3 no tiene sistema de archivos convencional. La manera estándar de incluir datos estáticos (como un modelo de IA) es compilarlos directamente en el firmware como un array de bytes en memoria flash.

---

### Paso 4: Compilar y Flashear

**Librería Arduino requerida:**
En Arduino IDE → Herramientas → Administrar Librerías → buscar:
```
TensorFlow Lite Micro for Espressif Chips
```
o alternativamente:
```
Arduino_TensorFlowLite
```

**Configuración del board:**
- Board: `ESP32S3 Dev Module` (o la variante de tu board)
- PSRAM: `OPI PSRAM` (necesario para Octal 8MB)
- Partition Scheme: una con espacio suficiente para el firmware grande (ej. `Huge APP (3MB No OTA)`)
- Flash Size: `8MB`

**Compilar y subir:**
Abre `core/core.ino` en Arduino IDE y haz Upload.

---

### Paso 5: Compilar nervius

Abre `nervius/nervius.ino` en Arduino IDE.

**Pines de actuadores (nervius/config.h):**
| Pin | Función | Descripción |
|-----|---------|-------------|
| 25  | UV_PIN  | PWM 5kHz 8-bit → driver de LEDs UV |
| 26  | RIEGO_PIN | Digital → relé de bomba de agua |

Ajusta estos pines según tu circuito real si son distintos.

---

## Integración en el ESP32-S3

### Dónde vive Alicia en el firmware

```
core/
├── core.ino          ← Sketch principal (bucle UART + servidor HTTP)
├── alicia.h          ← Módulo de inferencia TFLite (nuevo)
├── alicia_model.h    ← Modelo compilado como array C (generado por Python)
├── config.h          ← Configuración de pines y WiFi
├── sensors.h         ← Struct Sensores (compartido entre núcleos)
├── cam.h             ← Módulo de cámara
└── icons.h           ← Íconos para LCD
```

### Flujo de ejecución en el S3

```
ESP32 (nervius) ──UART JSON──▶  ESP32-S3 (core)
                                    │
                              Deserializar JSON
                                    │
                              Actualizar sensores (mutex)
                                    │
                              Alicia::inferir(sensores)
                              ┌─────────────────────────┐
                              │  Normalizar 6 entradas   │
                              │  Invoke() — TFLite Micro │
                              │  Leer 2 salidas          │
                              └─────────────────────────┘
                                    │
                              uv = salida[0] × 255
                              riego = salida[1] > 0.5
                                    │
                              Actualizar sensores (mutex)
                                    │
                              Serializar [uv, riego, 0]
                                    │
ESP32 (nervius) ◀─UART JSON──  Enviar por UART
                    │
              ledcWrite(UV_PIN, uv)
              digitalWrite(RIEGO_PIN, riego)
```

### Uso de Memoria en el ESP32-S3

| Componente          | Memoria usada          | Ubicación |
|---------------------|------------------------|-----------|
| Modelo TFLite       | ~4–8 KB                | Flash     |
| Tensor Arena Alicia | 16 KB                  | PSRAM     |
| Buffer de cámara    | ~150 KB (VGA JPEG)     | PSRAM     |
| Servidor HTTP       | ~8 KB                  | SRAM      |
| Stack FreeRTOS      | ~8 KB por tarea        | SRAM      |
| Framework TFLite    | ~50–100 KB             | Flash     |

La PSRAM de 8MB Octal tiene espacio de sobra para la arena de tensores. La RAM interna (512KB) queda libre para el servidor HTTP, FreeRTOS y buffers de UART.

---

## Parámetros Clave del Modelo

| Parámetro           | Valor                        |
|---------------------|------------------------------|
| Tipo de red         | MLP Feedforward              |
| Entradas            | 6 neuronas                   |
| Capas ocultas       | 2 (16 → 8 neuronas)         |
| Salidas             | 2 neuronas                   |
| Activación oculta   | ReLU                         |
| Activación salida   | Sigmoid                      |
| Total parámetros    | 266                          |
| Optimizador         | Adam (lr=0.001)              |
| Función de pérdida  | MSE                          |
| Cuantización        | Dinámica (pesos int8)        |
| Arena de tensores   | 16 KB en PSRAM               |
| Tamaño del modelo   | ~4–8 KB (post-cuantización)  |
| Framework runtime   | TensorFlow Lite Micro        |

---

## Preguntas Frecuentes

**¿Puedo cambiar el número de neuronas?**
Sí. Si el error en UV o en riego es alto, puedes aumentar la capa oculta 1 a 32 neuronas. Recuerda re-entrenar y re-convertir. El arena de tensores necesitará ser mayor (aumenta `ALICIA_ARENA_KB` en `alicia.h`).

**¿Puedo re-entrenar con datos de mi invernadero real?**
Sí, es lo ideal. Puedes modificar `generar_datos.py` para cargar lecturas reales del CSV de tu invernadero y ajustar la lógica según tus observaciones.

**¿Qué pasa si el modelo falla en runtime?**
`Alicia::inferir()` retorna `false`. En ese caso, `core.ino` activa el fallback seguro: UV=0 y riego=false.

**¿Puedo agregar más salidas (como ventilación)?**
Sí. Añade una neurona más a la capa de salida, agrega la columna al CSV y re-entrena. En `alicia.h` lee `_output->data.f[2]` para la tercera salida.

**¿La IA aprende en tiempo real?**
No. Este es un modelo de **inferencia** únicamente. El aprendizaje ocurre offline en Python. El ESP32-S3 solo ejecuta el modelo pre-entrenado.
