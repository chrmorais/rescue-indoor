
#include <SoftwareSerial.h>
#include <WiFlyHQ.h>

SoftwareSerial wifiSerial(4, 5);
SoftwareSerial rfidReader(2, 3); //Pin 2 - RX /Pin 3 - TX
char c = 0; // value read for serial port
int readByte = 0;
const int tagLen = 16;
const int idLen = 13;
const int kTags = 2;
char newTag[idLen];

void terminal();

WiFly wifly;



void setup()
{ Serial.begin(115200);
  
  rfidReader.begin(9600);
    
  //rfidReader.begin(9600);
}

uint32_t connectTime = 0;
void setNewTag() {
   
  // Counter for the newTag array
  int i = 0;
  // Variable to hold each byte read from the serial buffer
  int readByte;
  // Flag so we know when a tag is over
  boolean tag = false;

  // This makes sure the whole tag is in the serial buffer before
  // reading, the Arduino can read faster than the ID module can deliver!
  if (rfidReader.available() == tagLen) {
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
    Serial.print("New TAG: ");
    Serial.println(newTag);
    rfidReader.end();
    //delay(200);
    conect();
    delay(200);
    checkId(newTag);
    rfidReader.begin(9600);
  }
}
void loop()
{
  setNewTag();
}


void checkId(char nTag[]) {
  //char Tag[15];
  // Tag[13]='\n';
   //Tag[14]='\r';
  //for(int i=0; i<idLen; i++){
  //  Tag[i]=nTag[i];
  //  }
  Serial.println("Connecting");
    if (wifly.open("10.103.17.28", 20606)) {
      Serial.println("Connected");
      delay(200);
      //wifly.write(strcat(nTag,"\n\r"));
      wifly.write(nTag);
      wifly.write("\n\r");
      //wifly.flush();
      delay(200);
      Serial.println("Response: ");
      //Serial.write(wifly.read()); // imprime resposta
      while(wifly.available() > 0) {
      Serial.write(wifly.read());
      }
      Serial.println("");
      Serial.println("Disconnecting");
      wifly.close();
    } else {
      Serial.println("Failed to open");
    }
}

void conect() {
  const char mySSID[] = "ECDU-ALUNOS";
  const char myPassword[] = "alunoifba";
  char buf[80];

  Serial.println(F("Starting"));
  Serial.print(F("Free memory: "));
  Serial.println(wifly.getFreeMemory(), DEC);

  wifiSerial.begin(9600);
  if (!wifly.begin(&wifiSerial, &Serial)) {
    Serial.println(F("Failed to start wifly"));
    wifly.terminal();
  }
  if (!wifly.isAssociated()) {
    Serial.println(F("Joining network"));
    wifly.setSSID(mySSID);
    wifly.setPassphrase(myPassword);
    wifly.enableDHCP();
    wifly.save();
    if (wifly.join()) {
      Serial.println(F("Joined wifi network"));
    }
    else {
      Serial.println(F("Failed to join wifi network"));
      wifly.terminal();
    }
  }
  else {
    Serial.println(F("Already joined network"));
  }

  wifly.setBroadcastInterval(0);
  Serial.print(F("MAC: "));
  Serial.println(wifly.getMAC(buf, sizeof(buf)));
  Serial.print(F("IP: "));
  Serial.println(wifly.getIP(buf, sizeof(buf)));

  wifly.setDeviceID("Wifly-WebServer");

  if (wifly.isConnected()) {
    Serial.println(F("Old connection active. Closing"));
    wifly.close();
  }

  wifly.setProtocol(WIFLY_PROTOCOL_TCP);
  if (wifly.getPort() != 80) {
    wifly.setPort(80);
    /* local port does not take effect until the WiFly has rebooted (2.32) */
    wifly.save();
    Serial.println(F("Set port to 80, rebooting to make it work"));
    wifly.reboot();
    delay(3000);
  }
  Serial.println(F("Ready"));

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
