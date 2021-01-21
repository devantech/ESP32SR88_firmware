
#include <WiFi.h>
#include <PubSubClient.h>

// Update these with values suitable for your network.


WiFiClient espClient;
PubSubClient MQTTclient(espClient);
long lastMsg = 0;
char msg[50];
int value = 0;
int MQTTconnectionTimer = 1;
int MQTTPortInUse = 0;

void setupMQTT() {
  if (MqttPort == 0) return;                  // do not try to connect if port is zero
  MQTTclient.setServer(MqttServer, MqttPort);
  MQTTclient.setCallback(callback);
  MQTTconnectionTimer = millis() - 5000;
  MQTTPortInUse = MqttPort;
}

void callback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");
  for (int i = 0; i < length; i++) {
    Serial.print((char)payload[i]);
  }
  Serial.println();

  if (strcmp(topic, R1Topic) == 0) {
    mqttRelayAction( (char) payload[0], RELAY_1 );
  }
  if (strcmp(topic, R2Topic) == 0) {
    mqttRelayAction( (char) payload[0], RELAY_2 );
  }
  if (strcmp(topic, R3Topic) == 0) {
    mqttRelayAction( (char) payload[0], RELAY_3 );
  }
  if (strcmp(topic, R4Topic) == 0) {
    mqttRelayAction( (char) payload[0], RELAY_4 );
  }
  if (strcmp(topic, R5Topic) == 0) {
    mqttRelayAction( (char) payload[0], RELAY_5 );
  }
  if (strcmp(topic, R6Topic) == 0) {
    mqttRelayAction( (char) payload[0], RELAY_6 );
  }
  if (strcmp(topic, R7Topic) == 0) {
    mqttRelayAction( (char) payload[0], RELAY_7 );
  }
  if (strcmp(topic, R8Topic) == 0) {
    mqttRelayAction( (char) payload[0], RELAY_8 );
  }
}

void mqttRelayAction(char state, int relay) {
  if (state == '1') digitalWrite(relay, HIGH);
  else digitalWrite(relay, LOW);
}

void reconnect() {
  if (WiFi.status() != WL_CONNECTED) return; //WIFI disconnected so don't attempt reconnect
  Serial.println("Attempting MQTT connection...");
    if (MQTTclient.connect(MqttID,MqttUser,MqttPasswd)) {
    printState();

    // subscribe
    MQTTclient.subscribe(R1Topic);
    MQTTclient.subscribe(R2Topic);
    MQTTclient.subscribe(R3Topic);
    MQTTclient.subscribe(R4Topic);
    MQTTclient.subscribe(R5Topic);
    MQTTclient.subscribe(R6Topic);
    MQTTclient.subscribe(R7Topic);
    MQTTclient.subscribe(R8Topic);
  } else {
    printState();
    Serial.println(" try again in 10 seconds");
  }
}



void modeMQTT() {

  static int x = 0;

  if (WiFi.status() != WL_CONNECTED) return;


  if (MQTTPortInUse != MqttPort) {
    setupMQTT();
    return;
  }

  if (MQTTclient.loop() == true) {
    switch (x) {
      case 0:
        mqttCheckOptoState( x, N1Topic );
        break;
      case 1:
        mqttCheckOptoState( x, N2Topic );
        break;
      case 2:
        mqttCheckOptoState( x, N3Topic );
        break;
      case 3:
        mqttCheckOptoState( x, N4Topic );
        break;
      case 4:
        mqttCheckOptoState( x, N5Topic );
        break;
      case 5:
        mqttCheckOptoState( x, N6Topic );
        break;
      case 6:
        mqttCheckOptoState( x, N7Topic );
        break;
      case 7:
        mqttCheckOptoState( x, N8Topic );
        break;
      default:
        x = 0;
        break;
    }

    if ( ++x > 7 ) {
      x = 0;   // bump to next input to process
    }

  } else {
    if (MqttPort == 0) return;                  // do not try to connect if port is zero
    if (millis() - MQTTconnectionTimer < 10000) return;
    printState();
    reconnect();
    MQTTconnectionTimer = millis();
  }

}

void mqttCheckOptoState( int opto, char topic[] ) {

  if ( mqtt_opto_states[ opto ] != opto_states[ opto ] ) {  // if its changed ..
    mqtt_opto_states[ opto ] = opto_states[ opto ];       // store new state
    char Msg[] = "0";
    msg[0] = !mqtt_opto_states[ opto ] + 0x30;            // generate MQTT message
    MQTTclient.publish(topic, msg, true);                 // and publish it
    Serial.print("Message published [");
    Serial.print(topic);
    Serial.print("] ");
    Serial.println(msg[0]);
  }

}

void printState() {

  switch ( MQTTclient.state() ) {
    case MQTT_CONNECTION_TIMEOUT:
      Serial.println( "Connection to MQTT server timed out." );
      break;
    case MQTT_CONNECTION_LOST:
      Serial.println( "Connection to MQTT server was lost." );
      break;
    case MQTT_CONNECT_FAILED:
      Serial.println( "Connection to MQTT server failed." );
      break;
    case MQTT_DISCONNECTED:
      Serial.println( "Disconnected from the MQTT server." );
      break;
    case MQTT_CONNECTED:
      Serial.println( "Connected to MQTT server." );
      break;
    case MQTT_CONNECT_BAD_PROTOCOL:
      Serial.println("server doesn't support the requested version of MQTT.");
      break;
    case MQTT_CONNECT_BAD_CLIENT_ID:
      Serial.println("MQTT server rejected the client identifier.");
      break;
    case MQTT_CONNECT_UNAVAILABLE:
      Serial.println("server was unable to accept the connection.");
      break;
    case MQTT_CONNECT_BAD_CREDENTIALS:
      Serial.println("MQTT the username/password were rejected.");
      break;
    case MQTT_CONNECT_UNAUTHORIZED:
      Serial.println("MQTT client was not authorized to connect.");
      break;
  }

}
