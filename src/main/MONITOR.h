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
          font-family: 'Segoe UI', Roboto, Helvetica, Arial, sans-serif;
          background: #0a0a0a;
          color: white;
          min-height: 100vh;
          display: flex;
          flex-direction: column;
        }
        header {
          padding: 20px;
          text-align: center;
          background: #111;
          color: #00adb5;
          font-size: 1.4em;
          font-weight: 800;
          letter-spacing: 1px;
          border-bottom: 1px solid #222;
        }
        .container {
          flex: 1;
          display: flex;
          flex-direction: column;
          padding: 15px;
          gap: 12px;
        }
        .row {
          display: flex;
          gap: 12px;
          flex: 1;
        }
        .card {
          flex: 1;
          background: #161616;
          border-radius: 20px;
          display: flex;
          flex-direction: column;
          justify-content: center;
          align-items: center;
          padding: 15px;
          border: 1px solid #222;
          transition: transform 0.2s;
        }
        .card:active { transform: scale(0.98); }
        .icon {
          width: 32px;
          height: 32px;
          fill: none;
          stroke: currentColor;
          stroke-width: 2;
          stroke-linecap: round;
          stroke-linejoin: round;
          margin-bottom: 8px;
          opacity: 0.8;
        }
        .label {
          font-size: 0.7em;
          text-transform: uppercase;
          color: #666;
          letter-spacing: 1.5px;
          margin-bottom: 5px;
        }
        .val {
          font-size: 2.2em;
          font-weight: 700;
          display: flex;
          align-items: baseline;
        }
        .unit {
          font-size: 0.4em;
          color: #00adb5;
          margin-left: 4px;
        }
        footer {
          padding: 15px;
          text-align: center;
          font-size: 0.7em;
          color: #333;
          text-transform: uppercase;
        }
      </style>
      <script>
        function update() {
          fetch('/data').then(r => r.json()).then(d => {
            document.getElementById('ha').innerText = d.ha;
            document.getElementById('te').innerText = d.te;
            document.getElementById('ht').innerText = d.ht;
            document.getElementById('li').innerText = d.li;
            document.getElementById('ap').innerText = d.ap; // Nueva funcionalidad para altura
          }).catch(e => console.log("Error API"));
        }
        setInterval(update, 1500);
      </script>
    </head>
    <body onload='update()'>
      <header>SKELL'S GREENHOUSE</header>
      <div class='container'>
        <div class='row'>
          <div class='card' style='border-bottom: 4px solid #00adb5'>
            <svg class='icon' viewBox='0 0 24 24' style='color:#00adb5'><path d='M9.59 4.59A2 2 0 1 1 11 8H2m10.59 11.41A2 2 0 1 0 14 16H2m15.73-8.27A2.5 2.5 0 1 1 19.5 12H2'/></svg>
            <div class='label'>Aire</div>
            <div class='val'><span id='ha'>--</span><span class='unit'>%</span></div>
          </div>
          <div class='card' style='border-bottom: 4px solid #ff5722'>
            <svg class='icon' viewBox='0 0 24 24' style='color:#ff5722'><path d='M14 4v10.54a4 4 0 1 1-4 0V4a2 2 0 0 1 4 0Z'/></svg>
            <div class='label'>Temp</div>
            <div class='val'><span id='te'>--</span><span class='unit'>°C</span></div>
          </div>
        </div>
        
        <div class='row'>
          <div class='card' style='border-bottom: 4px solid #8bc34a'>
            <svg class='icon' viewBox='0 0 24 24' style='color:#8bc34a'><path d='M12 10V3m0 7L8 6m4 4 4-4M3 21h18M5 21v-7a2 2 0 0 1 2-2h10a2 2 0 0 1 2 2v7'/></svg>
            <div class='label'>Tierra</div>
            <div class='val'><span id='ht'>--</span><span class='unit'>%</span></div>
          </div>
          <div class='card' style='border-bottom: 4px solid #2196f3'>
            <svg class='icon' viewBox='0 0 24 24' style='color:#2196f3'><path d='M12 2.69l5.66 5.66a8 8 0 1 1-11.31 0z'/></svg>
            <div class='label'>Tanque</div>
            <div class='val'><span id='li'>--</span><span class='unit'>L</span></div>
          </div>
        </div>

        <div class='row'>
          <div class='card' style='border-bottom: 4px solid #fbc02d; flex: 1;'>
            <svg class='icon' viewBox='0 0 24 24' style='color:#fbc02d'><path d='M12 20V5m-4 4 4-4 4 4M7 21h10'/></svg>
            <div class='label'>Crecimiento</div>
            <div class='val'><span id='ap'>--</span><span class='unit'>cm</span></div>
          </div>
        </div>
      </div>
      <footer>Skell was here &bull; 2026</footer>
    </body>
  </html>)rawliteral";
};

extern WebPage monitor;