// Pin definitions
#define BUTTON_PIN 14    // GPIO pin connected to the button
#define LED1_PIN 18       // GPIO pin for LED 1
#define LED2_PIN 23       // GPIO pin for LED 2
#define LED3_PIN 22      // GPIO pin for LED 3
#define LED4_PIN 12     // GPIO pin for LED 4
#define LED5_PIN 27       // GPIO pin for LED 5

// Variables
int currentLED = 0;  // Tracks which LED is currently ON

void setup() {
  // Initialize the button pin as input with an internal pull-up resistor
  pinMode(BUTTON_PIN, INPUT_PULLUP);
  
  // Initialize LED pins as outputs
  pinMode(LED1_PIN, OUTPUT);
  pinMode(LED2_PIN, OUTPUT);
  pinMode(LED3_PIN, OUTPUT);
  pinMode(LED4_PIN, OUTPUT);
  pinMode(LED5_PIN, OUTPUT);

  // Turn off all LEDs initially
  digitalWrite(LED1_PIN, LOW);
  digitalWrite(LED2_PIN, LOW);
  digitalWrite(LED3_PIN, LOW);
  digitalWrite(LED4_PIN, LOW);
  digitalWrite(LED5_PIN, LOW);

  // Begin Serial communication
  Serial.begin(115200);
}

void loop() {
  // Read the button state
  static bool lastButtonState = HIGH;  // Tracks the previous state of the button
  bool buttonState = digitalRead(BUTTON_PIN);

  // Check for a button press (LOW because of pull-up resistor)
  if (buttonState == LOW && lastButtonState == HIGH) {
    // Turn off the current LED
    turnOffAllLEDs();

    // Move to the next LED
    currentLED = (currentLED + 1) % 5;  // Cycle through 0 to 4

    // Turn on the new LED
    turnOnLED(currentLED);

    // Print the current LED number
    Serial.print("Current LED: ");
    Serial.println(currentLED + 1);

    // Debounce delay
    delay(200);
  }

  lastButtonState = buttonState;  // Update the button state
}

void turnOffAllLEDs() {
  digitalWrite(LED1_PIN, LOW);
  digitalWrite(LED2_PIN, LOW);
  digitalWrite(LED3_PIN, LOW);
  digitalWrite(LED4_PIN, LOW);
  digitalWrite(LED5_PIN, LOW);
}

void turnOnLED(int ledIndex) {
  switch (ledIndex) {
    case 0: digitalWrite(LED1_PIN, HIGH); break;
    case 1: digitalWrite(LED2_PIN, HIGH); break;
    case 2: digitalWrite(LED3_PIN, HIGH); break;
    case 3: digitalWrite(LED4_PIN, HIGH); break;
    case 4: digitalWrite(LED5_PIN, HIGH); break;
  }
}
