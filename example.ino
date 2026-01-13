// The focus of this code is to make the continuous rotation servo motor MG996R travel an angle in degrees.
// Contains minimal numerical and logical errors that will be fixed later.

#define ServoPin 5
#include <Servo.h>

Servo myServo;

const float pi = 3.142;

// This represents the time it takes for the motor to travel one revolution
const float time_period_for_one = 1.515;                 // in seconds
// This is the angular frequency
float w = 2 * pi / time_period_for_one;                  // Angular Frequency w = deg2rad(360) / 1.515

float req_time = 0;           // Initialize required time to reach a specific angle

// For Control
float ref = 0;                // Initialize Reference Angle
float for_ref_time = 0;       // Initialize time required to come back to reference angle

// Previous and Next
int angle = 0;
int previous_angle = 0;
int next_angle = 0;

void setup() {
  Serial.begin(9600);
  myServo.attach(ServoPin);
}

void loop() 
{
  Serial.println("Enter Angle: ");
  while (Serial.available() == 0); 
  
  next_angle = Serial.parseInt();

  angle = previous_angle - next_angle;

  if(angle == 1234)             // Code to come back to reference angle
  {
    Reference(ref, for_ref_time);
    ref = 0;
    for_ref_time = 0;
    angle = 0; previous_angle = 0; next_angle = 0;
  }
  else
  {
    if(angle > 0) 
    {
      float radians = angle * pi / 180;                 // Convert Angle to Radians
      req_time = radians / w * 1000;                    // Calculate Time = Angle / w * 1000
      CCWROT(radians, req_time);
      ref += angle;                                     // Update Reference Angle Distance
    }

    if(angle < 0)
    {
      float radians = -angle * pi / 180;
      req_time = radians / w * 1000;
      CWROT(radians, req_time);
      ref -= angle;
    }
  }

  Serial.print("Angle : "); Serial.println(angle);
  Serial.print("Ref Distance : "); Serial.println(ref);
  Serial.print("Ref Time : "); Serial.println(for_ref_time);
  if(ref > 360)
  {
    ref = 360 - ref;          // To keep reference angle periodic
  }

  previous_angle = angle;
}

// =============================================
void CWROT(float rad, float req_time)
{
  unsigned long startTime = millis();                 // Starting time of function
  
  while (millis() - startTime < req_time) {           // While time of function less than required time to reach specific 'rad' angle
    myServo.write(0);                                 // Rotate servo in maximum speed
    delay(50);
  }

  Serial.println(millis() - startTime);
  for_ref_time += millis() - startTime;               // Update time required to go back to reference angle
  myServo.write(90);                                  // Stop the servo
}

// =============================================
void CCWROT(float rad, float req_time)
{
  unsigned long startTime = millis();               // Starting time of function
  
  while (millis() - startTime < req_time) {         // While time of function less than required time to reach specific 'rad' angle
    myServo.write(180);                             // Rotate servo to maximum speed
    delay(50);
  }

  Serial.println(millis() - startTime);
  for_ref_time -= millis() - startTime;             // Update time required to go back to reference angle
  myServo.write(90);                                // Stop the servo
}

// =============================================
// This function helps take the motor back to the position where it started.
void Reference(float ref, float for_ref_time)
{
  unsigned long startTime = millis();
  while(millis() - startTime < for_ref_time)
  {
    if(ref > 0)
    {
      myServo.write(180);
      delay(50);
    }
    else if(ref < 0)
    {
      myServo.write(0);
      delay(50);
    }
  }
  myServo.write(90);
}
