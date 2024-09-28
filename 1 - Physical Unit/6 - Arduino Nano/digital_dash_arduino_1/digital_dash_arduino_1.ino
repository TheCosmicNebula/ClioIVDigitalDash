// digital inputs
int stalk_plus = 12;
int stalk_minus = 11;

int oil_pressure = 8;
int brake_fluid = 7;
int handbrake = 4;

// analogue inputs
int fuel_level = A1;
int ext_temp = A2;

// digital values
unsigned int stalk_plus_ = 0;
unsigned int stalk_minus_ = 0;

unsigned int oil_pressure_ = 0;
unsigned int brake_fluid_ = 0;
unsigned int handbrake_ = 0;

// analogue values
unsigned int ext_temp_  = 0;
unsigned int fuel_level_ = 0;

unsigned int placeholder = 0;

int output[7] = {0};

void setup()
{
  pinMode(stalk_plus, INPUT_PULLUP);
  pinMode(stalk_minus, INPUT_PULLUP);
  
  pinMode(oil_pressure, INPUT_PULLUP);
  pinMode(brake_fluid, INPUT_PULLUP);
  pinMode(handbrake, INPUT_PULLUP);

  pinMode(fuel_level, INPUT);
  pinMode(ext_temp, INPUT);

  // init serial
  Serial.begin(115200);
  delay(100);

  Serial.println("Arduino ONLINE");
}

void loop() {
  ReadDigitalStatuses();
  ReadAnalogStatuses();
  SendCANFramesToSerial();

  /*
  for (int i = 0; i < 5; i++) 
  {
    Serial.println(output[i]);
  }
  */
  
  delay(5);
}

void ReadDigitalStatuses() {
  output[0] = digitalRead(handbrake) == HIGH ? 0 : 1;
  output[1] = digitalRead(stalk_plus) == HIGH ? 0 : 1;
  output[2] = digitalRead(stalk_minus) == HIGH ? 0 : 1;
  output[3] = digitalRead(oil_pressure) == HIGH ? 0 : 1;
  output[4] = digitalRead(brake_fluid) == HIGH ? 0 : 1;
}

void ReadAnalogStatuses() {
  fuel_level_ = analogRead(fuel_level);
  ext_temp_ = analogRead(ext_temp);
}

void SendCANFramesToSerial() {
  byte buf[8];

  // build & send CAN frames to RealDash.
  // a CAN frame payload is always 8 bytes containing data in a manner
  // described by the RealDash custom channel description XML file
  // all multibyte values are handled as little endian by default.
  // endianess of the values can be specified in XML file if it is required to use big endian values

  // build 1st CAN frame
  memcpy(buf, &ext_temp_, 2);
  memcpy(buf + 2, &fuel_level_, 2);
  memcpy(buf + 4, &output, 1);
  memcpy(buf + 5, &placeholder, 3);

  // write first CAN frame to serial
  SendCANFrameToSerial(197, buf);
  /*
  // build 2nd CAN frame, Arduino digital pins and 2 analog values
  memcpy(buf, &digitalPins, 2);
  memcpy(buf + 2, &analogPins[0], 2);
  memcpy(buf + 4, &analogPins[1], 2);
  memcpy(buf + 6, &analogPins[2], 2);

  // write 2nd CAN frame to serial
  SendCANFrameToSerial(197, buf);

  // build 3rd CAN frame, rest of Arduino analog values
  memcpy(buf, &analogPins[3], 2);
  memcpy(buf + 2, &analogPins[4], 2);
  memcpy(buf + 4, &analogPins[5], 2);
  memcpy(buf + 6, &analogPins[6], 2);

  // write 3rd CAN frame to serial
  SendCANFrameToSerial(197, buf);*/
}

void SendCANFrameToSerial(unsigned long canFrameId, const byte* frameData) {
  // the 4 byte identifier at the beginning of each CAN frame
  // this is required for RealDash to 'catch-up' on ongoing stream of CAN frames
  const byte serialBlockTag[4] = { 0x44, 0x33, 0x22, 0x11 };
  Serial.write(serialBlockTag, 4);

  // the CAN frame id number (as 32bit little endian value)
  Serial.write((const byte*)&canFrameId, 4);

  // CAN frame payload
  Serial.write(frameData, 8);
}

