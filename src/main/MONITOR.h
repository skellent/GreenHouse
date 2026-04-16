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
          background: #08050a;
          background-image: radial-gradient(circle at 50% -20%, #2e1065, #08050a);
          color: white; height: 100vh; display: flex; flex-direction: column; overflow: hidden;
        }
        header {
          padding: 12px; text-align: center; background: rgba(255, 255, 255, 0.03);
          backdrop-filter: blur(10px); color: #a855f7; font-size: 1.1em; font-weight: 900;
          letter-spacing: 2px; border-bottom: 1px solid rgba(168, 85, 247, 0.3);
        }
        .container { flex: 1; display: flex; flex-direction: column; padding: 8px; gap: 8px; }
        .row { display: flex; gap: 8px; flex: 1; }
        .card {
          flex: 1; background: rgba(255, 255, 255, 0.04); backdrop-filter: blur(12px);
          -webkit-backdrop-filter: blur(12px); border-radius: 18px; display: flex;
          flex-direction: column; justify-content: center; align-items: center;
          border: 1px solid rgba(255, 255, 255, 0.1); transition: 0.3s; position: relative;
        }
        .card::before {
          content: ''; position: absolute; inset: 0; border-radius: 18px;
          padding: 1px; background: linear-gradient(135deg, rgba(255,255,255,0.2), transparent);
          -webkit-mask: linear-gradient(#fff 0 0) content-box, linear-gradient(#fff 0 0);
          mask-composite: exclude; pointer-events: none;
        }
        .icon { width: 30px; height: 30px; fill: none; stroke: currentColor; stroke-width: 2; margin-bottom: 8px; filter: drop-shadow(0 0 8px currentColor); }
        .label { font-size: 0.6em; text-transform: uppercase; color: rgba(255,255,255,0.5); letter-spacing: 1px; }
        .val { font-size: 2.2em; font-weight: 800; display: flex; align-items: baseline; }
        .unit { font-size: 0.4em; color: #22c55e; margin-left: 3px; }
        
        /* FORMULARIO FUNCIONAL */
        .config-card { flex: 0.7; background: rgba(168, 85, 247, 0.05); border: 1px solid rgba(168, 85, 247, 0.2); }
        .form-ui { display: flex; width: 90%; gap: 8px; margin-top: 10px; }
        input {
          flex: 1; background: rgba(0, 0, 0, 0.4); border: 1px solid rgba(168, 85, 247, 0.4);
          border-radius: 10px; color: white; padding: 8px; text-align: center; outline: none;
        }
        input:focus { border-color: #22c55e; box-shadow: 0 0 10px rgba(34, 197, 94, 0.2); }
        button {
          background: #22c55e; color: #052e16; border: none; padding: 8px 15px;
          border-radius: 10px; font-weight: 900; cursor: pointer; transition: 0.2s;
        }
        button:active { transform: scale(0.95); background: #16a34a; }
        
        .upd { animation: blink 0.5s ease; }
        @keyframes blink { 50% { background: rgba(34, 197, 94, 0.2); } }
        footer { padding: 8px; text-align: center; font-size: 0.6em; color: rgba(255,255,255,0.2); }
      </style>
    </head>
    <body onload='tick()'>
      <header>SKELL'S GREENHOUSE</header>
      <div class='container'>
        <div class='row'>
          <div class='card' style='color:#22c55e'><svg class='icon' viewBox='0 0 24 24'><path d='M9.59 4.59A2 2 0 1 1 11 8H2m10.59 11.41A2 2 0 1 0 14 16H2m15.73-8.27A2.5 2.5 0 1 1 19.5 12H2'/></svg>
            <div class='label'>Aire</div><div class='val'><span id='ha'>--</span><span class='unit'>%</span></div>
          </div>
          <div class='card' style='color:#a855f7'><svg class='icon' viewBox='0 0 24 24'><path d='M14 4v10.54a4 4 0 1 1-4 0V4a2 2 0 0 1 4 0Z'/></svg>
            <div class='label'>Temp</div><div class='val'><span id='te'>--</span><span class='unit'>°C</span></div>
          </div>
        </div>
        <div class='row'>
          <div class='card' style='color:#4ade80'><svg class='icon' viewBox='0 0 24 24'><path d='M12 10V3m0 7L8 6m4 4 4-4M3 21h18M5 21v-7a2 2 0 0 1 2-2h10a2 2 0 0 1 2 2v7'/></svg>
            <div class='label'>Tierra</div><div class='val'><span id='ht'>--</span><span class='unit'>%</span></div>
          </div>
          <div class='card' style='color:#7c3aed'><svg class='icon' viewBox='0 0 24 24'><path d='M12 2.69l5.66 5.66a8 8 0 1 1-11.31 0z'/></svg>
            <div class='label'>Tanque</div><div class='val'><span id='li'>--</span><span class='unit'>%</span></div>
          </div>
        </div>
        <div class='row'>
          <div class='card config-card' style='flex:1'>
            <div class='label' style='color:#a855f7'>PROGRAMAR LUZ UV (MINUTOS)</div>
            <form class='form-ui' onsubmit='send(event)'>
              <input type='number' id='m' placeholder='Min' required>
              <button type='submit'>SET</button>
            </form>
          </div>
        </div>
      </div>
      <footer>Skell &bull; 2026</footer>
      <script>
        const $ = i => document.getElementById(i);
        let l = {};
        function tick() {
          fetch('/data').then(r => r.json()).then(d => {
            ['ha','te','ht','li'].forEach(k => {
              if(l[k] !== d[k]) { $(k).innerText = d[k]; $(k).parentElement.parentElement.classList.add('upd'); 
              setTimeout(()=>$(k).parentElement.parentElement.classList.remove('upd'), 500); l[k]=d[k]; }
            });
          });
        }
        function send(e) {
          e.preventDefault();
          const v = $('m').value;
          fetch(`/config?time=${v*60000}`).then(r => r.ok ? alert('TIEMPO ACTUALIZADO') : null);
        }
        setInterval(tick, 2500);
      </script>
    </body>
  </html>)rawliteral";
};
extern WebPage monitor;