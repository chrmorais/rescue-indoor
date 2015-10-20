#include <WiFlyHQ.h>
#include <SoftwareSerial.h>
SoftwareSerial wifiSerial(4, 5);
SoftwareSerial rfidReader(2, 3); //Pin 2 - RX /Pin 3 - TX

WiFly wifly;
char newTag[13];
const char ip[]="10.103.17.2";
void setup() {
  Serial.begin(115200);
  rfidReader.begin(9600); //setup the Rfid reader speed
  conect();
}

void loop () {
  // Counter for the newTag array
  int i = 0;
  // Variable to hold each byte read from the serial buffer
  int readByte;
  // Flag so we know when a tag is over
  boolean tag = false;
  rfidReader.listen();
  // This makes sure the whole tag is in the serial buffer before
  // reading, the Arduino can read faster than the ID module can deliver!
  if (rfidReader.available() == 16) {
    tag = true;
  }

  if (tag == true) {
    while (rfidReader.available() > 0) {

      readByte = rfidReader.read(); // read from the serial port
      /* This will skip the first byte (2, STX, start of text) and the last three,
      ASCII 13, CR/carriage return, ASCII 10, LF/linefeed, and ASCII 3, ETX/end of
      text, leaving only the unique part of the tag string. It puts the byte into
      the first space in the array, then steps ahead one spot */
      if (readByte != 2 && readByte != 13 && readByte != 10 && readByte != 3) {
        newTag[i] = readByte;
        i++;
      }

      // If we see ASCII 3, ETX, the tag is over
      if (readByte == 3) {
        tag = false;
      }

    }

  }

  // don't do anything if the newTag array is full of zeroes
  if (strlen(newTag) == 0) {
    return;
  }
  else {
    wifiSerial.listen();
    if (wifly.isConnected() == false) {
      Serial.println("Connecting");
      if (wifly.open(ip, 1234)) {
        Serial.println("Connected");
      } else {
        Serial.println("Failed to open");
      }
    }
    sendData(newTag);
    
  }


  // Once newTag has been sended, fill it with zeroes
  // to get ready for the next tag read
  for (int c = 0; c < 13; c++) {
    newTag[c] = 0;

  }
}

  

int sendData(char nTag[]) {

  //send tag here
  //Serial.println(nTag);
  wifly.println(nTag);
  wifly.flushRx(200);
  char ch;
  delay(100);
  wifiSerial.listen();
  while (wifly.available()) {
    ch = wifly.read();
    Serial.print(ch);
  } 
 
  wifly.close();

  return 1;
}

int checkTag(char nTag[], char oTag[]) {

  for (int i = 0; i < 13; i++) {
    if (nTag[i] != oTag[i]) {
      return 0;
    }
  }
  return 1;
}

void conect() {
  const char mySSID[] = "ECDU-PROFESSORES";
  const char myPassword[] = "gsortifba2015";

  char buf[64];


  Serial.println("Starting");
  Serial.print("Free memory: ");
  Serial.println(wifly.getFreeMemory(), DEC);

  wifiSerial.begin(9600);
  if (!wifly.begin(&wifiSerial, &Serial)) {
    Serial.println("Failed to start wifly");
    terminal();
  }

  /* Join wifi network if not already associated */
  if (!wifly.isAssociated()) {
    /* Setup the WiFly to connect to a wifi network */
    //Serial.println("Joining network");
    wifly.setSSID(mySSID);
    wifly.setPassphrase(myPassword);
    wifly.enableDHCP();

    if (wifly.join()) {
      Serial.println("Joined wifi network");
    } else {
      Serial.println("Failed to join wifi network");
      terminal();
    }
  } else {
    Serial.println("Already joined network");
  }

  Serial.println("WiFly ready");

  //Serial.print("MAC: ");
  //Serial.println(wifly.getMAC(buf, sizeof(buf)));
  Serial.print("IP: ");
  Serial.println(wifly.getIP(buf, sizeof(buf)));
  //Serial.print("Netmask: ");
  //Serial.println(wifly.getNetmask(buf, sizeof(buf)));
  //Serial.print("Gateway: ");
 // Serial.println(wifly.getGateway(buf, sizeof(buf)));

  //Serial.println("Set DeviceID");
  wifly.setDeviceID("Wifly-TCP");
  ///Serial.print("DeviceID: ");
  //Serial.println(wifly.getDeviceID(buf, sizeof(buf)));

  wifly.setIpProtocol(WIFLY_PROTOCOL_TCP);

  if (wifly.isConnected()) {
    Serial.println("Old connection active. Closing");
    wifly.close();
  }

}

void terminal()
{
  while (1) {
    if (wifly.available() > 0) {
      Serial.write(wifly.read());
    }


    if (Serial.available()) { // Outgoing data
      wifly.write(Serial.read());
    }
  }
}


