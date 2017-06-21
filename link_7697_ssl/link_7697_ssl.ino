// LinkIt 7697 sketch for MQTT Demo

#include <LWiFi.h>


#include "PubSubClient.h"

/* 
	Modify to your WIFI Access Credentials. 
*/
char ssid[] = "need151";      //  your network SSID (name)  //zino
char pass[] = "3E58567890";  // your network password (use for WPA, or use as key for WEP)  //zino
int keyIndex = 0;               // your network key Index number (needed only for WEP)  //zino

int status = WL_IDLE_STATUS;  //zino

/*
	Modify to your MQTT broker - Select only one
*/
char mqttBroker[] = "iot.eclipse.org";
// byte mqttBroker[] = {192,168,1,220}; // modify to your local broker

// This is the root certificate for our host.
// Different host server may have different root CA.
static const char rootCA[] = "-----BEGIN CERTIFICATE-----\r\n"
"MIIDSjCCAjKgAwIBAgIQRK+wgNajJ7qJMDmGLvhAazANBgkqhkiG9w0BAQUFADA/\r\n"
"MSQwIgYDVQQKExtEaWdpdGFsIFNpZ25hdHVyZSBUcnVzdCBDby4xFzAVBgNVBAMT\r\n"
"DkRTVCBSb290IENBIFgzMB4XDTAwMDkzMDIxMTIxOVoXDTIxMDkzMDE0MDExNVow\r\n"
"PzEkMCIGA1UEChMbRGlnaXRhbCBTaWduYXR1cmUgVHJ1c3QgQ28uMRcwFQYDVQQD\r\n"
"Ew5EU1QgUm9vdCBDQSBYMzCCASIwDQYJKoZIhvcNAQEBBQADggEPADCCAQoCggEB\r\n"
"AN+v6ZdQCINXtMxiZfaQguzH0yxrMMpb7NnDfcdAwRgUi+DoM3ZJKuM/IUmTrE4O\r\n"
"rz5Iy2Xu/NMhD2XSKtkyj4zl93ewEnu1lcCJo6m67XMuegwGMoOifooUMM0RoOEq\r\n"
"OLl5CjH9UL2AZd+3UWODyOKIYepLYYHsUmu5ouJLGiifSKOeDNoJjj4XLh7dIN9b\r\n"
"xiqKqy69cK3FCxolkHRyxXtqqzTWMIn/5WgTe1QLyNau7Fqckh49ZLOMxt+/yUFw\r\n"
"7BZy1SbsOFU5Q9D8/RhcQPGX69Wam40dutolucbY38EVAjqr2m7xPi71XAicPNaD\r\n"
"aeQQmxkqtilX4+U9m5/wAl0CAwEAAaNCMEAwDwYDVR0TAQH/BAUwAwEB/zAOBgNV\r\n"
"HQ8BAf8EBAMCAQYwHQYDVR0OBBYEFMSnsaR7LHH62+FLkHX/xBVghYkQMA0GCSqG\r\n"
"SIb3DQEBBQUAA4IBAQCjGiybFwBcqR7uKGY3Or+Dxz9LwwmglSBd49lZRNI+DT69\r\n"
"ikugdB/OEIKcdBodfpga3csTS7MgROSR6cz8faXbauX+5v3gTt23ADq1cEmv8uXr\r\n"
"AvHRAosZy5Q6XkjEGB5YGV8eAlrwDPGxrancWYaLbumR9YbK+rlmM6pZW87ipxZz\r\n"
"R8srzJmwN0jP41ZL9c8PDHIyh8bwRLtTcm1D9SZImlJnt1ir/md2cXjbDaJWFBM5\r\n"
"JDGFoqgCWjBH4d1QB7wCCZAA62RjYJsWvIjJEubSfZGL+T0yjWW06XyxV3bqxbYo\r\n"
"Ob8VZRzI9neWagqNdwvYkQsEjgfbKbYK7p2CNTUQ\r\n"
"-----END CERTIFICATE-----\r\n";


//WiFiClient wifiClient;
TLSClient wificlient;
//PubSubClient client( wifiClient );
PubSubClient client;

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

   wificlient.setRootCA(rootCA, sizeof(rootCA));
   
   client.setClient (wificlient);
   client.setServer( mqttBroker, 8883);
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

