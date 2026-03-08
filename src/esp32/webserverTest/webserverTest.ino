// ============================================================
// ESP32 Wi‑Fi Client Web Server – "Hello Skell!"
// ============================================================
// This code connects your ESP32 to your existing Wi‑Fi network
// and hosts a simple webpage that says "Hello Skell!".
//
// Libraries used:
//   - WiFi.h (built into the ESP32 board package)
// No extra libraries needed.
// ============================================================

#include <WiFi.h>

// ------------------------------------------------------------
// YOUR WI‑FI CREDENTIALS – REPLACE WITH YOUR OWN
// ------------------------------------------------------------
const char* ssid = "mannqui";          // <-- CHANGE THIS
const char* password = "576m12935266";   // <-- CHANGE THIS

// Create a server that listens on port 80
WiFiServer server(80);

// ------------------------------------------------------------
// setup() – runs once when the ESP32 starts
// ------------------------------------------------------------
void setup() {
  Serial.begin(115200);
  delay(1000);
  Serial.println("\n\n=== ESP32 Wi‑Fi Client Starting ===");

  // Connect to your Wi‑Fi network
  Serial.print("Connecting to ");
  Serial.print(ssid);
  WiFi.begin(ssid, password);

  // Wait until the connection is established
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("\nConnected to Wi‑Fi!");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());   // This is the address you'll use in your browser

  // Start the server
  server.begin();
  Serial.println("Server started. Open a browser and go to the IP address above.");
}

// ------------------------------------------------------------
// loop() – handles incoming client requests
// ------------------------------------------------------------
void loop() {
  // Check if a client has connected
  WiFiClient client = server.available();
  if (!client) {
    return;   // no client, go back and check again
  }

  // Client connected – wait a moment for the request to arrive
  unsigned long timeout = millis() + 2000;
  while (!client.available() && millis() < timeout) {
    delay(1);
  }

  // Read the first line of the request (we don't actually need it)
  String request = client.readStringUntil('\r');
  Serial.println("Request: " + request);
  client.flush();

  // --------------------------------------------------------
  // Simple HTML page – just "Hello Skell!"
  // --------------------------------------------------------
  String html = R"rawliteral(
<!DOCTYPE html>
<html>
<head>
    <title>Hello Skell!</title>
</head>
<body>
    <h1>Hello Skell!</h1>
    <p>This is a simple test page served by your ESP32.</p>
</body>
</html>
)rawliteral";

  // Send HTTP response headers
  client.println("HTTP/1.1 200 OK");
  client.println("Content-Type: text/html");
  client.println("Connection: close");
  client.print("Content-Length: ");
  client.println(html.length());
  client.println();

  // Send the HTML page
  client.print(html);

  // Close the connection
  client.stop();
  Serial.println("Client disconnected.\n");
}