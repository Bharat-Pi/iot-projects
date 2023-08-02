#define TRIG_PIN 33
#define ECHO_PIN 32
#define BUZZER_PIN 23

void setup() {
  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);
  pinMode(BUZZER_PIN, OUTPUT);
  Serial.begin(115200);
}

void loop() {
  long duration, distance_cm;
  
 
  digitalWrite(TRIG_PIN, LOW);
  delayMicroseconds(2);
  

  digitalWrite(TRIG_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG_PIN, LOW);
  
  // Measure the echo pulse duration
  duration = pulseIn(ECHO_PIN, HIGH);
  
  // Calculate the distance in centimeters
  distance_cm = duration * 0.034 / 2;

  // Print the distance to the serial monitor
  Serial.print("Distance: ");
  Serial.print(distance_cm);
  Serial.println(" cm");

  // Check if an object is within a certain distance (adjust the threshold as needed)
  if (distance_cm < 25) {
    // Object detected, activate the buzzer
    digitalWrite(BUZZER_PIN, HIGH);
    delay(500);  // Buzzer ON time (adjust as needed)
    digitalWrite(BUZZER_PIN, LOW);
    delay(500);  // Buzzer OFF time (adjust as needed)
  } else {
    digitalWrite(BUZZER_PIN, LOW); // Turn off the buzzer
  }

  delay(100); // Adjust the delay as needed to control the sensor update rate
}
