/*
  Modified sketch to control a 28BYJ-48 stepper motor 
  with ULN2003 driver board and Arduino UNO, an additional servo motor,
  an infrared sensor for object detection with count for different waste types, 
  and an OLED display for displaying the count of different waste types.
*/

// Include the necessary libraries:
#include "Stepper.h"
#include <Servo.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

// Define number of steps per rotation for stepper motor:
const int stepsPerRevolution = 2048;

// OLED display width and height
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 32

// Define the OLED reset pin (use -1 if no reset pin)
#define OLED_RESET -1

// Create an OLED display object
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

// Wiring for stepper motor:
// Pin 8 to IN1 on the ULN2003 driver
// Pin 9 to IN2 on the ULN2003 driver
// Pin 10 to IN3 on the ULN2003 driver
// Pin 11 to IN4 on the ULN2003 driver

// Create stepper object called 'myStepper', note the pin order:
Stepper myStepper = Stepper(stepsPerRevolution, 8, 10, 9, 11);

// Create servo object called 'myServo':
Servo myServo;

// Define servo and IR sensor pins
const int servoPin = 6;
const int irSensorPin = 7;  // Pin where the IR sensor is connected

// Initialize count variables for different waste types
int biodegradableCount = 0;
int recyclableCount = 0;
int nonRecyclableCount = 0;
int eWasteCount = 0;

// Flags to track detection and readiness of IR sensor
bool objectDetected = false;
bool irSensorReady = true;

// Array to store detection signals
const int maxDetections = 5;
char detectionArray[maxDetections];
int detectionIndex = 0;  // Index to store detections in the array

void setup() {
  // Attach the servo on pin 6
  myServo.attach(servoPin);

  // Set the speed for the stepper motor to 10 rpm for increased torque:
  myStepper.setSpeed(7);

  // Begin Serial communication at a baud rate of 9600:
  Serial.begin(9600);

  // Initialize the IR sensor pin as input
  pinMode(irSensorPin, INPUT);

  // Ensure the servo starts at 0 degrees
  myServo.write(0);

  // Initialize the OLED display
  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) { // Address 0x3C for 128x32
    Serial.println(F("SSD1306 allocation failed"));
    for (;;);
  }
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0, 0);
  display.print("Waste Type Counts:");
  display.display();
}

void updateDisplay() {
  // Clear and update display with the latest counts for each waste type
  display.clearDisplay();
  
  // First line: B: and R:
  display.setCursor(0, 0);
  display.print("B: ");
  display.print(biodegradableCount);
  display.print("   R: ");
  display.print(recyclableCount);
  
  // Second line: N: and E:
  display.setCursor(0, 16);
  display.print("N: ");
  display.print(nonRecyclableCount);
  display.print("   E: ");
  display.print(eWasteCount);
  
  display.display();
}

void resetDetectionArray() {
  // Clear the detection array by resetting each element to a default value
  for (int i = 0; i < maxDetections; i++) {
    detectionArray[i] = ' ';  // or any other default character
  }
  detectionIndex = 0;  // Reset the index to start fresh
}

void loop() {
  // Only detect objects if the IR sensor is ready for new detections
  if (irSensorReady && digitalRead(irSensorPin) == LOW && detectionIndex < maxDetections) {  
    // Store detection signal in the array
    detectionArray[detectionIndex] = 'D';  // Store detection signal
    detectionIndex++;  // Increment the index for the next detection

    // Mark the object as detected
    objectDetected = true;

    // Update the OLED display
    updateDisplay();

    // If the array has at least one detection, send the signal
    if (detectionIndex > 0) {
      Serial.write('D');
      Serial.print("Object detected!");
    }

    // Block further detections until the process completes
    irSensorReady = false;
  }

  // After sending the detection signal, process the motor actions
  if (objectDetected && detectionIndex > 0) {
    if (Serial.available() > 0) {
      // Read the incoming byte from Python
      char incomingByte = Serial.read();

      // Print for debugging purposes
      Serial.print("Received: ");
      Serial.println(incomingByte);

      // Perform action based on the character received
      if (incomingByte == '1') {
        Serial.println("Biodegradable Waste detected");
        biodegradableCount++;  // Increment biodegradable count
        myStepper.step(0);  // Original position (no step)
        delay(500);
        myServo.write(90);
        delay(3000);
        myStepper.step(0);  // Return to original position
      } 
      else if (incomingByte == '2') {
        Serial.println("Recyclable Waste detected");
        recyclableCount++;  // Increment recyclable count
        myStepper.step(1600);  // Half revolution clockwise
        myServo.write(90);
        delay(3000);
        myStepper.step(-1600);  // Return to original position
      } 
      else if (incomingByte == '3') {
        Serial.println("Non-Recyclable Waste detected");
        nonRecyclableCount++;  // Increment non-recyclable count
        myStepper.step(3100);  // Two full revolutions clockwise
        myServo.write(90);
        delay(3000);
        myStepper.step(-3100);  // Return to original position
      } 
      else if (incomingByte == 'e') {
        Serial.println("E-Waste detected");
        eWasteCount++;  // Increment e-waste count
        myStepper.step(-1600);  // Half revolution counterclockwise
        myServo.write(90);
        delay(3000);
        myStepper.step(1600);  // Return to original position
      }

      // Reset the servo back to 0 degrees
      myServo.write(0);

      // Reset flags after completing the motor actions
      irSensorReady = true;  // Ready for new detection
      objectDetected = false;  // Reset object detected flag

      // Clear the detection array after completing the motor actions
      resetDetectionArray();  // Clear the detection array
    }
  }
}
