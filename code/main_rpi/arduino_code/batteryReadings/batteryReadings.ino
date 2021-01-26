#include "LTC2944.h"

const unsigned int fullCapacity = 3000; // Maximum value is 3 Ah

LTC2944 gauge(1300); // Takes R_SENSE value (in milliohms) as constructor argument, can be omitted if using CJMCU-294

void setup() {
  Serial.begin(9600);
  Serial.println("LTC2944 Battery Readings Example");
  Serial.println();

  Wire.begin();

  while (gauge.begin() == false) {
    Serial.println("Failed to detect LTC2944!");
    delay(5000);
  }

  gauge.setBatteryCapacity(fullCapacity);
  gauge.setBatteryToFull(); // Sets accumulated charge registers to the maximum value
  gauge.setADCMode(ADC_MODE_SLEEP); // In sleep mode, voltage and temperature measurements will only take place when requested
  gauge.startMeasurement();
}

void loop() {
/*  unsigned int raw = gauge.getRawAccumulatedCharge();
  Serial.print(F("Raw Accumulated Charge: "));
  Serial.println(raw, DEC);
*/
  
  float capacity = gauge.getRemainingCapacity();
  Serial.print(F("Battery Capacity: "));
  Serial.print(capacity, 3);
  Serial.print(F(" / "));
  Serial.print(fullCapacity, DEC);
  Serial.println(F(" mAh"));

  Serial.print(F("SOC: "));
  float soc = (capacity/fullCapacity)*100;
  Serial.println(soc);

  float voltage = gauge.getVoltage();
  Serial.print(F("Voltage: "));
  Serial.print(voltage, 3);
  Serial.println(F(" V"));

  float temperature = gauge.getTemperature();
  Serial.print(F("Temperature: "));
  Serial.print(temperature, 2);
  Serial.println(F(" 'C"));

  float current = gauge.getCurrent();
  //int currentInt = (int)current;
  Serial.print(F("Current: "));
  Serial.print(current, 2);
  Serial.println(F(" 'mA"));

  Serial.println();

  delay(5000);
}
