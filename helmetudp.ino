//helmet detection code


#include <SPI.h>
#include <WiFiNINA.h>
#include <WiFiUdp.h>

int status = WL_IDLE_STATUS;
// #include "arduino_secrets.h" 
char ssid[] = "RTCAR";        // your network SSID (name)
char pass[] = "409504504";    // your network password (use for WPA, or use as key for WEP)
int keyIndex = 0;            // your network key index number (needed only for WEP)

unsigned int localPort = 2391;      // local port to listen on
IPAddress ip(192, 168, 100, 7); // Helmet UDP Server Default IP 

char packetBuffer[256]; //buffer to hold incoming packet
char  ReplyBuffer[] = "acknowledged";       // a string to send back

WiFiUDP Udp;


int dis;
int dur;
int recentavr;
int rondo=5;
int room[10];
int MAX;
int recmsg;
const int trigPin = 9;
const int echoPin = 10;



void setup() {
  
  Serial.begin(9600);

  if (WiFi.status() == WL_NO_MODULE) {
    Serial.println("Communication with WiFi module failed!");
    // don't continue
    while (true);
  }


  String fv = WiFi.firmwareVersion();
  if (fv < WIFI_FIRMWARE_LATEST_VERSION) {
    Serial.println("Please upgrade the firmware");
  }

  // attempt to connect to WiFi network:
  while (status != WL_CONNECTED) {
    Serial.print("Attempting to connect to SSID: ");
    Serial.println(ssid);
    // Connect to WPA/WPA2 network. Change this line if using open or WEP network:
    status = WiFi.begin(ssid, pass);

    // wait 10 seconds for connection:
    delay(5000);
  }
  Serial.println("Connected to WiFi");
  printWifiStatus();

  Serial.println("\nStarting connection to server...");
  // if you get a connection, report back via serial:
  Udp.begin(localPort);

  pinMode(trigPin, OUTPUT); // Sets the trigPin as an Output
  pinMode(echoPin, INPUT); // Sets the echoPin as an Input

}

void loop() {

  int packetSize = Udp.parsePacket();
  if (packetSize) {
    Serial.print("Received packet of size ");
    Serial.println(packetSize);
    Serial.print("From ");
    IPAddress remoteIp = Udp.remoteIP();
    Serial.print(remoteIp);
    Serial.print(", port ");
    Serial.println(Udp.remotePort());

    // read the packet into packetBufffer
    int len = Udp.read(packetBuffer, 32);
    if (len > 0) {
      packetBuffer[len] = 0;
    }
      rondo++;
      if(rondo==100) rondo=0;
      digitalWrite(trigPin, LOW);
      delayMicroseconds(2);
      digitalWrite(trigPin, HIGH);
      delayMicroseconds(10);
      digitalWrite(trigPin, LOW);
      dur = pulseIn(echoPin, HIGH);
      dis = dur * 0.034 / 2;
      
      room[rondo%5]=dis;
      recentavr=(room[0]+room[1]+room[2]+room[3]+room[4]+room[5]+room[6]+room[7]+room[8]+room[9])/3;
      
      for(int i=0;i<3;i++) {                  
        if(room[i]>room[i-1]) MAX=room[i];
      }
      if(recentavr>=20){
          if(recmsg!=1) Serial.print("Helmet not detected !\n");
          recmsg=1;
      }
      else if(recentavr>=7){
          if(recmsg!=2) Serial.print("Please wear your helmet properly !\n");
          recmsg=2;
      }
      else{
              if(recmsg!=3) Serial.print("Helmet detected\n");
          recmsg=3;
      }
  
      // send a reply, to the IP address and port that sent us the packet we received
      Udp.beginPacket(Udp.remoteIP(), Udp.remotePort());
      char temp[64];
      memset(temp, 0x00, 64);

      sprintf(temp, "{\"helmet_stats\": %d}", recmsg);    
      Udp.write(temp); // ReplyBuffer);
      Udp.endPacket();

      delay(5);
      Udp.stop();

      delay(5);
      Serial.println("\nStarting connection to server...");
      // if you get a connection, report back via serial:
      Udp.begin(localPort);
      
      Serial.print("Listening on port ");
      Serial.println(localPort);      

  }
  
  
  delay(100);
}



void printWifiStatus() {
  // print the SSID of the network you're attached to:
  Serial.print("SSID: ");
  Serial.println(WiFi.SSID());

  // print your board's IP address:
  IPAddress ip = WiFi.localIP();
  Serial.print("IP Address: ");
  Serial.println(ip);

  // print the received signal strength:
  long rssi = WiFi.RSSI();
  Serial.print("signal strength (RSSI):");
  Serial.print(rssi);
  Serial.println(" dBm");
}



