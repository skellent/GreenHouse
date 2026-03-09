#pragma once
#include <Arduino.h>

struct WebPage {
  const String WEBPAGE = R"rawliteral(
  <!DOCTYPE html>
  <html>
    <head>
      <meta name='viewport' content='width=device-width, initial-scale=1.0, maximum-scale=1.0, user-scalable=no' charset='UTF-8'>
      <style>
        * {
          box-sizing: border-box;
          margin: 0;
          padding: 0;
        }
        body {
          font-family: -apple-system, sans-serif;
          background: #000;
          color: white;
          height: 100vh;
          display: flex;
          flex-direction: column;
        }
        header {
          padding: 15px;
          text-align: center;
          background: #1a1a1a;
          color: #00adb5;
          font-size: 1.2em;
          font-weight: bold;
        }
        .container {
          flex: 1;
          display: flex;
          flex-direction: column;
          padding: 10px;
          gap: 10px;
        }
        .row {
          flex: 1;
          display: flex;
          gap: 10px;
        }
        .card {
          flex: 1;
          background: #1e1e1e;
          border-radius: 15px;
          display: flex;
          flex-direction: column;
          justify-content: center;
          align-items: center;
          border-bottom: 6px solid #444;
        }
        .label {
          font-size: 0.9em;
          text-transform: uppercase;
          color: #aaa;
          margin-bottom: 5px;
        }
        .val {
          font-size: 3em;
          font-weight: bold;
        }
        .unit {
          font-size: 0.4em;
          color: #00adb5;
        }
        footer {
          padding: 5px;
          text-align: center;
          font-size: 0.7em;
          color: #444;
        }
      </style>
      <script>
        function update() {
          fetch('/data').then(r => r.json()).then(d => {
            document.getElementById('ha').innerText = d.ha;
            document.getElementById('te').innerText = d.te;
            document.getElementById('ht').innerText = d.ht;
            document.getElementById('li').innerText = d.li;
          });
        }
        setInterval(update, 1500);
      </script>
    </head>
    <body onload='update()'>
      <header>
        Skell's GreenHouse Live
      </header>
      <div class='container'>
        <div class='row'>
          <div class='card' style='border-color: #00adb5'>
            <div class='label'>Aire</div>
            <div class='val'>
              <span id='ha'>--</span><span class='unit'>%</span>
            </div>
          </div>
          <div class='card' style='border-color: #ff5722'>
            <div class='label'>Temp</div>
            <div class='val'>
              <span id='te'>--</span><span class='unit'>°C</span>
            </div>
          </div>
        </div>
        <div class='row'>
          <div class='card' style='border-color: #8bc34a'>
            <div class='label'>Tierra</div>
            <div class='val'>
              <span id='ht'>--</span><span class='unit'>%</span>
            </div>
          </div>
          <div class='card' style='border-color: #2196f3'>
            <div class='label'>Agua</div>
            <div class='val'>
              <span id='li'>--</span><span class='unit'>L</span>
            </div>
          </div>
        </div>
      </div>
      <footer>
        Skell was here
      </footer>
    </body>
  </html>)rawliteral";
};

extern WebPage monitor;