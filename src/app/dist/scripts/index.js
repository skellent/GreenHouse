let ip = "";
const dom = {};

window.addEventListener("DOMContentLoaded", () => {
    const elementsToCache = [
        "humedad-tierra-value", "temperatura-ambiente-value", "tanque-value",
        "altura-planta-value", "uv-value", "bomba-value", 
        "credits-modal", "display-foto", "foto"
    ];

    elementsToCache.forEach(id => dom[id] = document.getElementById(id));

    // Eventos de modales que deben asignarse al cargar la página
    document.getElementById("open-credits").onclick = () => dom["credits-modal"].classList.remove("hidden");
    document.getElementById("close-credits").onclick = () => dom["credits-modal"].classList.add("hidden");
    document.getElementById("cerrar-foto").onclick = () => dom["display-foto"].classList.add("hidden");

    // Evento del botón Obturador de la cámara
    document.getElementById("obturador").onclick = async () => {
        if(!ip) return alert("ESP32 no detectado, esperando conexión...");
        
        const btn = document.getElementById("obturador");
        const originalContent = btn.innerHTML;
        btn.innerHTML = "📸 PROCESANDO...";

        try {
            fetch(`http://${ip}/cam`); // para arreglar un problema con la API de fotos del ESP32 que no responde a la primera solicitud de forma consistente
            const response = await fetch(`http://${ip}/cam`);
            if (!response.ok) throw new Error("Error del servidor");
            
            const blob = await response.blob();
            const objectURL = URL.createObjectURL(blob);
            dom["foto"].src = objectURL;
            dom["display-foto"].classList.remove("hidden");
            
            // Asignar el evento de descarga una vez que la foto existe
            document.getElementById("descargar-foto").onclick = async () => {
                try {
                    const response = await fetch(`http://${ip}/cam`);
                    if (!response.ok) throw new Error("Error del servidor al obtener la foto");
                    const blob = await response.blob();
                    const reader = new FileReader();
                    reader.readAsDataURL(blob);
                    
                    reader.onloadend = async () => {
                        const base64data = reader.result.split(',')[1];
                        try {
                            const { Filesystem } = Capacitor.Plugins;
                            const fileName = `Skells_GreenHouse_Foto_${new Date().getTime()}.jpg`;
                            
                            await Filesystem.writeFile({
                                path: `GreenHouse/${fileName}`,
                                data: base64data,
                                directory: 'DOCUMENTS',
                                recursive: true
                            });
                            alert(`¡Foto guardada exitosamente en Documentos como ${fileName}!`);
                        } catch (fsError) {
                            console.error(fsError);
                            alert("Error al guardar la foto en el dispositivo.");
                        } finally {
                            btn.innerHTML = originalContent;
                        }
                    };
                } catch (error) {
                    console.error("Fallo al capturar:", error);
                    alert("Error al descargar la imagen desde el ESP32.");
                } finally {
                    btn.innerHTML = originalContent;
                }
            };

        } catch (err) {
            console.error(err);
            alert("Error al procesar la foto.");
        } finally {
            btn.innerHTML = originalContent;
        }
    };

    // Iniciar el escáner de fuerza bruta al arrancar la app
    buscarESP32();

});


function buscarESP32() {
    const base = "192.168.0.";
    const octetos = Array.from({length: 254}, (_, i) => i + 1);
    
    const escanear = async (idx) => {
        if(idx >= octetos.length) return;
        const candidate = base + octetos[idx];
        const ctrl = new AbortController();
        const tid = setTimeout(() => ctrl.abort(), 750); // Mantenemos tu tiempo optimizado para la IA
        
        try {
            const r = await fetch(`http://${candidate}/data`, { signal: ctrl.signal });
            if(r.ok) {
                ip = candidate;
                iniciarBucle();
                return;
            }
        } catch(e) {}
        escanear(idx + 1);
    };
    escanear(0);
}

function iniciarBucle() {
    actualizarUI();
    setInterval(actualizarUI, 2500);
}

async function actualizarUI() {
    try {
        const res = await fetch(`http://${ip}/data`);
        const data = await res.json();
        
        const map = {
            "humedad-tierra-value": data.humtierra,
            "temperatura-ambiente-value": data.tempambi,
            "tanque-value": data.litagua,
            "altura-planta-value": data.alturpl,
            "uv-value": data.uv.toFixed(0),
            "bomba-value": data.bomba == 1 ? "ON" : "OFF" 
        };
        
        for (let [id, val] of Object.entries(map)) {
            const el = dom[id];
            if(el && el.innerText != val) {
                el.innerText = val;
                el.classList.add("upd");
                setTimeout(() => el.classList.remove("upd"), 500);
            }
        }
    } catch(e) { 
        console.warn("Reconectando al invernadero..."); 
    }
}