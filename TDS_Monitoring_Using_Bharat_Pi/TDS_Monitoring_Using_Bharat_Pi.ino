/*
   TDS Code with Curve fitting data of


  Name: TDS monitoring code for Bharat pi
  Description: Accurately Measure the TDS values and give it in PPM
  Author: Naveen.G
  Version: 1.0.1
  Created Date: 29 Aug 2023

  Revision History Table:
  ------------------------------------------
  Date      | Firmware Version | Comments
  ------------------------------------------
  29 Aug 2023 -    1001         - Changed the code to have a Curve fitting, Measure multiple TDS values with the Meter and note down voltage.
                                  Use curve fitting sites like https://atozmath.com/ to identify the values for 4 coefficients
                                  TDS works like having a Cubic relationship with the voltage value : d*x^3 + c*x^2 + b*x + a


  x : 0.25 : y : 0
  X : 0.36 : y : 116
  X : 0.56 : y : 326
  x : 0.81 : y : 616
  x : 1.01 : y : 1371

  a = -596.5795
  b = 3609.5997
  c = -6113.7654
  d = 4409.4202
  y = -596.5795 + 3609.5997x - 6113.7654x2 + 4409.4202x3

  here x is the voltage and y is the TDS value calculated for Bharat Pi


*/





#define TdsSensorPin 12
#define VREF 3.3           // analog reference voltage(Volt) of the ADC
#define SCOUNT 30          // sum of sample point
int analogBuffer[SCOUNT];  // store the analog value in the array, read from ADC
int analogBufferTemp[SCOUNT];
int analogBufferIndex = 0, copyIndex = 0;
float averageVoltage = 0, tdsValue = 0, temperature = 20;
void setup() {
  Serial.begin(115200);
  pinMode(TdsSensorPin, INPUT_PULLUP);
}
void loop() {
  static unsigned long analogSampleTimepoint = millis();
  if (millis() - analogSampleTimepoint > 40U)  //every 40 milliseconds,read the analog value from the ADC
  {
    analogSampleTimepoint = millis();
    int analogVal = analogRead(TdsSensorPin);
    analogBuffer[analogBufferIndex] = analogRead(TdsSensorPin);  //read the analog value and store into the buffer
    analogBufferIndex++;
    if (analogBufferIndex == SCOUNT)
      analogBufferIndex = 0;
  }
  static unsigned long printTimepoint = millis();
  if (millis() - printTimepoint > 800U) {
    printTimepoint = millis();
    for (copyIndex = 0; copyIndex < SCOUNT; copyIndex++) {
      analogBufferTemp[copyIndex] = analogBuffer[copyIndex];
    }
    float medianNum = getMedianNum(analogBufferTemp, SCOUNT);
    averageVoltage = medianNum * (float)VREF / 4096.0;  // read the analog value more stable by the median filtering algorithm, and convert to voltage value
    float compensationVolatge = averageVoltage;        //temperature compensation
    Serial.print("Voltage");
    Serial.println(compensationVolatge);
    tdsValue = (4409.4202 * compensationVolatge * compensationVolatge * compensationVolatge - 6113.7654 * compensationVolatge * compensationVolatge + 3609.5997 * compensationVolatge - 596.5795); //convert voltage value to tds value
    if (tdsValue < 0) {
      tdsValue = 0;
    }
    Serial.print("TDS Value:");
    Serial.print(tdsValue, 0);
    Serial.println("ppm");
  }
}
int getMedianNum(int bArray[], int iFilterLen) {
  int bTab[iFilterLen];
  for (byte i = 0; i < iFilterLen; i++)
    bTab[i] = bArray[i];
  int i, j, bTemp;
  for (j = 0; j < iFilterLen - 1; j++) {
    for (i = 0; i < iFilterLen - j - 1; i++) {
      if (bTab[i] > bTab[i + 1]) {
        bTemp = bTab[i];
        bTab[i] = bTab[i + 1];
        bTab[i + 1] = bTemp;
      }
    }
  }
  if ((iFilterLen & 1) > 0)
    bTemp = bTab[(iFilterLen - 1) / 2];
  else
    bTemp = (bTab[iFilterLen / 2] + bTab[iFilterLen / 2 - 1]) / 2;
  return bTemp;
}
