# Skell's GreenHouse
> Desarrollado por: Skellent

## Resúmen del Proyecto

El proyecto es un invernadero autónomo modular inteligente para el cuidado y monitoreo autónomo de plantas sensibles, impulsado y controlado mediante una red neuronal entrenada.

## Sensores (Entradas)

1. Temperatura del Ambiente.
2. Temperatura del Tanque de Agua.
3. Humedad del Ambiente.
4. Humedad de La Tierra (Sensor dentro de la tierra).
5. Nivel de Iluminación Exterior.
6. Tipo de Clima Estimado (o "perfil")

## Actuadores (Salidas)

1. Luz Ultravioleta
2. Riego a la Planta

## Red Neuronal

La idea es que la red neuronal reciba las entradas de esos sensores y devuelva como resultado activar o desactivar los actuadores. La red debe tener en consideración el tipo de clima o "perfil" deseado para decidir en base al estado de la tierra y ambiente regar o no, a su vez decidir siempre la intensidad de las luces UV. Lo ideal es que mientras más luz haya alrededor, más intensa sea la luz UV para forzar a la planta a crecer recto; si hay mucho calor y dependiendo del perfil, regar o no, si la humedad del aire es baja, regar igualmente teniendo en cuenta la humedad de la tierra para no regar en exceso. En pocas palabras, hacer que la red tenga sentido común.

## Formatos de Entradas y Salidas Esperado

### Entradas:

1. Temperatura del Ambiente: float
2. Temperatura del Tanque: float
3. Humedad del Ambiente: float
4. Humedad de la Tierra: uint8 (0 - 100)
5. Nivel de Iluminación: uint8 (0 - 100)
6. Tipo de Clima: uint8 ( 1=desértico | 2=regular | 3=alpino )

### Salidas:

1. Luz Ultravioleta: uint8 (0 - 255) (Es para usar PWM en LEDs UV)
2. Riego: bool (Encendido, Apagado)

## Flujo de desarrollo de la Red Neuronal

La red neuronal debe ser ejecutada en una ESP32-S3 + OV2640 8MB Octal, No puede desbordar mucha memoria ya que también se encontrará corriendo en el núcleo 0 una API HTTP donde una de sus características es permitir tomar fotos que ocupan su debido espacio en memoria, a su vez también se encuentra en comunicación cíclica mediante UART con un ESP32 que ejecuta los actuadores, podrás entender esto cuando veas el código existente.

He indagado unas herramientas y basado en experiencias previas, el flujo es el siguiente:

1. Crear la Red Neuronal en TensorFlow con Python
2. Convertidor la Red Neuronal a formato ".h" para incluir la red en la compilación como "una variable"/"librería"
3. Usar TensorFlow Lite para correr la IA en el ESP32-S3

Respecto al entrenamiento, supongo yo que debe ser en formato .csv y que además debe ser de la siguiente manera:

- Debe ser un script creador del .CSV que me solicite el valor "máximo" y valor "mínimo", el resto de cálculos booleanos como la Bomba deberían ser por sentido común. Realmente no soy experto en entrenamiento pero tu debes usar la lógica, si es posible, generar tu mismo el archivo .CSV con el entrenamiento

Debes proporcionarme además una versión mejorada de los archivos.ino para que directamente estén listos para correr la IA.

### Flujo de Acciones en el S3 Esperado:

1. Recibe información UART (entrante).
2. Ejecuta la IA.
3. Retorna resultado mediante UART (salida).
4. Actualiza la pantalla LCD con unos cuantos sensores y con la salida de la IA.

## NOTAS IMPORTANTES

- Tienes que dejarme extremadamente documentado cómo fue la creación de la IA, cuántas neuronas, ocultas, entradas, salidas, tienes que explicarme TODO eso a fondo.
- Tienes que dejarme la explicación de la creación PASO a PASO con precisión y detalle.
- Ten en consideración los detalles técnicos que coexistirán con la IA.
- Nombra a la IA como "Alicia".
- Si necesitas más información, por favor pregúntame.
