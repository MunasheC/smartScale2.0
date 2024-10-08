#include <WiFi.h>
#include <WebServer.h>
#include "HX711.h"

// HX711 circuit wiring
const int LOADCELL_DOUT_PIN = 17;
const int LOADCELL_SCK_PIN = 16;

const char* ssid = "##############";
const char* password = "#########";

HX711 scale;
WebServer server(80);

void setup() {
  Serial.begin(115200);
  
  // Connect to Wi-Fi
  WiFi.begin(ssid, password);
  Serial.print("Connecting to WiFi...");
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.print(".");
  }
  Serial.println("Connected to WiFi");

  // Start the web server and add routes
  server.on("/", handleRoot);
  server.on("/getWeight", handleGetWeight);  // Route for getting the weight
  server.begin();
  Serial.println("HTTP server started");

  // Initialize the scale
  scale.begin(LOADCELL_DOUT_PIN, LOADCELL_SCK_PIN);
  scale.set_scale(25556.9/225);  // Calibration factor
  scale.tare();  // Reset scale to 0
}

void loop() {
  server.handleClient();  // Listen for web requests
}

void handleRoot() {
  // Build the HTML response with CSS styling and a button
  String html = "<!DOCTYPE html><html>";
  html += "<head><title>Weight Scale</title>";
  
  // Add CSS styles
  html += "<style>";
  html += "body { font-family: Arial, sans-serif; background-color: #f4f4f4; text-align: center; padding: 50px; }";
  html += "h1 { color: #333; }";
  html += ".container { background-color: #fff; padding: 20px; border-radius: 10px; box-shadow: 0 0 15px rgba(0,0,0,0.1); max-width: 400px; margin: 0 auto; }";
  html += ".reading { font-size: 2em; color: #4CAF50; margin: 20px 0; }";
  html += "p { font-size: 1.2em; color: #555; }";
  html += "button { padding: 10px 20px; font-size: 1.2em; background-color: #4CAF50; color: white; border: none; border-radius: 5px; cursor: pointer; }";
  html += "button:hover { background-color: #45a049; }";
  html += "</style>";

  // Add JavaScript for AJAX request
  html += "<script>";
  html += "function getWeight() {";
  html += "var xhr = new XMLHttpRequest();";  // Create XMLHttpRequest object
  html += "xhr.open('GET', '/getWeight', true);";  // Request to /getWeight
  html += "xhr.onload = function() {";  // When the request completes
  html += "if (xhr.status == 200) {";
  html += "document.getElementById('weight').innerHTML = xhr.responseText + ' kg';";  // Update the weight reading
  html += "}";
  html += "};";
  html += "xhr.send();";  // Send the request
  html += "}";
  html += "</script>";

  html += "</head>";
  html += "<body>";
  
  // HTML content
  html += "<div class='container'>";
  html += "<h1>Weight Scale</h1>";
  html += "<p>Current weight:</p>";
  html += "<div id='weight' class='reading'>-- kg</div>";  // The weight value will be displayed here
  html += "<button onclick='getWeight()'>Get Weight</button>";  // Button to get the weight
  html += "</div>";

  html += "</body></html>";
  
  // Send the response to the client
  server.send(200, "text/html", html);
}

void handleGetWeight() {
  float weight = scale.get_units(10);  // Get the average of 10 readings
  weight = weight/1000;
  if (weight < 0) {
    weight = 0;
  }
  
  server.send(200, "text/plain", String(weight, 2));  // Send the weight as plain text
}
