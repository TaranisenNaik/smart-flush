#include <Servo.h>

// Define pins for Ultrasonic Sensor 1 (person detection)
#define TRIG1 2
#define ECHO1 3

// Define pins for Ultrasonic Sensor 2 (water level detection)
#define TRIG2 4
#define ECHO2 5

// Servo pin
#define SERVO_PIN 9

Servo flushServo;

// Thresholds (adjust after testing)
float personThreshold = 10.0;   // cm change that indicates someone is sitting
float flushThreshold = 5.0;     // cm change that indicates water level drop
int flushAngle = 90;            // servo rotation for flush
int baseAngle = 0;              // normal position

// Variables
float baseDistance1 = 0;
float baseDistance2 = 0;
bool personPresent = false;

float getDistance(int trigPin, int echoPin) {
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
  long duration = pulseIn(echoPin, HIGH);
  float distance = duration * 0.034 / 2; // convert to cm
  return distance;
}

void setup() {
  Serial.begin(9600);
  pinMode(TRIG1, OUTPUT);
  pinMode(ECHO1, INPUT);
  pinMode(TRIG2, OUTPUT);
  pinMode(ECHO2, INPUT);
  flushServo.attach(SERVO_PIN);
  flushServo.write(baseAngle);

  // Take initial base readings
  Serial.println("Calibrating base distances...");
  delay(2000);
  baseDistance1 = getDistance(TRIG1, ECHO1);
  baseDistance2 = getDistance(TRIG2, ECHO2);
  Serial.print("Base Person Distance: "); Serial.println(baseDistance1);
  Serial.print("Base Water Distance: "); Serial.println(baseDistance2);
}

void loop() {
  float personDist = getDistance(TRIG1, ECHO1);
  float waterDist = getDistance(TRIG2, ECHO2);

  Serial.print("PersonDist: "); Serial.print(personDist);
  Serial.print("  WaterDist: "); Serial.println(waterDist);

  // Detect person sitting
  if (abs(personDist - baseDistance1) > personThreshold && !personPresent) {
    personPresent = true;
    Serial.println("Person detected — Occupied!");
  }

  // Detect person leaving
  if (personPresent && abs(personDist - baseDistance1) < personThreshold / 2) {
    Serial.println("Person left — Checking for flush...");

    delay(5000); // small wait after leaving
    float newWaterDist = getDistance(TRIG2, ECHO2);

    if (abs(newWaterDist - baseDistance2) < flushThreshold) {
      Serial.println("No flush detected — activating servo!");
      flushServo.write(flushAngle);
      delay(1500); // hold flush
      flushServo.write(baseAngle);
      Serial.println("Flush complete ✅");
    } else {
      Serial.println("Manual flush detected — no action needed.");
    }

    // Reset for next person
    personPresent = false;
    delay(3000);
  }

  delay(500);
}