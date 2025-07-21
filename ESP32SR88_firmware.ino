
/******************************************************************************
 * V1.7
 * Added escape char \ so that double quote (") can be included in passwords
 * 
 * v1.8
 * Fix issue with null comparison.
 ******************************************************************************/

#include <Preferences.h>
#include <WiFi.h>

const char ver[] = {"1.8"};
const char moduleID = 43;

Preferences nvm;
WiFiServer server(80);
WiFiServer tcpServer(0);

IPAddress local_IP(192, 168, 0, 121);
IPAddress gateway(192, 168, 0, 1);
IPAddress subnet(255, 255, 0, 0);
IPAddress primaryDNS(8, 8, 8, 8); //optional
IPAddress secondaryDNS(8, 8, 4, 4); //optional

#define RELAY_1  13
#define RELAY_2  12
#define RELAY_3  4
#define RELAY_4  27
#define RELAY_5  26
#define RELAY_6  25
#define RELAY_7  33
#define RELAY_8  32

#define LED_POWER 15

#define OPTO_1    14
#define OPTO_2    16
#define OPTO_3    17
#define OPTO_4    18
#define OPTO_5    19
#define OPTO_6    21
#define OPTO_7    22
#define OPTO_8    23

#define WIFI_LED 5

#define BUFSIZE 51

char buffer[BUFSIZE];
char ssid[BUFSIZE];
char WifiPassword[BUFSIZE];
char password2[BUFSIZE];
uint AsciiPort;
char MqttServer[BUFSIZE];
char MqttID[BUFSIZE];
uint MqttPort;
char MqttUser[BUFSIZE];
char MqttPasswd[BUFSIZE];
char mqttpasswd2[BUFSIZE];
char R1Topic[BUFSIZE];    // Relay topics
char R2Topic[BUFSIZE];
char R3Topic[BUFSIZE];
char R4Topic[BUFSIZE];
char R5Topic[BUFSIZE];
char R6Topic[BUFSIZE];
char R7Topic[BUFSIZE];
char R8Topic[BUFSIZE];
char N1Topic[BUFSIZE];    // Input topics
char N2Topic[BUFSIZE];
char N3Topic[BUFSIZE];
char N4Topic[BUFSIZE];
char N5Topic[BUFSIZE];
char N6Topic[BUFSIZE];
char N7Topic[BUFSIZE];
char N8Topic[BUFSIZE];
char AsciiPassword[BUFSIZE];

char mqtt_opto_states[ 8 ] = { 0, 0, 0, 0, 0, 0, 0, 0 };

char opto_states[ 8 ] = { 0, 0, 0, 0, 0, 0, 0, 0 };


void setup()
{
  // init strings.
  sprintf(ssid, "");
  sprintf(WifiPassword, "");
  sprintf(password2, "");
  sprintf(MqttServer, "");
  sprintf(MqttID, "");
  sprintf(R1Topic, "");
  sprintf(R2Topic, "");
  sprintf(R3Topic, "");
  sprintf(R4Topic, "");
  sprintf(R5Topic, "");
  sprintf(R6Topic, "");
  sprintf(R7Topic, "");
  sprintf(R8Topic, "");
  sprintf(N1Topic, "");
  sprintf(N2Topic, "");
  sprintf(N3Topic, "");
  sprintf(N4Topic, "");
  sprintf(N5Topic, "");
  sprintf(N6Topic, "");
  sprintf(N7Topic, "");
  sprintf(N8Topic, "");
  sprintf(AsciiPassword, "");

  // set the pin modes
  pinMode( RELAY_1, OUTPUT );      
  pinMode( RELAY_2, OUTPUT );
  pinMode( RELAY_3, OUTPUT );
  pinMode( RELAY_4, OUTPUT );
  pinMode( RELAY_5, OUTPUT );
  pinMode( RELAY_6, OUTPUT );
  pinMode( RELAY_7, OUTPUT );
  pinMode( RELAY_8, OUTPUT );
  pinMode( WIFI_LED, OUTPUT );
  digitalWrite( RELAY_1, LOW );
  digitalWrite( RELAY_2, LOW );
  digitalWrite( RELAY_3, LOW );
  digitalWrite( RELAY_4, LOW );
  digitalWrite( RELAY_5, LOW );
  digitalWrite( RELAY_6, LOW );
  digitalWrite( RELAY_7, LOW );
  digitalWrite( RELAY_8, LOW );
  digitalWrite( WIFI_LED, HIGH );

  pinMode( LED_POWER, OUTPUT );
  digitalWrite( LED_POWER, HIGH );

  setOptoPinModes( INPUT );

  Serial.begin( 115200 );

  delay( 10 );

  wifi_connect();
}


void loop() {
  modeHttp();
  modeAscii();
  modeMQTT();
  serialMonitor();
  doOpto();
  if (WiFi.status() != WL_CONNECTED) wifi_connect();
}

void wifi_connect( void )
{
  unsigned int x;

  digitalWrite(WIFI_LED, HIGH);
  Serial.println("");
  nvm.begin("devantech", false);    // Note: Namespace name is limited to 15 chars
  local_IP = nvm.getUInt("IPAddress", 0);
  gateway = nvm.getUInt("GateWay", 0);
  subnet = nvm.getUInt("SubNet", 0);
  primaryDNS = nvm.getUInt("primaryDNS", 0);
  secondaryDNS = nvm.getUInt("secondaryDNS", 0);
  nvm.getString("ssid", ssid, sizeof(ssid) - 1);
  nvm.getString("WifiPassword", WifiPassword, sizeof(WifiPassword) - 1);
  strcpy(password2, "********");
  AsciiPort = nvm.getUInt("AsciiPort", 17123);
  nvm.getString("MqttServer", MqttServer, BUFSIZE - 1);
  nvm.getString("MqttID", MqttID, BUFSIZE - 1);
  nvm.getString("MqttUser", MqttUser, BUFSIZE-1);
  nvm.getString("MqttPasswd", MqttPasswd, BUFSIZE-1);
  strcpy(mqttpasswd2, "********");
  MqttPort = nvm.getUInt("MqttPort", 0);            // 0 means do not connect, normally should be 1883
  nvm.getString("R1Topic", R1Topic, BUFSIZE - 1);
  nvm.getString("R2Topic", R2Topic, BUFSIZE - 1);
  nvm.getString("R3Topic", R3Topic, BUFSIZE - 1);
  nvm.getString("R4Topic", R4Topic, BUFSIZE - 1);
  nvm.getString("R5Topic", R5Topic, BUFSIZE - 1);
  nvm.getString("R6Topic", R6Topic, BUFSIZE - 1);
  nvm.getString("R7Topic", R7Topic, BUFSIZE - 1);
  nvm.getString("R8Topic", R8Topic, BUFSIZE - 1);
  nvm.getString("N1Topic", N1Topic, BUFSIZE - 1);
  nvm.getString("N2Topic", N2Topic, BUFSIZE - 1);
  nvm.getString("N3Topic", N3Topic, BUFSIZE - 1);
  nvm.getString("N4Topic", N4Topic, BUFSIZE - 1);
  nvm.getString("N5Topic", N5Topic, BUFSIZE - 1);
  nvm.getString("N6Topic", N6Topic, BUFSIZE - 1);
  nvm.getString("N7Topic", N7Topic, BUFSIZE - 1);
  nvm.getString("N8Topic", N8Topic, BUFSIZE - 1);
  nvm.getString("AsciiPassword", AsciiPassword, BUFSIZE - 1);

  // We start by connecting to a WiFi network
  Serial.print("Connecting to ");
  Serial.print(ssid);
  Serial.println("...");

  if ((uint32_t)local_IP != 0) {
    if (!WiFi.config(local_IP, gateway, subnet, primaryDNS, secondaryDNS)) {
      Serial.println("STA Failed to configure");
    }
  }
  WiFi.mode(WIFI_STA);

  while (WiFi.status() != WL_CONNECTED) {
    WiFi.disconnect();
    delay(100);
    WiFi.begin(ssid, WifiPassword);
    for (x = 0; x < 300; x++) {
      delay(10);
      serialMonitor();
    }
  }
  digitalWrite(WIFI_LED, LOW);
  Serial.println("");
  Serial.println("WiFi connected.");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
  server.begin();
  tcpServer.close();
  tcpServer = WiFiServer(AsciiPort);
  tcpServer.begin();
  setupMQTT();
}

void setOptoPinModes( int p_mode ) {
  pinMode( OPTO_1, p_mode );
  pinMode( OPTO_2, p_mode );
  pinMode( OPTO_3, p_mode );
  pinMode( OPTO_4, p_mode );
  pinMode( OPTO_5, p_mode );
  pinMode( OPTO_6, p_mode );
  pinMode( OPTO_7, p_mode );
  pinMode( OPTO_8, p_mode );
}

void readOptoPins() {
  setOptoPinModes( INPUT );                 // Set the opto pins to inputs
  delayMicroseconds( 5 );                   // Short delay to allow voltage to settle
  opto_states[ 0 ] = digitalRead( OPTO_1 ); // Read the input states.
  opto_states[ 1 ] = digitalRead( OPTO_2 );
  opto_states[ 2 ] = digitalRead( OPTO_3 );
  opto_states[ 3 ] = digitalRead( OPTO_4 );
  opto_states[ 4 ] = digitalRead( OPTO_5 );
  opto_states[ 5 ] = digitalRead( OPTO_6 );
  opto_states[ 6 ] = digitalRead( OPTO_7 );
  opto_states[ 7 ] = digitalRead( OPTO_8 );

}

void driveOptoPins() {
  setOptoPinModes( OUTPUT );                // Set the opto pins to outputs to drive the LEDs.
  digitalWrite( OPTO_1, opto_states[ 0 ] ); // Drive the LEDs to show the opto states.
  digitalWrite( OPTO_2, opto_states[ 1 ] );
  digitalWrite( OPTO_3, opto_states[ 2 ] );
  digitalWrite( OPTO_4, opto_states[ 3 ] );
  digitalWrite( OPTO_5, opto_states[ 4 ] );
  digitalWrite( OPTO_6, opto_states[ 5 ] );
  digitalWrite( OPTO_7, opto_states[ 6 ] );
  digitalWrite( OPTO_8, opto_states[ 7 ] );

}

void doOpto() {
  digitalWrite( LED_POWER, HIGH );  // Turn the opto led power off
  readOptoPins();                   // Read the state of the optos
  driveOptoPins();                  
  digitalWrite( LED_POWER, LOW );   // Turn the opto led power on again
}
