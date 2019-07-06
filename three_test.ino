#include <HDC2010.h>
#include <icp101xx.h>

// Get sensor at below address
#define HDCADDR 0x40
HDC2010 hdcsensor(HDCADDR);
// ICP sensor
ICP101xx icpsensor;

// CO; Requires individual calibration, see link in Carbon_Monoxide
float R0 = 8100;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);

  // Start HDC sensor, soft reset
  hdcsensor.begin();
  hdcsensor.reset();

  // Set measurements to temperature and humidity 
  // (check libary for just humidity, since the other 
  // sensor might have better temp readings)
  hdcsensor.setMeasurementMode(TEMP_AND_HUMID); 
  // Set measurement frequency
  hdcsensor.setRate(ONE_HZ);
  // Accuracy
  hdcsensor.setTempRes(FOURTEEN_BIT);
  hdcsensor.setHumidRes(FOURTEEN_BIT);

  icpsensor.begin();
}

void loop() {  
  /* CO */
  // Read CO value off A0
  float sensorValue = analogRead(A0);
  // Calculate voltage, gas ratios
  float sensor_volt = sensorValue/1024*5.0;
  float RS_gas = (5.0-sensor_volt)/sensor_volt;
  float ratio = RS_gas / R0;
  // Calculate PPM
  float x = 1538.46 * ratio;
  float ppm = pow(x,-1.709);
  Serial.print("CO: ");
  Serial.print(ppm);
  Serial.print(" ppm, ");

  /* Temp & Relative Pressure */
  if (!icpsensor.isConnected()) {
    Serial.print("ICP sensor not responding, ");
  } else {
    // Start measurement cycle, waiting until it is completed.
    // Optional: Measurement mode
    //    sensor.FAST: ~3ms
    //    sensor.NORMAL: ~7ms (default)
    //    sensor.ACCURATE: ~24ms
    //    sensor.VERY_ACCURATE: ~95ms
    icpsensor.measure(icpsensor.VERY_ACCURATE);

    // Read and output measured temperature in Celsius and pressure in Pascal.
    Serial.print("Temp1: ");
    Serial.print(icpsensor.getTemperatureC());
    Serial.print(" C, ");
    Serial.print("Pressure: ");
    Serial.print(icpsensor.getPressurePa());
    Serial.print(" Pa, ");
  }
  
  /* Temp and Humidity */
  // Get HDC measurement data
  hdcsensor.triggerMeasurement();
  
  // Temperature of -40 indicates sensor isn't responding
  float temp = hdcsensor.readTemp();
  if (temp == -40.0) {
    Serial.println("HDC not responding");
  } else {
    Serial.print("Temp2: ");
    Serial.print(temp);
    Serial.print(" C, Humidity: ");
    Serial.print(hdcsensor.readHumidity());
    Serial.println("%");
  }

  delay(1000);
}
