// LinkIt 7697 sketch for MQTT Demo

#include <LWiFi.h>


#include "PubSubClient.h"

/* 
	Modify to your WIFI Access Credentials. 
*/
char ssid[] = "ssid";      //  your network SSID (name)  //zino
char pass[] = "pwd";  // your network password (use for WPA, or use as key for WEP)  //zino
int keyIndex = 0;               // your network key Index number (needed only for WEP)  //zino

int status = WL_IDLE_STATUS;  //zino

/*
	Modify to your MQTT broker - Select only one
*/
char mqttBroker[] = "iot.eclipse.org";
// byte mqttBroker[] = {192,168,1,220}; // modify to your local broker

WiFiClient wifiClient;

PubSubClient client( wifiClient );

unsigned long lastSend;

void InitLWiFi()
{
    while (status != WL_CONNECTED) {
        Serial.print("Attempting to connect to SSID: ");
        Serial.println(ssid);
        // Connect to WPA/WPA2 network. Change this line if using open or WEP network:
        status = WiFi.begin(ssid, pass);
    }
    Serial.println("Connected to wifi");
    printWifiStatus();
}

void printWifiStatus() {
    // print the SSID of the network you're attached to:
    Serial.print("SSID: ");
    Serial.println(WiFi.SSID());

    // print your WiFi shield's IP address:
    IPAddress ip = WiFi.localIP();
    Serial.print("IP Address: ");
    Serial.println(ip);

    // print the received signal strength:
    long rssi = WiFi.RSSI();
    Serial.print("signal strength (RSSI):");
    Serial.print(rssi);
    Serial.println(" dBm");
}

void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Connecting to MQTT broker ...");
    // Attempt to connect
    if ( client.connect("LinkIt One Client") ) {	// Better use some random name
      Serial.println( "[DONE]" );
      // Publish a message on topic "outTopic"
      client.publish( "outTopic","Hello, This is LinkIt One" );
	  // Subscribe to topic "inTopic"
      client.subscribe( "inTopic" );
    } else {
      Serial.print( "[FAILED] [ rc = " );
      Serial.print( client.state() );
      Serial.println( " : retrying in 5 seconds]" );
      // Wait 5 seconds before retrying
      delay( 5000 );
    }
  }
}


void setup()
{
   delay( 10000 );
   Serial.begin( 9600 );//9600
   InitLWiFi();

   client.setServer( mqttBroker, 1883 );
   client.setCallback( callback );

   lastSend = 0;
}

void loop()
{
  
  if( !client.connected() ) {
    reconnect();
  }

  if( millis()-lastSend > 5000 ) {	// Send an update only after 5 seconds
    sendAnalogData();
    lastSend = millis();
  }
  
  client.loop();
}

void callback( char* topic, byte* payload, unsigned int length ) {
  Serial.print( "Recived message on Topic:" );
  Serial.print( topic );
  Serial.print( "    Message:");
  for (int i=0;i<length;i++) {
    Serial.print( (char)payload[i] );
  }
  Serial.println();
}

void sendAnalogData() {
  // Read data to send
  int data_A0 = analogRead( A0 );
  int data_A1 = analogRead( A1 );
  int data_A2 = analogRead( A2 );

  // Just debug messages
  Serial.print( "Sending analog data : [" );
  Serial.print( data_A0 ); Serial.print( data_A1 ); Serial.print( data_A2 );
  Serial.print( "]   -> " );

  // Prepare a JSON payload string
  String payload = "{";
  payload += "\"A0\":\""; payload += int(data_A0); payload += "\", ";
  payload += "\"A1\":\""; payload += int(data_A1); payload += "\", ";
  payload += "\"A2\":\""; payload += int(data_A2); payload += "\"";
  payload += "}";
  
  // Send payload
  char analogData[100];
  payload.toCharArray( analogData, 100 );
  client.publish( "analogData", analogData );
  Serial.println( analogData );
}

