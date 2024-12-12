// importing the necessary libraries
#include <SoftwareSerial.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>

// defining the pins for the components and variables
SoftwareSerial sim(6, 7);     //RX = 6, TX = 7

// specifying the phone number where the text message will be sent to (country code +63 for Philippines)
String number = "+639060616083";
const char* phoneNumber = "+639060616083";

// Initialize the I²C LCD (0x27 is a common I²C address; yours might differ)
LiquidCrystal_I2C lcd(0x27, 16, 2); // 16 columns, 2 rows

int buzzerPin = 9;      // Pin connected to the buzzer
int lightPin = 13;       // Pin connected to the fairy lights
int buttonPin = 2;      // Pin connected to the button
bool isPlaying = false; // State to track if melody is currently playing
unsigned long lastButtonPress = 0; // To track last button press time
const unsigned long debounceDelay = 200; // Debounce delay in milliseconds

void setup() {
  Serial.begin(9600);               // Start serial communication
  sim.begin(9600);                  // Initialize the GSM module

  pinMode(buzzerPin, OUTPUT);        // Set the buzzer pin as output
  pinMode(lightPin, OUTPUT);        // Set the fairy lights pin as output
  pinMode(buttonPin, INPUT_PULLUP); // Set the button pin as input with internal pull-up

  // Initialize the LCD and print the initial message
  lcd.init();                       // Initialize the I²C LCD
  
}

void loop() {
  // Check if button is pressed (LOW state due to pull-up resistor)
  if (digitalRead(buttonPin) == LOW && !isPlaying) {
    isPlaying = true;              // Set state to indicate that the melody is playing
    
    lcd.backlight();
    lcd.clear();                   // Clear LCD before showing new message
    lcd.print("If no response,");  // First line of the message
    lcd.setCursor(0, 1);           // Move to the second line
    lcd.print("call instead.");

    playMelody();                  // Plays the doorbell sound
    checkSIM();    
    MakeCall();                 // 

    lcd.clear();                   // Reset LCD to default message
    isPlaying = false;             // Reset state after melody is finished
    lcd.noBacklight();
  }
}

// Function to play the melody and flicker the lights
void playMelody() {
  digitalWrite(lightPin, HIGH);
  tone(buzzerPin, 500, 500);
  delay(500);      // Play the melody
  digitalWrite(lightPin, LOW);
  delay(500);

  digitalWrite(lightPin, HIGH);
  tone(buzzerPin, 500, 500);
  delay(500);      // Play the melody
  digitalWrite(lightPin, LOW);
  delay(500);

  digitalWrite(lightPin, HIGH);
  tone(buzzerPin, 500, 500);
  delay(500);      // Play the melody
  digitalWrite(lightPin, LOW);
  delay(500);

  digitalWrite(lightPin, HIGH);
  tone(buzzerPin, 500, 500);
  delay(500);      // Play the melody
  digitalWrite(lightPin, LOW);
  delay(500);
}

// Optional: Function to validate phone number format (if dynamic input is used in the future)
bool isValidNumber(String number) {
  if (number.startsWith("+") && number.length() == 13) {
    return true;
  } 
  return false;
} 

void checkSIM() {
  sim.println("AT+CPIN?");
  delay(1000); // Wait for the module to respond

  while (sim.available()) {
    String response = sim.readString();
    Serial.println("SIM Response: " + response);
    if (response.indexOf("READY") != -1) {
      Serial.println("SIM is detected and ready.");
    } else if (response.indexOf("SIM PIN") != -1) {
      Serial.println("SIM requires a PIN.");
    } else if (response.indexOf("SIM PUK") != -1) {
      Serial.println("SIM is locked with a PUK.");
    } else {
      Serial.println("No SIM detected or an unknown error occurred.");
    }
  }
}

// Function to make a call
void MakeCall() {
  sim.println("ATD" + String(phoneNumber) + ";"); // Send the ATD command with the phone number
  delay(10000); // Allow time for the GSM module to process the command
  sim.println("ATH"); // Send the ATD command with the phone number
  // Check for response from the GSM module and print it to the Serial Monitor
  while (sim.available()) {
    Serial.print(char(sim.read())); // Print response from the GSM module
  }
}
