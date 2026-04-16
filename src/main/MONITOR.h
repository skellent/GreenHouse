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
        header { padding: 1.5vh 0; text-align: center; color: #8a2be2; font-size: 1.2em; font-weight: 900; letter-spacing: 2px; flex-shrink: 0; }
        .container { flex: 1; display: flex; flex-direction: column; padding: 10px; gap: 8px; min-height: 0; }
        .row { display: flex; gap: 8px; flex: 1; min-height: 0; }
        .card {
          flex: 1; background: rgba(255, 255, 255, 0.05); backdrop-filter: blur(15px); -webkit-backdrop-filter: blur(15px);
          border-radius: 18px; display: flex; flex-direction: column; justify-content: center; align-items: center;
          border: 1px solid rgba(255, 255, 255, 0.1); position: relative;
        }
        .icon { width: 3.5vh; height: 3.5vh; fill: none; stroke: currentColor; stroke-width: 2; margin-bottom: 0.5vh; filter: drop-shadow(0 0 8px currentColor); }
        .label { font-size: 0.65em; text-transform: uppercase; color: rgba(255,255,255,0.5); font-weight: 600; }
        .val { font-size: 5.5vh; font-weight: 900; line-height: 1; display: flex; align-items: baseline; }
        .unit { font-size: 0.35em; color: #22c55e; margin-left: 4px; }
        
        /* Ajuste de formulario para ocupar menos espacio */
        .config-area { flex: 0.4; min-height: fit-content; gap: 6px; padding: 12px; margin-bottom: 10px; }
        .form-ui { display: flex; width: 100%; gap: 6px; }
        input {
          flex: 1; background: rgba(0, 0, 0, 0.4); border: 1px solid #8a2be2; height: 38px;
          border-radius: 8px; color: white; padding: 0 10px; text-align: center; font-size: 0.85em; outline: none;
        }
        button { background: #22c55e; color: #052e16; border: none; padding: 0 15px; border-radius: 8px; font-weight: 900; cursor: pointer; font-size: 0.8em; }
        
        .upd { animation: pulse 0.4s ease; }
        @keyframes pulse { 50% { transform: scale(1.03); color: #22c55e; } }
      </style>
    </head>
    <body onload='tk()'>
      <header>SKELL'S GREENHOUSE</header>
      <div class='container'>
        <div class='row'>
          <div class='card' style='color:#22c55e'><svg class='icon' viewBox='0 0 24 24'><path d='M9.59 4.59A2 2 0 1 1 11 8H2m10.59 11.41A2 2 0 1 0 14 16H2m15.73-8.27A2.5 2.5 0 1 1 19.5 12H2'/></svg>
            <div class='label'>Aire</div><div class='val'><span id='ha'>--</span><span class='unit'>%</span></div></div>
          <div class='card' style='color:#8a2be2'><svg class='icon' viewBox='0 0 24 24'><path d='M14 4v10.54a4 4 0 1 1-4 0V4a2 2 0 0 1 4 0Z'/></svg>
            <div class='label'>Temp</div><div class='val'><span id='te'>--</span><span class='unit'>°C</span></div></div>
        </div>
        <div class='row'>
          <div class='card' style='color:#4ade80'><svg class='icon' viewBox='0 0 24 24'><path d='M12 10V3m0 7L8 6m4 4 4-4M3 21h18M5 21v-7a2 2 0 0 1 2-2h10a2 2 0 0 1 2 2v7'/></svg>
            <div class='label'>Tierra</div><div class='val'><span id='ht'>--</span><span class='unit'>%</span></div></div>
          <div class='card' style='color:#a855f7'><svg class='icon' viewBox='0 0 24 24'><path d='M12 2.69l5.66 5.66a8 8 0 1 1-11.31 0z'/></svg>
            <div class='label'>Tanque</div><div class='val'><span id='li'>--</span><span class='unit'>%</span></div></div>
        </div>
        <div class='row'>
          <div class='card' style='color:#3b82f6'><svg class='icon' viewBox='0 0 24 24'><path d='M13 5V21m-8-3h16M5 12h16M5 6h16'/></svg>
            <div class='label'>Crecimiento</div><div class='val'><span id='ap'>--</span><span class='unit'>cm</span></div></div>
        </div>

        <div class='card config-area'>
          <form class='form-ui' onsubmit='sd(event,"time","m")'>
            <input type='number' id='m' placeholder='Segundos de Luz' required min="0"><button type='submit'>LUZ</button>
          </form>
          <form class='form-ui' onsubmit='sd(event,"hum","h")'>
            <input type='number' id='h' placeholder='Humedad Min (%)' required min="0" max="100"><button type='submit'>HUM</button>
          </form>
        </div>
      </div>
      <script>
        const g = i => document.getElementById(i);
        function tk() {
          fetch('/data').then(r => r.json()).then(d => {
            ['ha','te','ht','li','ap'].forEach(k => {
              if(g(k) && g(k).innerText != d[k]) { g(k).innerText = d[k]; g(k).classList.add('upd'); setTimeout(()=>g(k).classList.remove('upd'),400); }
            });
          });
        }
        function sd(e,p,i) {
          e.preventDefault();
          fetch(`/config?${p}=${g(i).value}`).then(r => r.text()).then(t => alert(t));
        }
        setInterval(tk, 2500);
      </script>
    </body>
  </html>)rawliteral";
};
extern WebPage monitor;