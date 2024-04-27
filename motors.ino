#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <ArduinoJson.h>

// WiFi credentials
const char* ssid = "YOUR_SSID";
const char* password = "YOUR_PASSWORD";

ESP8266WebServer server(80);

// Motor control pins
const int motor1Pin1 = D1;  // Motor 1 forward
const int motor1Pin2 = D2;  // Motor 1 backward
const int motor2Pin1 = D3;  // Motor 2 forward
const int motor2Pin2 = D4;  // Motor 2 backward

// Ultrasonic Sensor Pins
const int trigPin = D5;
const int echoPin = D6;

float measureDistance();
void stopAllMotors();

void setup() {
  Serial.begin(115200);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("Connected to WiFi");

  pinMode(motor1Pin1, OUTPUT);
  pinMode(motor1Pin2, OUTPUT);
  pinMode(motor2Pin1, OUTPUT);
  pinMode(motor2Pin2, OUTPUT);

  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);

  stopAllMotors();

  server.on("/motor1/left", []() {
    digitalWrite(motor1Pin1, HIGH);
    digitalWrite(motor1Pin2, LOW);
    DynamicJsonDocument doc(1024);
    doc["status"] = "success";
    doc["message"] = "Motor 1 turning Left";
    String response;
    serializeJson(doc, response);
    server.send(200, "application/json", response);
  });

  server.on("/motor1/right", []() {
    digitalWrite(motor1Pin1, LOW);
    digitalWrite(motor1Pin2, HIGH);
    DynamicJsonDocument doc(1024);
    doc["status"] = "success";
    doc["message"] = "Motor 1 turning Right";
    String response;
    serializeJson(doc, response);
    server.send(200, "application/json", response);
  });

  server.on("/motor2/left", []() {
    digitalWrite(motor2Pin1, HIGH);
    digitalWrite(motor2Pin2, LOW);
    DynamicJsonDocument doc(1024);
    doc["status"] = "success";
    doc["message"] = "Motor 2 turning Left";
    String response;
    serializeJson(doc, response);
    server.send(200, "application/json", response);
  });

  server.on("/motor2/right", []() {
    digitalWrite(motor2Pin1, LOW);
    digitalWrite(motor2Pin2, HIGH);
    DynamicJsonDocument doc(1024);
    doc["status"] = "success";
    doc["message"] = "Motor 2 turning Right";
    String response;
    serializeJson(doc, response);
    server.send(200, "application/json", response);
  });

  server.on("/distance", []() {
    DynamicJsonDocument doc(1024);
    if (server.hasArg("meters")) {
      float targetDistance = server.arg("meters").toFloat();
      float measuredDistance = measureDistance();
      while (measuredDistance < targetDistance) {
        if (measureDistance() < 30.0) {
          stopAllMotors();
          doc["status"] = "obstacle-detected";
          doc["message"] = "Obstacle detected! Motors stopped.";
          String response;
          serializeJson(doc, response);
          server.send(200, "application/json", response);
          return;
        }
        digitalWrite(motor1Pin1, HIGH);
        delay(100);
        measuredDistance += measureDistance();
      }
      stopAllMotors();
      doc["status"] = "success";
      doc["message"] = "Target distance reached";
      doc["distance"] = measuredDistance;
    } else {
      doc["status"] = "error";
      doc["message"] = "Bad Request: No distance provided";
    }
    String response;
    serializeJson(doc, response);
    server.send(200, "application/json", response);
  });

  server.begin();
  Serial.println("HTTP server started");
  Serial.println(WiFi.localIP());
}

void loop() {
  server.handleClient();
}

float measureDistance() {
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
  
  long duration = pulseIn(echoPin, HIGH);
  float distance = duration * 0.034 / 2;
  return distance;
}

void stopAllMotors() {
  digitalWrite(motor1Pin1, LOW);
  digitalWrite(motor1Pin2, LOW);
  digitalWrite(motor2Pin1, LOW);
  digitalWrite(motor2Pin2, LOW);
}
