#include <Arduino.h>

// Pin definitions
#define RED_LED    16  // D0
#define GREEN_LED   5  // D1
#define BLUE_LED    4  // D2
#define LDR_PIN     A0 // A0 - Photoresistor

// Simple function to turn all LEDs off
void allLEDsOff() {
  digitalWrite(RED_LED, HIGH);   // HIGH = OFF for common anode
  digitalWrite(GREEN_LED, HIGH);
  digitalWrite(BLUE_LED, HIGH);
}

// Function to read LDR with a specific LED on
int readWithLED(int ledPin) {
  allLEDsOff();
  delay(500);
  int ambient = analogRead(LDR_PIN);
  
  digitalWrite(ledPin, LOW);  // Turn LED ON
  delay(500);
  int withLED = analogRead(LDR_PIN);
  digitalWrite(ledPin, HIGH); // Turn LED OFF
  
  return ambient - withLED;  // Return difference
}

String detectColor(int r, int g, int b) {
  
  float redGain = 0.22;
  float greenGain = 1;
  float blueGain = 1.24;

  float float_r = r * redGain;
  float float_g = g * greenGain;
  float float_b = b * blueGain;

  const int MIN_THRESHOLD = 100;
  
  // If very low reflection - no object
  if (float_r < MIN_THRESHOLD && float_g < MIN_THRESHOLD && float_b < MIN_THRESHOLD) {
    return "NONE";
  }

  // Find maximum
  if (float_r > float_g && float_r > float_b) {
    return "RED";
  }
  else if (float_g > float_r && float_g > float_b) {
    return "GREEN";
  }
  else if (float_b > float_r && float_b > float_g) {
    return "BLUE";
  }
  else {
    return "UNKNOWN";
  }
}

void setup() {
  Serial.begin(9600);
  delay(1000);
  
  // Setup LED pins
  pinMode(RED_LED, OUTPUT);
  pinMode(GREEN_LED, OUTPUT);
  pinMode(BLUE_LED, OUTPUT);
  pinMode(LDR_PIN, INPUT);
  
  // Start with all LEDs OFF
  allLEDsOff();
  
  Serial.println("RED,GREEN,BLUE,RAW");
}

void loop() {
  int redVal = readWithLED(RED_LED);
  int greenVal = readWithLED(GREEN_LED);
  int blueVal = readWithLED(BLUE_LED);
  int rawVal = analogRead(LDR_PIN);

  String detected = detectColor(redVal, greenVal, blueVal);

  Serial.print("R: ");
  Serial.print(redVal);
  Serial.print(" G: ");
  Serial.print(greenVal);
  Serial.print(" B: ");
  Serial.print(blueVal);
  Serial.print(" -> DETECTED: ");
  Serial.println(detected);

  delay(1000);
}