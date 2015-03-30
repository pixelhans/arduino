/*
HMC5883L_Example.pde - Example sketch for integration with an HMC5883L triple axis magnetomerwe.
Copyright (C) 2011 Love Electronics (loveelectronics.co.uk)

This program is free software: you can redistribute it and/or modify
it under the terms of the version 3 GNU General Public License as
published by the Free Software Foundation.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.

*/

// Reference the I2C Library
#include <Wire.h>
// Reference the HMC5883L Compass Library
#include <HMC5883L.h>

// Store our compass as a variable.
HMC5883L compass;
// Record any errors that may occur in the compass.
int error = 0;

int flip=11;                    // dot direction one Pin
int flop=10;                    // dot direction two Pin
bool flipped = false;

float diffAngle;
float diffAngleDegrees;

int switchPin = 8;

// Out setup routine, here we will configure the microcontroller and compass.
void setup()
{
  pinMode(switchPin,INPUT_PULLUP);
  
  pinMode(flip,OUTPUT);        // pin A declared as OUTPUT
  pinMode(flop,OUTPUT);        // pin B declared as OUTPUT

  // Initialize the serial port.
  Serial.begin(9600);

  Serial.println("Starting the I2C interface.");
  Wire.begin(); // Start the I2C interface.

  Serial.println("Constructing new HMC5883L");
  compass = HMC5883L(); // Construct a new HMC5883 compass.
    
  Serial.println("Setting scale to +/- 1.3 Ga");
  error = compass.SetScale(1.3); // Set the scale of the compass.
  if(error != 0) // If there is an error, print it out.
    Serial.println(compass.GetErrorText(error));
  
  Serial.println("Setting measurement mode to continous.");
  error = compass.SetMeasurementMode(Measurement_Continuous); // Set the measurement mode to Continuous
  if(error != 0) // If there is an error, print it out.
    Serial.println(compass.GetErrorText(error));
    
  //reset flipdot
  Serial.println("Reseting FlipDot.");
  digitalWrite(flip,LOW);     // dot direction one pin switched to HIGH (GND)
  digitalWrite(flop,HIGH);  // dot direction two pin switched to LOW  (+5V)
  delay(10);
  digitalWrite(flop,LOW);  // dot direction two pin switched to LOW  (GND)
  flipped = false;
}

// Our main program loop.
void loop()
{
  // Retrive the raw values from the compass (not scaled).
  MagnetometerRaw raw = compass.ReadRawAxis();
  // Retrived the scaled values from the compass (scaled to the configured scale).
  MagnetometerScaled scaled = compass.ReadScaledAxis();
  
  if (digitalRead(switchPin) == LOW) {
    //calculate angle between compass vector and vector(1,0,0)
    diffAngle = acos((scaled.XAxis)/(sqrt(square(scaled.XAxis)+square(scaled.YAxis)+square(scaled.ZAxis))));
    //convert rad to deg
    diffAngleDegrees = diffAngle*180/M_PI;
    
    //print values over serial
    Serial.print("diffAngle(RAD): ");
    Serial.print(diffAngle);
    Serial.print(" diffAngle(DEG): ");
    Serial.println(diffAngleDegrees);
    
    //turn on flipdot if heading is +-10Â° to north, else turn off 
    if (diffAngleDegrees <= 10 && flipped == false) 
    {
      digitalWrite(flip,HIGH);     // dot direction one pin switched to HIGH (+5V)
      digitalWrite(flop,LOW);      // dot direction two pin switched to LOW  (GND)
      delay(10);
      digitalWrite(flip,LOW);    // dot direction one pin switched to LOW (GND)
      flipped = true;
      Serial.println("FLIP");
    } 
    else if (diffAngleDegrees > 10 && flipped == true)
    {
      digitalWrite(flip,LOW);     // dot direction one pin switched to HIGH (GND)
      digitalWrite(flop,HIGH);  // dot direction two pin switched to LOW  (+5V)
      delay(10);
      digitalWrite(flop,LOW);  // dot direction two pin switched to LOW  (GND)
      flipped = false;
      Serial.println("FLOP");
     }
   } 
   else
   {
    // Calculate heading when the magnetometer is level, then correct for signs of axis.
    float heading = atan2(scaled.YAxis, scaled.XAxis);
    
    // Once you have your heading, you must then add your 'Declination Angle', which is the 'Error' of the magnetic field in your location.
    // Find yours here: http://www.magnetic-declination.com/
    // Mine is: 2ï¿½ 37' W, which is 2.617 Degrees, or (which we need) 0.0456752665 radians, I will use 0.0457
    // If you cannot find your Declination, comment out these two lines, your compass will be slightly off.
    float declinationAngle = 0.0457;
    heading += declinationAngle;
    
    // Correct for when signs are reversed.
    if(heading < 0)
      heading += 2*PI;
      
    // Check for wrap due to addition of declination.
    if(heading > 2*PI)
      heading -= 2*PI;
     
    // Convert radians to degrees for readability.
    float headingDegrees = heading * 180/M_PI; 
    
    //print values over serial
    Serial.print("heading(RAD): ");
    Serial.print(heading);
    Serial.print(" heading(DEG): ");
    Serial.println(headingDegrees);
    
    //turn on flipdot if heading is +-10Â° to north, else turn off 
    if ((headingDegrees <= 10 || headingDegrees >= 350) && flipped == false) 
    {
      digitalWrite(flip,HIGH);     // dot direction one pin switched to HIGH (+5V)
      digitalWrite(flop,LOW);      // dot direction two pin switched to LOW  (GND)
      delay(10);
      digitalWrite(flip,LOW);    // dot direction one pin switched to LOW (GND)
      flipped = true;
      Serial.println("FLIP");
    } 
    else if ((headingDegrees > 10 && headingDegrees < 350) && flipped == true)
    {
      digitalWrite(flip,LOW);     // dot direction one pin switched to HIGH (GND)
      digitalWrite(flop,HIGH);  // dot direction two pin switched to LOW  (+5V)
      delay(10);
      digitalWrite(flop,LOW);  // dot direction two pin switched to LOW  (GND)
      flipped = false;
      Serial.println("FLOP");
     }
   }

  

  delay(66);
}
