from flask import Flask, jsonify
import random
import time

app = Flask(__name__)


def validarDato(val):
    try:
        return float(val)
    except Exception:
        return 0


@app.route('/data')
def data():
    # Simula los mismos campos de la API original
    payload = {
        "ha": round(random.uniform(35.0, 75.0), 0),  # humedad aire
        "te": round(random.uniform(18.0, 30.0), 1),  # temperatura
        "ht": round(random.uniform(20.0, 90.0), 0),  # humedad tierra
        "li": round(random.uniform(0.0, 100.0), 0),  # litros / nivel tanque
        "ap": round(random.uniform(10.0, 120.0), 0),  # altura planta
        # Campos con nombres largos para compatibilidad con la app
        "TEMPERATURA": f"{random.uniform(18.0, 30.0):.1f}",
        "HUMEDAD": f"{random.uniform(35.0, 75.0):.0f}",
        "SUELO": f"{random.uniform(20.0, 90.0):.0f}",
        "ALTURA": f"{random.uniform(10.0, 120.0):.0f}",
        "TANQUE": f"{random.uniform(0.0, 100.0):.0f}",
        "TEMP. TANQUE": f"{random.uniform(10.0, 35.0):.1f}",
    }
    return jsonify(payload)


if __name__ == '__main__':
    # Ejecutar en todas las interfaces para que sea detectable en la LAN
    print('API simulada en http://0.0.0.0:5000/data')
    app.run(host='0.0.0.0', port=5000)
