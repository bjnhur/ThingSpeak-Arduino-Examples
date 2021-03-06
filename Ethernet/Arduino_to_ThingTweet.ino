/*

 Arduino --> ThingTweet via Ethernet
 
 The ThingTweet sketch is designed for the Arduino + Ethernet Shield.
 This sketch updates a Twitter status via the ThingTweet App
 (https://thingspeak.com/docs/thingtweet) using HTTP POST.
 ThingTweet is a Twitter proxy web application that handles the OAuth.
 
 Getting Started with ThingSpeak and ThingTweet:
 
    * Sign Up for a New User Account for ThingSpeak - https://www.thingspeak.com/users/new
    * Link your Twitter account to the ThingTweet App - Apps / ThingTweet
    * Enter the ThingTweet API Key in this sketch under "ThingSpeak Settings"
  
 Arduino Requirements:
 
   * Arduino with Ethernet Shield or Arduino Ethernet
   * Arduino 1.0 IDE
   * Twitter account linked to your ThingSpeak account
   
  Network Requirements:

   * Ethernet port on Router    
   * DHCP enabled on Router
   * Unique MAC Address for Arduino
 
 Created: October 17, 2011 by Hans Scharler (http://www.nothans.com)
 Updated: December 7, 2012 by Hans Scharler (http://www.nothans.com)
 
 Additional Credits:
 Example sketches from Arduino team, Ethernet by Adrian McEwen
 
 Updated: June 29, 2015 by Bongjun Hur
 changed some code for new WIZnet W5500 Ethernet Shield.
*/


#include <SPI.h>
#include <Ethernet.h>

// Local Network Settings
byte mac[] = { 0xD4, 0x28, 0xB2, 0xFF, 0xA0, 0xA1 }; // Must be unique on local network

// for static IP users.
IPAddress ip(111, 111, 111, 111);
IPAddress gateway(222, 222, 222, 1);
IPAddress subnet(255, 255, 255, 0);
// fill in your Domain Name Server address here:
IPAddress myDns(8, 8, 8, 8); // google puble dns

// ThingSpeak Settings
char thingSpeakAddress[] = "api.thingspeak.com";
String thingtweetAPIKey = "XXXXXXX6LNWOU08H";

// Variable Setup
long lastConnectionTime = 0; 
boolean lastConnected = false;
int failedCounter = 0;

// Initialize Arduino Ethernet Client
EthernetClient client;

// error at 1.6.4 IDE, so add declaration of function. 
void startEthernet();
void updateTwitterStatus(String tsData);

void setup()
{
  // Open serial communications and wait for port to open:
  Serial.begin(9600);
  while (!Serial) {
    ; // wait for serial port to connect. Needed for Leonardo only
  }
  
  // Start Ethernet on Arduino
  startEthernet();
  
  delay(1000);
  
  // Update Twitter via ThingTweet
  updateTwitterStatus("My thing is social #3 @thingspeak");
}

void loop()
{  
  // Print Update Response to Serial Monitor
  if (client.available())
  {
    char c = client.read();
    Serial.print(c);
  }

  // Disconnect from ThingSpeak
  if (!client.connected() && lastConnected)
  {
    Serial.println("...disconnected");
    Serial.println();
    
    client.stop();
  }
    
  // Check if Arduino Ethernet needs to be restarted
  if (failedCounter > 3 ) {startEthernet();}
  
  lastConnected = client.connected();
}

void updateTwitterStatus(String tsData)
{
  if (client.connect(thingSpeakAddress, 80))
  { 
    // Create HTTP POST Data
    tsData = "api_key="+thingtweetAPIKey+"&status="+tsData;
            
    client.print("POST /apps/thingtweet/1/statuses/update HTTP/1.1\n");
    client.print("Host: api.thingspeak.com\n");
    client.print("Connection: close\n");
    client.print("Content-Type: application/x-www-form-urlencoded\n");
    client.print("Content-Length: ");
    client.print(tsData.length());
    client.print("\n\n");

    client.print(tsData);
    
    lastConnectionTime = millis();
    
    if (client.connected())
    {
      Serial.println("Connecting to ThingSpeak...");
      Serial.println();
      
      failedCounter = 0;
    }
    else
    {
      failedCounter++;
  
      Serial.println("Connection to ThingSpeak failed ("+String(failedCounter, DEC)+")");   
      Serial.println();
    }
    
  }
  else
  {
    failedCounter++;
    
    Serial.println("Connection to ThingSpeak Failed ("+String(failedCounter, DEC)+")");   
    Serial.println();
    
    lastConnectionTime = millis(); 
  }
}

void startEthernet()
{
  
  client.stop();

  Serial.println("Connecting Arduino to network...");
  Serial.println();  

  delay(1000);
  
  // Connect to network amd obtain an IP address using DHCP
  // initialize the ethernet device
#if defined __USE_DHCP__
#if defined(WIZ550io_WITH_MACADDRESS) // Use assigned MAC address of WIZ550io
  if (Ethernet.begin() == 0)
#else
  if (Ethernet.begin(mac) == 0)
#endif  
  {
    Serial.println("DHCP Failed, reset Arduino to try again");
    Serial.println();
  }
  else
  {
    Serial.println("Arduino connected to network using DHCP");
    Serial.println();
  }
#else
#if defined(WIZ550io_WITH_MACADDRESS) // Use assigned MAC address of WIZ550io
  Ethernet.begin(ip, myDns, gateway, subnet);
#else
  Ethernet.begin(mac, ip, myDns, gateway, subnet);
#endif  
    Serial.println("Arduino connected to network using Static IP : ");
    Serial.println(Ethernet.localIP());
#endif   

  delay(1000);
}
