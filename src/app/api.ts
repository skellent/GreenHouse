const CAM_FILE_PATH = "./img.jpg";
const LOCAL_IP = "192.168.0.103";

const server = Bun.serve({
  port: 3000,
  async fetch(req) {
    const url = new URL(req.url);

    const headers = {
      "Access-Control-Allow-Origin": "*",
    };

    /* ── GET /data ── */
    if (url.pathname === "/data" && req.method === "GET") {
      const data = {
        humaire: parseFloat((Math.random() * (80 - 40) + 40).toFixed(1)), // 40% a 80%
        tempambi: parseFloat((Math.random() * (35 - 15) + 15).toFixed(1)), // 15°C a 35°C
        tempesp: parseFloat((Math.random() * (60 - 40) + 40).toFixed(1)), // 40°C a 60°C
        tempagua: parseFloat((Math.random() * (25 - 18) + 18).toFixed(1)), // 18°C a 25°C
        humtierra: parseFloat((Math.random() * (100 - 20) + 20).toFixed(1)), // 20% a 100%
        litagua: parseFloat((Math.random() * (20 - 5) + 5).toFixed(1)), // 5 a 20 Litros
        luz: Math.floor(Math.random() * 4095), // ADC del ESP32 (0 a 4095)
        alturpl: parseFloat((Math.random() * 15).toFixed(1)), // 0 a 15 cm
        uv: Math.floor(Math.random() * 255), // PWM (0 a 255)
        bomba: Math.random() > 0.5 ? 1 : 0, // 0 (Apagada) o 1 (Encendida)
        camOk: true
      };

      return new Response(JSON.stringify(data), {
        status: 200,
        headers: {
          ...headers,
          "Content-Type": "application/json",
        },
      });
    }

    /* ── GET /cam ── */
    if (url.pathname === "/cam" && req.method === "GET") {
      const file = Bun.file(CAM_FILE_PATH);
      const exists = await file.exists();
      if (!exists) {
        return new Response(JSON.stringify({ error: "camara no disponible" }), {
          status: 503,
          headers: {
            ...headers,
            "Content-Type": "application/json",
          },
        });
      }

      return new Response(file, {
        status: 200,
        headers: {
          ...headers,
          "Content-Type": "image/jpeg",
          "Content-Disposition": "inline; filename=planta.jpg",
        },
      });
    }
    return new Response("Not Found", { status: 404, headers });
  },
});

console.log(`🌱 Simulador de Skell's GreenHouse corriendo en: http://${LOCAL_IP}:${server.port}`);
console.log(`📊 API de Datos: http://${LOCAL_IP}:${server.port}/data`);
console.log(`📷 API de Cámara: http://${LOCAL_IP}:${server.port}/cam`);