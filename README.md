
# Skell's GreenHouse

Proyecto de invernadero inteligente y autónomo basado en ESP32, sensores ambientales y pantalla TFT. Permite el monitoreo y control de variables como temperatura, humedad, nivel de agua y crecimiento de plantas, con interfaz gráfica y servidor web integrado.

---

## Índice
1. [Descripción](#descripción)
2. [Hardware](#hardware)
3. [Software y Estructura de Archivos](#software-y-estructura-de-archivos)
4. [Flujo del Código](#flujo-del-código)
5. [Variables y Sensores](#variables-y-sensores)
6. [Instrucciones de Uso](#instrucciones-de-uso)
7. [Pantalla TFT y User_Setup.h](#pantalla-tft-y-usersetuph)
8. [Licencia](#licencia)

---

## Descripción
Skell's GreenHouse busca automatizar el cuidado de un invernadero, integrando sensores y actuadores para crear un entorno óptimo para el cultivo. Incluye elementos culturales en el diseño visual y una interfaz gráfica amigable.

## Hardware
- **ESP32 Dev Board**: Microcontrolador principal.
- **Pantalla TFT ILI9341 (SPI)**: Visualización de datos y GUI.
- **Sensores**:
	- DHT22: Temperatura y humedad del aire.
	- DS18B20: Temperatura del sistema.
	- Sensor de humedad de tierra.
	- Sensor de nivel de agua.
- **Actuadores**:
	- Luces UV (PWM, controladas por ESP32).
	- Bomba de agua.
- **[Esquema electrónico en KiCad](Chris/Esquema circuito/Esquema circuito.kicad_sch)**

## Software y Estructura de Archivos

El código está modularizado para facilitar su mantenimiento y extensión:

- **main.ino**: Punto de entrada. Inicializa hardware, sensores, red y ejecuta el bucle principal.
- **CONFIG.h / CONFIG_example.h**: Configuración de red WiFi y parámetros globales.
- **GUI.h / GUI.cpp**: Lógica de la interfaz gráfica en la pantalla TFT. Incluye iconos personalizados y marcos visuales.
- **MONITOR.h / MONITOR.cpp**: Define la página web embebida servida por el ESP32 para monitoreo remoto.
- **User_Setup.h**: Configuración específica de la pantalla TFT (pines, driver, frecuencia SPI). **Debe sobrescribir el archivo de la librería TFT_eSPI**.

### Estructura de carpetas

- `src/main/`: Código fuente principal y módulos.
- `Chris/Esquema circuito/`: Esquemas electrónicos en KiCad.
- `docs/`: Documentación web estática y recursos para GitHub Pages.

## Flujo del Código

El flujo principal del firmware sigue estos pasos:

```mermaid
flowchart TD
		A[Inicio: setup()] --> B[Inicialización de sensores y pantalla]
		B --> C[Configuración de red WiFi]
		C --> D[Loop principal]
		D --> E[Lectura de sensores]
		E --> F[Actualización de variables globales]
		F --> G[Actualización de GUI en TFT]
		F --> H[Actualización de servidor web]
		G --> D
		H --> D
		D -->|Evento| I[Control de actuadores (bomba, luces UV)]
		I --> D
```

### Detalle de cada etapa

- **setup()**: Inicializa la comunicación serie, sensores (DHT, DS18B20), pantalla TFT (TFT_eSPI), red WiFi y PWM para actuadores.
- **Loop principal**: Lee sensores periódicamente, actualiza variables globales, refresca la GUI en la pantalla y envía datos al servidor web embebido.
- **Control de actuadores**: Según los valores de sensores, activa/desactiva la bomba de agua y ajusta la intensidad de las luces UV mediante PWM.

## Variables y Sensores

Variables globales principales:

- `float Humedad_ambiental`: Humedad del aire (%).
- `float Temperatura_ambiental`: Temperatura del aire (°C).
- `int Humedad_Tierra`: Humedad del sustrato (%).
- `float temperatura_ESP32`: Temperatura interna del sistema (°C).
- `int brillo1, brillo2`: Intensidad de LEDs UV (PWM).

Sensores y pines:

- DHT22: `DHTPIN 16` (main.ino) o `22` (Proyecto.ino, ejemplo alternativo).
- DS18B20: `Sensor_Temp_ESP32 23`.
- Humedad de tierra: `SOIL_PIN 35`.
- Nivel de agua: `WATER_PIN 34`.
- Luces UV: `LUZ_UVA 25`, `LUZ_UVB 27`.

## Instrucciones de Uso
1. Clona o descarga este repositorio.
2. Abre el proyecto en Arduino IDE o PlatformIO.
3. Copia y edita `src/main/CONFIG_example.h` como `CONFIG.h` para tus credenciales WiFi.
4. **Reemplaza el archivo `User_Setup.h` de la librería TFT_eSPI por el de `src/User_Setup.h`** para asegurar la correcta configuración de la pantalla.
5. Compila y sube el firmware al ESP32.
6. Accede al servidor web desde la IP mostrada en la pantalla TFT.

## Pantalla TFT y User_Setup.h

El archivo `User_Setup.h` es fundamental para la correcta comunicación con la pantalla ILI9341. Debe sobrescribir el archivo original de la librería TFT_eSPI instalada. Configura los pines SPI y la frecuencia según el hardware especificado.

**Configuración relevante:**

- Pines SPI: `MISO=19`, `MOSI=23`, `SCLK=18`, `CS=15`, `DC=2`, `RST=4`, `TOUCH_CS=21`
- Frecuencia SPI: `27000000` Hz
- Driver: `ILI9341`

## Detalle de Archivos Clave

- **main.ino**: Orquesta la inicialización y el ciclo principal. Incluye comentarios para cada sección de sensores y actuadores.
- **GUI.cpp/h**: Define la interfaz visual, iconos personalizados (gota, aire, temperatura, tierra, agua), y el método `marco()` para los marcos verdes redondeados.
- **MONITOR.h/cpp**: Define la web embebida, con estilos CSS responsivos y actualización periódica vía JavaScript (`fetch('/data')`).
- **CONFIG_example.h**: Plantilla para la configuración de red WiFi y parámetros globales.
- **User_Setup.h**: Configuración de la pantalla TFT, pines, driver, fuentes y frecuencias. **¡No olvidar reemplazar el original de la librería!**

## Licencia
Este proyecto está bajo la Licencia MIT. Consulta el archivo LICENSE para más detalles.
