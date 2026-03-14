#include <Arduino.h>

// Pin definitions
#define RED_LED    16  // D0
#define GREEN_LED   5  // D1
#define BLUE_LED    4  // D2
#define LDR_PIN     A0 // D4 - Photoresistor

// Simple function to turn all LEDs off
void allLEDsOff() {
  digitalWrite(RED_LED, HIGH);   // HIGH = OFF for common anode
  digitalWrite(GREEN_LED, HIGH);
  digitalWrite(BLUE_LED, HIGH);
}

// Function to read LDR with a specific LED on
int readWithLED(int ledPin) {
  allLEDsOff();
  delay(200);
  int ambient = analogRead(LDR_PIN);
  
  digitalWrite(ledPin, LOW);  // Turn LED ON
  delay(100);
  int withLED = analogRead(LDR_PIN);
  digitalWrite(ledPin, HIGH); // Turn LED OFF
  
  return ambient - withLED;  // Return difference
}

// Global calibration variables
float redGain = 1.0;
float greenGain = 1.0;
float blueGain = 1.0;
bool calibrated = false;

void calibrateWithWhite() {
  Serial.println("\n=== AUTO-CALIBRATION ===");
  Serial.println("Place WHITE paper and press ENTER");
  
  while(!Serial.available()) delay(100);
  while(Serial.available()) Serial.read();
  
  Serial.println("Calibrating...");
  delay(1000);
  
  const int samples = 10;
  long sumR = 0, sumG = 0, sumB = 0;
  
  for(int i = 0; i < samples; i++) {
    sumR += readWithLED(RED_LED);
    sumG += readWithLED(GREEN_LED);
    sumB += readWithLED(BLUE_LED);
    delay(200);
  }
  
  int avgR = sumR / samples;
  int avgG = sumG / samples;
  int avgB = sumB / samples;
  
  Serial.print("Raw white readings - R: ");
  Serial.print(avgR);
  Serial.print(" G: ");
  Serial.print(avgG);
  Serial.print(" B: ");
  Serial.println(avgB);
  
  // Calculate target as AVERAGE of all channels
  float target = (avgR + avgG + avgB) / 3.0;
  
  redGain = target / avgR;
  greenGain = target / avgG;
  blueGain = target / avgB;
  
  Serial.print("Calculated gains - R: ");
  Serial.print(redGain);
  Serial.print(" G: ");
  Serial.print(greenGain);
  Serial.print(" B: ");
  Serial.println(blueGain);
  
  calibrated = true;
  Serial.println("Calibration complete!\n");
}

struct HSV {
  float h;  // 0-360 degrees
  float s;  // 0-100%
  float v;  // 0-100%
};

// Function to convert RGB to HSV
HSV rgbToHSV(int r, int g, int b) {
  HSV result;
  
  // Apply your existing gain values
  
  float rNorm = r * redGain;
  float gNorm = g * greenGain;
  float bNorm = b * blueGain;
  
  // Find max and min values
  float maxVal = max(rNorm, max(gNorm, bNorm));
  float minVal = min(rNorm, min(gNorm, bNorm));
  float delta = maxVal - minVal;
  
  // Calculate Value (brightness) - normalize to 0-100%
  result.v = (maxVal / 1024.0) * 100;
  
  // Calculate Saturation
  if (maxVal == 0) {
    result.s = 0;
  } else {
    result.s = (delta / maxVal) * 100;
  }
  
  // Calculate Hue
  if (delta == 0) {
    result.h = 0; // Undefined, default to 0
  } else if (maxVal == rNorm) {
    result.h = 60 * fmod(((gNorm - bNorm) / delta), 6);
  } else if (maxVal == gNorm) {
    result.h = 60 * (((bNorm - rNorm) / delta) + 2);
  } else { // maxVal == bNorm
    result.h = 60 * (((rNorm - gNorm) / delta) + 4);
  }
  
  // Ensure hue is between 0-360
  if (result.h < 0) {
    result.h += 360;
  }
  
  return result;
}

String detectColorHSV(HSV hsv) {
  // Check for black (very low value)
  if (hsv.v < 8) {
    return "BLACK";
  }
  
  // Check for white/gray (very low saturation)
  if (hsv.s < 20) {  // Increased from 15
    if (hsv.v > 65) return "WHITE";
    if (hsv.v > 35) return "GRAY";
    return "DARK GRAY";
  }
  
  // Detect colors based on ACTUAL data
  float h = hsv.h;
  
  // RED: 0-15 or 335-360
  if ((h >= 0 && h <= 5) || (h >= 355 && h <= 360)) {
    return "RED";
  }
  // ORANGE: 5-35
  else if (h >= 5 && h <= 35) {
    return "ORANGE";
  }
  // YELLOW: 35-60
  else if (h >= 35 && h <= 60) {
    return "YELLOW";
  }
  // GREEN: 60-155
  else if (h >= 60 && h <= 155) {
    return "GREEN";
  }
  // BLUE: 195-260
  else if (h >= 195 && h <= 260) {
    return "BLUE";
  }
  // PURPLE: 320-355
  else if (h >= 320 && h <= 355) {
    return "PURPLE";
  }
  
  return "UNKNOWN";
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

  calibrateWithWhite();
  
  Serial.println("RED,GREEN,BLUE,RAW");
}

void loop() {
  int redVal = readWithLED(RED_LED);
  int greenVal = readWithLED(GREEN_LED);
  int blueVal = readWithLED(BLUE_LED);

  HSV hsv = rgbToHSV(redVal, greenVal, blueVal);

  String detected = detectColorHSV(hsv);

  Serial.print("R: ");
  Serial.print(redVal);
  Serial.print(" G: ");
  Serial.print(greenVal);
  Serial.print(" B: ");
  Serial.print(blueVal);
  Serial.print(" | HSV: ");
  Serial.print(hsv.h, 1);
  Serial.print("° ");
  Serial.print(hsv.s, 1);
  Serial.print("% ");
  Serial.print(hsv.v, 1);
  Serial.print("% -> ");
  Serial.println(detected);

  delay(1000);
}