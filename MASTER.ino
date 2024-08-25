//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>> 05_Final_Master
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// A brief explanation of how this project works.                                                                                           //
// - The Master sends data to the Slave.                                                                                                    //
// - If the Slave receives data from the Master, the Slave will send data back to the Master,                                                //
//   the data contains the temperature value, humidity value, tilt sensor state, state of LED_1, and brightness of LED_2.                   //
// - The point is that the Master sends data to the Slave and the Slave only sends data to the Master if it receives data from the Master.  //
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//----------------------------------------Include Library.
#include <SoftwareSerial.h>
//----------------------------------------

// Defines the slave/destination address.
#define slave_Address 2

// "Timer" variable for data sending interval.
unsigned long previousMillis = 0;
const long interval = 3000; // Sending data to the slave every 3000 milliseconds (3 seconds).

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
  Serial.print("Send to Slave : ");
  Serial.println(str_Send);
  Serial.flush();
}
//________________________________________________________________________________ 

//________________________________________________________________________________ReyaxLoRa_Receive()
// Subroutine to receive data and display the received data on the Serial Monitor.
void ReyaxLoRa_Receive() {
  if (ReyaxLoRa.available() > 0 ) {
    // Reads data received from the LoRa Reyax RYLR993_Lite module.
    String rcv_Data_String = ReyaxLoRa.readString();
    
    Serial.println();
    Serial.print("LoRa Reyax RYLR993_Lite Result code : ");
    Serial.println(rcv_Data_String);
    Serial.flush();

    // If the received data contains the character "RCV" it will be identified as receiving data from the sender (slave).
    if(rcv_Data_String.indexOf("RCV") > 0 ) {
      // Processing data received from the LoRa Reyax RYLR993_Lite module.
      
      String _message = getValue(rcv_Data_String, ',', 2); //--> data/message (ASCII Format Data).

      // Data received from the sender (slave) will be separated based on the "|" separator.
      // The data format received from the sender (slave) is : 
      // > temperature_value|humidity_value|tilt_state|LED_1_state|LED_2_brightness
      // > example: 24.95|90|1|1|65
      
      float Temp_Val = getValue(_message, '|', 0).toFloat();
      int Humd_Val = getValue(_message, '|', 1).toInt();
      int Tilt_State = getValue(_message, '|', 2).toInt();
      int LED_1_State = getValue(_message, '|', 3).toInt();
      int LED_2_Brightness = getValue(_message, '|', 4).toInt();

      // Display received data on the Serial Monitor.
      Serial.print("Temperature: ");
      Serial.print(Temp_Val);
      Serial.println(" C");

      Serial.print("Humidity: ");
      Serial.print(Humd_Val);
      Serial.println(" %");

      Serial.print("Tilt State: ");
      Serial.println(Tilt_State == 1 ? "Tilted" : "Not Tilted");

      Serial.print("LED_1 State: ");
      Serial.println(LED_1_State == 1 ? "ON" : "OFF");

      Serial.print("LED_2 Brightness: ");
      Serial.print(LED_2_Brightness);
      Serial.println(" %");
    }
  }
}
//________________________________________________________________________________ 

//________________________________________________________________________________getValue()
// String function to split data based on certain characters.
// Reference : https://www.electroniclinic.com/reyax-lora-based-multiple-sensors-monitoring-using-arduino/
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

//________________________________________________________________________________VOID SETUP()
void setup() {
  // Set the data rate for the HardwareSerial port.
  Serial.begin(115200);
  Serial.println();
  
  delay(2000);
  
  // Set the data rate for the SoftwareSerial port (Communication with RYLR993_Lite).
  ReyaxLoRa.begin(9600); //--> 9600 is the LoRa Reyax RYLR993_Lite default baud rate.

  delay(1000);

  Serial.println("AT");
  ReyaxLoRa.print("AT\r");
}
//________________________________________________________________________________

//________________________________________________________________________________VOID LOOP()
void loop() { 
  // Millis/Timer to send data. Data is sent every 3 seconds (see "const long interval" variable).
  unsigned long currentMillis = millis();

  if (currentMillis - previousMillis >= interval) {
    previousMillis = currentMillis;

    // Send data to the slave.
    ReyaxLoRa_Send(slave_Address, "Request");
  }

  // Call the "ReyaxLoRa_Receive()" subroutine.
  ReyaxLoRa_Receive();
}
//________________________________________________________________________________
//<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
