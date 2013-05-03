#include <dht22.h>
#include <Versalino.h>

dht22 DHT22;

#include <SPI.h>
#include <Ethernet.h>
EthernetClient client;
char server[] = "servername";

unsigned long lastConnectionTime = 0;          // last time you connected to the server, in milliseconds
boolean lastConnected = false;                 // state of the connection last time through the main loop
const unsigned long postingInterval = 600000;  // delay between updates, in milliseconds

// Enter a MAC address and IP address for your controller below.
// The IP address will be dependent on your local network.
// gateway and subnet are optional:
byte mac[] = { 
  0x00, 0xAA, 0xBB, 0xCC, 0xDE, 0x02 };
IPAddress ip(192,168,1, 177);
IPAddress gateway(192,168,1, 1);
IPAddress subnet(255, 255, 0, 0);

void setup() {

  
  // Set DHT sensor to pin 5.
  DHT22.attach(5);
  // Open serial communications and wait for port to open:
  Serial.begin(9600);
 
  // start the Ethernet connection:
  Serial.println("Trying to get an IP address using DHCP");
  
  if (Ethernet.begin(mac) == 0) {
    Serial.println("Failed to configure Ethernet using DHCP");
    // initialize the ethernet device not using DHCP:
    Ethernet.begin(mac, ip, gateway, subnet);
  }
  // print your local IP address:
  Serial.print("My IP address: ");
  ip = Ethernet.localIP();
  for (byte thisByte = 0; thisByte < 4; thisByte++) {
    // print the value of each byte of the IP address:
    Serial.print(ip[thisByte], DEC);
    Serial.print("."); 
  }
  Serial.println();
}

void loop() {
  
  if (client.available()) {
    char c = client.read();
    //Serial.print(c);
  }
  else {

  }
  
  if (!client.connected() && lastConnected) {
    //Serial.println();
    //Serial.println("disconnecting.");
    client.stop();
  }
  
  if(!client.connected() && (millis() - lastConnectionTime > postingInterval)) {
    int chk = DHT22.read();
    if (chk == 0) {
      httpRequest( DHT22.fahrenheit(), DHT22.humidity);
    }
  }

  lastConnected = client.connected();
}

void httpRequest(float temp, float humid) {
  int iT = (int)temp;
  int iH = (int)humid;

  
  String PostData = "data[EnvironmentalReading][tempurature]=" + String(iT) + "&data[EnvironmentalReading][humidity]=" + String(iH);
  Serial.println(PostData);
  // if there's a successful connection:
  if (client.connect(server, 80)) {
    //Serial.println("connecting...");

      client.println("POST /environmental_readings/add HTTP/1.1");
      client.println("Content-Type: application/x-www-form-urlencoded");
      client.println("Connection: close");
      client.print("Content-Length: ");
      client.println(PostData.length());
      client.println("User-Agent: Arduino/1.0");
      client.println("Host: localhost");
      client.println();
      client.println(PostData);

    // note the time that the connection was made:
    lastConnectionTime = millis();
  } 
  else {
    // if you couldn't make a connection:
    Serial.println("connection failed");
    //Serial.println("disconnecting.");
    client.stop();
  }
  //Serial.println("HTTPRequest Completed");
}


