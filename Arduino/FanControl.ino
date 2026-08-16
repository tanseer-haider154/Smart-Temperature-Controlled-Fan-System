const int tempPin = A0;
const int fanPin = 9;

void setup() {
  Serial.begin(9600);
  pinMode(fanPin, OUTPUT);

  Serial.println("========================================");
  Serial.println("  SMART TEMPERATURE CONTROL SYSTEM");
  Serial.println("========================================");
  Serial.println("ADC  | Voltage | Temp(C) | Fan% | Status");
  Serial.println("----------------------------------------");
}

void loop() {
  long sum = 0;
  for (int i = 0; i < 10; i++) {
    sum += analogRead(tempPin);
    delay(10);
  }
  int adcValue = sum / 10;

  float voltage = adcValue * (5.0 / 1023.0);
  float temperature = voltage * 100.0;

  int pwmValue = 0;
  int fanPercent = 0;

  if (temperature <= 25.0) {
    pwmValue = 148;
    fanPercent = 58;
  }
  else if (temperature <= 37.0) {
    pwmValue = 199;
    fanPercent = 78;
  }
  else if (temperature <= 45.0) {
    pwmValue = 230;
    fanPercent = 90;
  }
  else if (temperature <= 50.0) {
    pwmValue = 255;
    fanPercent = 100;
  }
  else {
    pwmValue = 0;
    fanPercent = 0;
  }

  analogWrite(fanPin, pwmValue);

  Serial.print(adcValue);
  Serial.print(" | ");
  Serial.print(voltage, 2);
  Serial.print("V  | ");
  Serial.print(temperature, 1);
  Serial.print("   | ");
  Serial.print(fanPercent);
  Serial.print("%  | ");

  if (temperature <= 25.0) Serial.println("NORMAL");
  else if (temperature <= 37.0) Serial.println("NORMAL");
  else if (temperature <= 45.0) Serial.println("HIGH");
  else if (temperature <= 50.0) Serial.println("MAX");
  else Serial.println("SHUTDOWN!");

  delay(1000);
}
