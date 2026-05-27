# Skell's GreenHouse
> Desarrollado por: Skellent

## Prompt Usado Anteriormente
> Este prompt contiene información relevante sobre cómo funciona el proyecto a nivel técnico de Software, más no de Hardware, úsalo solo para poder explicar las partes de Software solicitadas en el informe.

Prompt:

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

---

## Información Adicional
> Hemos estado preparándonos para posibles preguntas, a su vez, estas preguntas están relacionadas o podrían salir en el informe, así que puedes usar las respuestas que hemos preparado para responder las presentes en el informe.

### Preguntas:

🌱 CONCEPTO DEL PROYECTO
- ¿Qué es?: Un invernadero autónomo modular inteligente para el cuidado y monitoreo autónomo de plantas sensibles impulsado mediante una red neuronal entrenada.

- ¿Cómo?: Mediante un ESP32-S3 como cerebro y ejecutor de la IA y cámara, se comunica con otro ESP32 Común dedicado a la lectura de datos y ejecución de actuadores.

- ¿Por qué?: Para aumentar la producción de plantas en condiciones desfavorables para las mismas con la intención de hacerlas mutar y adaptarlas al entorno deseado. Arriesgas un brote a cambio de una hectárea adaptada al ambiente.

- ¿Para?: Para aumentar la producción industrial de hectáreas de hortalizas que rechazan el ambiente actual mediante su adaptación con GreenHouse.

- ¿Cuándo?: El proceso de mutación depende del tipo de planta, el sujeto más exitoso ha sido un cebollín fino que comenzó a desarrollar ramas más gruesas durante su crecimiento.

- ¿Dónde?: Dentro del invernadero en un entorno cálido, específicamente el patio de Robert Rodríguez. en un clima semiárido (Lara)

- ¿Quiénes?: Los creadores son Robert Rodríguez, Christopher Ramírez y Fabiana Hernández.

🗣️ DEFENSA (No Redactadas Aún)
- Fabiana: blah.
- Chris: blah.
- Robert: blah.

⁉️ PREGUNTAS
- ¿Por qué una Red Neuronal y no una Lógica difusa?: Porque la Lógica Difusa requiere de programación adicional y reconfiguración general cuando se anexa un tipo de planta, mientras que la Red Neuronal es capaz de entrenarse manual o autónomamente permitiéndole adaptarse a cualquier caso que se presente.

- ¿Qué hace la IA?: Controlar y regular los actuadores para eo cuidado autónomo de la planta, la IA toma decisiones en base a los diversos sensores internos.

- ¿Cuántas capas tiene? (La IA): <INDEFINIDO AÚN>

- ¿Cómo logran mutar la planta?: Los diversos actuadores crean el entorno ideal para la planta y luego gradualmente van cambiando el entorno al deseado de forma pasiva para adaptar la planta.

- ¿Por qué Skellent?: Skellent es un juego de palabras entre Skellingtong y Enterprise, de ahí la abreviatura a "Skell's" en el nombre del proyecto.

- ¿Cuánto voltaje y amperaje consume?: 5 Voltios y 3 Amperios, aunque dentro del circuito hay un conversor a 3.3 Voltios.

- ¿La planta tiene que ser trasplantada?: No necesariamente, si se trata de una pequeña esta puede sobrevivir toda su vida dentro. Si se trata de por ejemplo la plántula de una planta de platano, surtiera efecto la mutación pero inevitablemente deberá ser transplantada.

- ¿Qué tipos o cuáles plantas podría plantar?: Idealmente pequeñas que no excedan el tamaño del invernadero, sin embargo, cualquier planta independientemente de su "habitad" o "climatología" o "tamaño inicial"puede sobrevivir.

- ¿En que motor se ubica su proyecto? ¿Por qué?: En el agroalimentario ya que permite el cultivo de plantas en cualquier zona mediante la mutación de la planta, ahorrando costos en hectáreas y permitiendo una independencia agropecuaria respecto al clima o temporada. También entra en Ciudades Inteligentes porque puede ser usado por personas comunes que quieren sembrar sus propias hortalizas pero no disponen de tiempo para su cuidado, inclusive para plantas decorativas en formación.

- ¿Por qué rediseñaron la "casa"?: Este diseño filtra mejor la luz y la temperatura, evitando fluctuaciones internas ocasionadas por el exterior

- Si quiero sembrar XPlanta, ¿Debo estar en temporada o ambiente apropiado? ¿Por qué?: No necesariamente, el invernadero por dentro recreará las condiciones apropiadas de la temporada, permitiendo sembrarla y posteriormente cultivarla.

- ¿En dónde / con qué programaron el proyecto?: Las ESP32 con C/C++ en ArduinoIDE y la red neuronal con TensorFlow en Python entre otras librerías.

---

## Aplicación Móvil

Desarrollamos una aplicación móvil que se conecta mediante una Red WiFi Local al ESP32 mediante su IP, permite monitorear todos los sensores y actuadores así como configurar: Perfil de Planta/Clima, Red WiFi a la que se conecte el ESP32-S3

## Motores Mencionados

Vas a investigar tu mismo en La Internet a qué se refiere el informe con respecto a esos motores y en base al proyecto, redactar lo necesario.

## Apartado de Experiencias

Vas a extraer del informe original el apartado de experiencias y lo vas a modificar para que coincida con la versión actual del proyecto.

## Hardware

- Si en el informe original se menciona una pantalla TFT, debes saber que esta fue descartada y remplzada por una LCD 16x2 I2C, muestra datos de los sensores, decisiones de la IA y mediante un botón, mostrar la información de la RED en la que está conectada, si en el original no se muestra, no menciones la TFT y enfocate en la LCD respecto a cuando la menciones.

- Antes solo usabamos un ESP32-S3, ahora usamos un ESP32-S3 que ejecuta la IA, la API, la cámara y controlar la LCD y recibir los botones; y un ESP32 común que ejecuta los actuadores, ambas ESP32 se comunican mediante UART (Serial).

- Las macetas ahora tienen incorporado su propio FC-28 permitiendo intercambiarlas facilmente.

- El motor Paso a Paso que insertaba el FC-28 fue completamente descartado, si no se menciona en el informe original, no menciones este detalle.

- El circuito entero funciona con 5V y 3A, sin embargo hay un conversor a 3.3V por algunos componentes.

### Sensores Definitivos y Actuadores

- DHT22 (x1) (Temperatura y Humedad del Aire)
- FC-28 (x2) (Humedad de la Tierra de Cada Maceta)
- Ultrasonido (x1) (Detectar la Altura de las Plantas)
- Fotoresistencia (x1) (Para saber si hay mucha iluminación exterior)
- Bomba de Agua DC 5V (x1) (Para el riego)
- LEDs UltraVioleta (x2) (Para el estímulo de crecimiento)

## Flujo de Generación

En el directorio raíz (greenhouse) están los directorios "core", "nervius" y "alicia" donde se encuentra el código de todo el proyecto, extrae de ahí los fragmento más relevantes y agregalos al informe como anexos, haz enfoque en core y alicia.

También hay unos documentos, en el documento llamado "INFORME_DESTINO" lo que harás es editarlo para crear el informe nuevo.
