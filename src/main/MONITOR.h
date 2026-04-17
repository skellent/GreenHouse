#pragma once
#include <Arduino.h>

struct WebPage {
  const String WEBPAGE = R"rawliteral(
  <!DOCTYPE html>
  <html>
    <head>
      <meta name='viewport' content='width=device-width, initial-scale=1.0, maximum-scale=1.0, user-scalable=no' charset='UTF-8'>
      <style>
        * { box-sizing: border-box; margin: 0; padding: 0; }
        body {
          font-family: 'Segoe UI', system-ui, sans-serif;
          background: #050307; background-image: radial-gradient(circle at 50% -20%, #4b1b7d, #050307);
          color: white; height: 100vh; display: flex; flex-direction: column; overflow: hidden;
        }
        header { padding: 1.5vh; text-align: center; color: #8a2be2; font-size: 1.1em; font-weight: 900; letter-spacing: 1px; }
        .container { flex: 1; display: flex; flex-direction: column; padding: 10px; gap: 10px; height: 100%; }
        
        /* Los monitores se expanden para llenar el espacio */
        .row { display: flex; gap: 10px; flex: 1; min-height: 0; }
        .card {
          flex: 1; background: rgba(255, 255, 255, 0.05); backdrop-filter: blur(15px); -webkit-backdrop-filter: blur(15px);
          border-radius: 20px; display: flex; flex-direction: column; justify-content: center; align-items: center;
          border: 1px solid rgba(255, 255, 255, 0.1);
        }
        .label { font-size: 0.65em; text-transform: uppercase; color: rgba(255,255,255,0.4); margin-bottom: 2px; }
        .val { font-size: 8vh; font-weight: 900; line-height: 1; display: flex; align-items: baseline; }
        .unit { font-size: 0.3em; color: #22c55e; margin-left: 3px; }
        
        /* El area de config se ajusta SOLO a su contenido */
        .config-area { height: auto; flex: 0 1 auto; display: flex; flex-direction: column; gap: 10px; padding: 15px; }
        .form-grid { display: flex; gap: 10px; width: 100%; }
        .form-ui { flex: 1; display: flex; flex-direction: column; gap: 5px; }
        
        select {
          width: 100%; background: rgba(0, 0, 0, 0.6); border: 1px solid #8a2be2;
          border-radius: 12px; color: white; padding: 10px; outline: none; font-size: 0.85em;
          appearance: none; -webkit-appearance: none; text-align: center;
        }
        button {
          background: #22c55e; color: #052e16; border: none; padding: 10px;
          border-radius: 12px; font-weight: 900; cursor: pointer; font-size: 0.8em; transition: 0.2s;
        }
        button:active { transform: scale(0.96); opacity: 0.9; }
        .upd { animation: pulse 0.4s ease; }
        @keyframes pulse { 50% { transform: scale(1.05); color: #22c55e; } }
      </style>
    </head>
    <body onload='tk()'>
      <header>Skell's GreenHouse</header>
      <div class='container'>
        <div class='row'>
          <div class='card' style='color:#22c55e'><div class='label'>Aire</div><div class='val'><span id='ha'>--</span><span class='unit'>%</span></div></div>
          <div class='card' style='color:#8a2be2'><div class='label'>Temp</div><div class='val'><span id='te'>--</span><span class='unit'>°</span></div></div>
        </div>
        <div class='row'>
          <div class='card' style='color:#4ade80'><div class='label'>Tierra</div><div class='val'><span id='ht'>--</span><span class='unit'>%</span></div></div>
          <div class='card' style='color:#a855f7'><div class='label'>Tanque</div><div class='val'><span id='li'>--</span><span class='unit'>%</span></div></div>
        </div>
        
        <div class='card config-area'>
          <div class='form-grid'>
            <form class='form-ui' onsubmit='sd(event,"time","m")'>
              <div class='label'>TIEMPO UV</div>
              <select id='m'>
                <option value='3600000'>1 Hora</option>
                <option value='14400000'>4 Horas</option>
                <option value='21600000' selected>6 Horas</option>
                <option value='32400000'>9 Horas</option>
                <option value='43200000'>12 Horas</option>
              </select>
              <button type='submit'>Actualizar</button>
            </form>
            <form class='form-ui' onsubmit='sd(event,"uv","u")'>
              <div class='label'>INTENSIDAD UV</div>
              <select id='u'>
                <option value='215'>Muy Bajo</option>
                <option value='185'>Bajo</option>
                <option value='155' selected>Medio</option>
                <option value='125'>Alto</option>
                <option value='100'>Muy Alto</option>
              </select>
              <button type='submit'>Actualizar</button>
            </form>
          </div>
          <form class='form-ui' style='width:100%' onsubmit='sd(event,"hum","h")'>
            <div class='label'>UMBRAL HUMEDAD TIERRA</div>
            <div style='display:flex; gap:10px'>
              <select id='h' style='flex:1'>
                <option value='20'>20%</option>
                <option value='40'>40%</option>
                <option value='50' selected>50%</option>
                <option value='60'>60%</option>
                <option value='70'>70%</option>
              </select>
              <button type='submit' style='flex:0.6'>Actualizar</button>
            </div>
          </form>
        </div>
      </div>
      <script>
        const g = i => document.getElementById(i);
        function tk() {
          fetch('/data').then(r => r.json()).then(d => {
            ['ha','te','ht','li'].forEach(k => {
              if(g(k).innerText != d[k]) { 
                g(k).innerText = d[k]; 
                g(k).classList.add('upd'); 
                setTimeout(()=>g(k).classList.remove('upd'),400); 
              }
            });
          });
        }
        function sd(e,p,i) {
          e.preventDefault();
          fetch(`/config?${p}=${g(i).value}`)
            .then(r => r.text())
            .then(txt => alert(txt));
        }
        setInterval(tk, 2500);
      </script>
    </body>
  </html>)rawliteral";
};
extern WebPage monitor;