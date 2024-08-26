#include <WiFi.h>
#include <WiFiClient.h>
#include <WebServer.h>
#include <ThingSpeak.h>

const char* ssid = "BLASTBALA";
const char* password = "KISHORE03";   // Your WiFi password

WebServer server(80);

const int trigPin = 12;    // Pin connected to the trigger pin of ultrasonic sensor 1
const int echoPin = 14;    // Pin connected to the echo pin of ultrasonic sensor 1
const int trigPin2 = 27;   // Pin connected to the trigger pin of ultrasonic sensor 2
const int echoPin2 = 26;   // Pin connected to the echo pin of ultrasonic sensor 2
const int servoPin = 4;    // Pin connected to the servo motor

const char* thingSpeakApiKey = "1806FYPV8L998QZJ";
const unsigned long channelNumber = 2502142;

WiFiClient client;

long duration;
int distance;
int distance2;

void setup() {
  Serial.begin(115200);
  
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
  pinMode(trigPin2, OUTPUT);
  pinMode(echoPin2, INPUT);
  pinMode(servoPin, OUTPUT);
  
  // Connect to Wi-Fi
  WiFi.begin(ssid, password);
  Serial.println("Connecting to WiFi...");
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting...");
  }
  Serial.println("Connected to WiFi");

  ThingSpeak.begin(client); // Initialize ThingSpeak client
  
  // Start the server
  server.on("/", handleRoot);
  server.begin();
  Serial.println("HTTP server started");
}

void loop() {
  // Measure distance from first ultrasonic sensor
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
  
  duration = pulseIn(echoPin, HIGH);
  distance = duration * 0.034 / 2;   // Calculate distance in cm
  
  // Measure distance from second ultrasonic sensor
  digitalWrite(trigPin2, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin2, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin2, LOW);
  
  duration = pulseIn(echoPin2, HIGH);
  distance2 = duration * 0.034 / 2;   // Calculate distance in cm
  
  Serial.print("Distance from Sensor 1: ");
  Serial.print(distance);
  Serial.println(" cm");

  Serial.print("Distance from Sensor 2: ");
  Serial.print(distance2);
  Serial.println(" cm");
  
  // Update ThingSpeak channel with distance values
  ThingSpeak.writeField(channelNumber, 1, distance, thingSpeakApiKey);
  ThingSpeak.writeField(channelNumber, 2, distance2, thingSpeakApiKey);
  
  server.handleClient();  // Handle client requests
  delay(1000);            // Update every second
}

void handleRoot() {
  String response = "Distance from Sensor 1: " + String(distance) + " cm\n" +
                    "Distance from Sensor 2: " + String(distance2) + " cm\n";
  server.send(200, "text/plain", response); // Send distance values to client
}
