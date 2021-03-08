      /*
  CAN-EGT to Mega/Micro squirt

  Capture the 0-5 volt output from the CAN-EGT which represents 0-1250 degrees C
  and convert it to a resistance via a digitial potentiometer.  This mimics
  what the MS is expecting for "coolant temperature" for air-cooled cars
  (old VWs!).
  
  To upload to your Gemma or Trinket:
  1) Select the proper board from the Tools->Board Menu
  2) Select USBtinyISP from the Tools->Programmer
  3) Plug in the Gemma/Trinket, make sure you see the green LED lit
  4) For windows, install the USBtiny drivers
  5) Press the button on the Gemma/Trinket - verify you see
     the red LED pulse. This means it is ready to receive data
  6) Click the upload button above within 10 seconds
*/
 
#define SPI_select 1  // SPI chip select for the digital pot.  (same pin as the built-in LED)
#define SPI_clock 2 // SPI clock
#define SPI_digout 0 // SPI serial output

#define EGT_in 2 // From the CAN-EGT's 0-5 volt out

#define Max_R 257 // Maximum resistance setting value.
#define Pot_address 0 // Which on-chip pot to use (0 if it's the only one)

#define numReadings 10 // How many readings to average out for smooth output

int sensor_value;

// The following variables are used to smooth the input reading
// It averages over the last "numReadings" input reads,
// which gives about an average of the last 2 seconds of reads.
int readings[numReadings];  // array to store the input reads
int readIndex = 0;        // which position in the array to update
int total = 0;            // running total of sensor readings
int average = 0;          // averaged output

// the setup routine runs once when you press reset:
void setup() {
  // initialize the digital pin as an output.
  pinMode(SPI_select, OUTPUT);
  pinMode(SPI_clock, OUTPUT);
  pinMode(SPI_digout, OUTPUT);
  //pinMode(EGT_in, INPUT);

  digitalWrite(SPI_select, HIGH);  // LOW triggers the start of comms
  digitalWrite(SPI_clock, LOW);  // Mode 0,0 in the digital pot docs

  read_sensor();

  // Initialize read array to the initial sensor value
  for (int ref = 0; ref < numReadings; ref++) {
    readings[ref] = sensor_value;
    total = total + sensor_value;
  }

}

void read_sensor() {
  // pause to allow the sensor to stabilize
  delay(10);
  sensor_value = analogRead(EGT_in);
  if (sensor_value > Max_R) {
    sensor_value = Max_R;
  }
}

void calculate_average() {
  // subtract the last reading:
  total = total - readings[readIndex];
  // read from the sensor:
  readings[readIndex] = sensor_value;
  // add the reading to the total:
  total = total + readings[readIndex];
  // advance to the next position in the array:
  readIndex = readIndex + 1;

  // if we're at the end of the array...
  if (readIndex >= numReadings) {
    // ...wrap around to the beginning:
    readIndex = 0;
  }

  // calculate the average:
  average = total / numReadings;
}

void send_r_value(int value) {
  // The command is made up of two bytes.
  int byte1 = 0;
  int byte2 = 0;
  
  // Set the clock to low for "mode 0,0"
  digitalWrite(SPI_clock, LOW);
  // Select the chip (also preps it for a message)
  digitalWrite(SPI_select, LOW);
  // Give the chip a moment to prepare
  delayMicroseconds(8);

  /*
   * Commands are sent in two bytes.
   * The first 4 bits indicate the address.  0000 is the first (or only) pot.
   * The next 2 bits are a command.  00 = set the value.
   * The next 10 bits are a value of 0 - 257
   */

  // Clamp at the max value
  if(value > Max_R) {
    value = Max_R;
  }

  // Prepare the data
  // first byte.  Mask the address over the first 4 bits
  byte1 = Pot_address << 4;
  // first byte.  Next two bits are write command - nothing to add for 00
  // first byte.  Next two bits are the upper two bits of the value
  byte1 = byte1 | ((value >> 8) & 0xFF);
  // second byte.  Remaining bits of the value
  byte2 = value & 0xFF;

  // Send the bytes
  shiftOut(SPI_digout, SPI_clock, MSBFIRST, byte1);
  shiftOut(SPI_digout, SPI_clock, MSBFIRST, byte2);

  // Give the chip a moment to think about things
  delayMicroseconds(8);
  
  // close down the connection, we're done
  digitalWrite(SPI_select, HIGH);
  digitalWrite(SPI_clock, LOW);
}

// the loop routine runs over and over again forever:
void loop() {
  read_sensor();
  calculate_average();
  send_r_value(average);
  
  delay(40);
  // clears the select and turns off the Trinket's LED.
  digitalWrite(SPI_select, LOW);
  delay(50);
  // sets the select and turns on the Trinket's LED.
  digitalWrite(SPI_select, HIGH);
}
    
