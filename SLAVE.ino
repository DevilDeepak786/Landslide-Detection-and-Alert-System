//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>> 04_Final_Slave
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// A brief explanation of how this project works.                                                                                           //
// - The Master sends data containing the Switch state and Potentiometer value to the Slave.                                                //
// - If the Slave receives data from the Master, the Slave will adjust the state of LED_1 based on the Switch state and                     //
//   the Brightness of LED_2 based on the Potentiometer value received from the Master.                                                     //
// - After the Slave receives data from the Master, the Slave sends data back to the master,                                                //
//   the data contains the temperature value, humidity value, state of LED_1, brightness of LED_2, and the tilt sensor state.              //
// - The point is that the Master sends data to the Slave and the Slave only sends data to the Master if it receives data from the Master.  //
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//----------------------------------------Include Library.
#include <SoftwareSerial.h>
#include "DHT.h"
//----------------------------------------

// Defines the DHT11 PIN and DHT sensor type.
#define DHTPIN    8
#define DHTTYPE   DHT11

// Defines the PIN for the LEDs.
#define LED_1_PIN   2
#define LED_2_PIN   3

// Defines the PIN for the Tilt Sensor.
#define TILT_SENSOR_PIN 6

// Defines the master/destination address.
#define master_Address 1

float h;
float t;
int tiltState;

// Create a DHT object as “dht11” and set the PIN and DHT Type.
DHT dht11(DHTPIN, DHTTYPE);

// Create a SoftwareSerial object as "ReyaxLoRa" and set the SoftwareSerial RX and TX PINs.
SoftwareSerial ReyaxLoRa(5, 4); //--> RX, TX

//________________________________________________________________________________ReyaxLoRa_Send()
// Subroutine for sending data.
void ReyaxLoRa_Send(int addr, String data_send) {
  String str_Send;
  
  // AT commands to transmit data. 
  // For more details see the document "RYLR993_AT_Command.pdf" in the "AT+SEND" section.
  str_Send = "AT+SEND=" + String(addr) + "," + String(data_send.length()) + "," + data_send + "\r";
  ReyaxLoRa.print(str_Send);
  
  Serial.println();
  Serial.print("Send to Master : ");
  Serial.println(str_Send);
  Serial.flush();
}
//________________________________________________________________________________ 

//________________________________________________________________________________ReyaxLoRa_Receive()
// Subroutine to receive data.
void ReyaxLoRa_Receive() {
  if (ReyaxLoRa.available() > 0 ) {
    // Reads data received from the LoRa Reyax RYLR993_Lite module.
    String rcv_Data_String = ReyaxLoRa.readString();
    
    Serial.println();
    Serial.print("LoRa Reyax RYLR993_Lite Result code : ");
    Serial.println(rcv_Data_String);

    // If the received data contains the character "RCV" it will be identified as receiving data from the sender (master).
    if(rcv_Data_String.indexOf("RCV") > 0 ) {
      
      //String _message = getValue(rcv_Data_String, ',', 2);    //--> data/message (ASCll Format Data).

      // Calling the get_DHT11_Sensor_Data() subroutine.
      get_DHT11_Sensor_Data();

      // Reading the state of the tilt sensor.
      tiltState = digitalRead(TILT_SENSOR_PIN);

      // Send data to master.
      ReyaxLoRa_Send(master_Address, String(t) + "|" + String(h) + "|" + String(tiltState));
    }
  }
}
//________________________________________________________________________________ 

//________________________________________________________________________________getValue()
// String function to split data based on certain characters.
String getValue(String data, char separator, int index) {
  int found = 0;
  int strIndex[] = { 0, -1 };
  int maxIndex = data.length() - 1;
  
  for (int i = 0; i <= maxIndex && found <= index; i++) {
    if (data.charAt(i) == separator || i == maxIndex) {
      found++;
      strIndex[0] = strIndex[1] + 1;
      strIndex[1] = (i == maxIndex) ? i+1 : i;
    }
  }
  return found > index ? data.substring(strIndex[0], strIndex[1]) : "";
}
//________________________________________________________________________________

//________________________________________________________________________________get_DHT11_Sensor_Data()
void get_DHT11_Sensor_Data() {
  // Reading temperature or humidity takes about 250 milliseconds!
  // Sensor readings may also be up to 2 seconds 'old' (its a very slow sensor)
  h = dht11.readHumidity();
  // Read temperature as Celsius (the default)
  t = dht11.readTemperature();

  // Check if any reads failed and exit early (to try again).
  if (isnan(h) || isnan(t)) {
    Serial.println();
    Serial.println(F("Failed to read from DHT sensor!"));
    h = 0;
    t = 0.00;
  }
}
//________________________________________________________________________________

//________________________________________________________________________________VOID SETUP()
void setup() {
  // put your setup code here, to run once:
  
  // set the data rate for the HardwareSerial port.
  Serial.begin(115200);
  Serial.println();
  
  delay(2000);
  
  // set the data rate for the SoftwareSerial port (Communication with RYLR993_Lite).
  ReyaxLoRa.begin(9600); //--> 9600 is the LoRa Reyax RYLR993_Lite default baud rate.

  delay(1000);

  // Init dht11.
  dht11.begin();

  delay(500);

  // Init the Tilt Sensor pin.
  pinMode(TILT_SENSOR_PIN, INPUT);

  Serial.println("AT");
  ReyaxLoRa.print("AT\r");
}
//________________________________________________________________________________

//________________________________________________________________________________VOID LOOP()
void loop() { 
  // put your main code here, to run repeatedly:

  // Call the "ReyaxLoRa_Receive()" subroutine.
  ReyaxLoRa_Receive();
}
//________________________________________________________________________________
//<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
