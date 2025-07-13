#include <SPI.h>
#include <Ethernet.h>

// Thay thế các địa chỉ MAC và IP bằng các địa chỉ thực tế của mạng của bạn
byte mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };

#define W5500_CS 5

#include <PubSubClient.h>
const char* mqtt_server = "test.mosquitto.org";
const char* mqtt_user = "";
const char* mqtt_pass = "";

EthernetClient ethClient;
PubSubClient client(ethClient);

void callback(char* topic, byte* payload, unsigned int length) {
  Serial.print("TOPIC: ");
  String TOPIC = String(topic);
  Serial.print(TOPIC);
  Serial.print("   PAYLOAD: ");
  String PAYLOAD;
  for (uint8_t i = 0; i < length; i++) {
    PAYLOAD += char(payload[i]);
  }
  Serial.println(PAYLOAD);

  if (TOPIC.lastIndexOf("eenju/inTopic") != -1) {
    Serial.print("ok có: ");
    Serial.println(PAYLOAD);
  }
}

void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Attempt to connect
    //if (client.connect("arduinoClient", mqtt_user, mqtt_pass)) {
    if (client.connect("arduinoClient")) {
      Serial.println("connected");
      // Once connected, publish an announcement...
      client.publish("eenju/outTopic", "hello world");
      // ... and resubscribe
      client.subscribe("eenju/inTopic");
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}

void setup() {
  Serial.begin(115200);

  // Khởi tạo giao tiếp SPI
  SPI.begin();
  Ethernet.init(W5500_CS);
  if (Ethernet.begin(mac) == 0) {
    Serial.println("Failed to configure Ethernet using DHCP");
    // No point in carrying on, so do nothing forevermore:
    while (true)
      ;
  }
  // In các thông tin mạng
  Serial.print("My IP address: ");
  Serial.println(Ethernet.localIP());
  Serial.print("Subnet mask: ");
  Serial.println(Ethernet.subnetMask());
  Serial.print("Gateway: ");
  Serial.println(Ethernet.gatewayIP());
  Serial.print("DNS server: ");
  Serial.println(Ethernet.dnsServerIP());

  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);
}

void loop() {
  if (!client.connected()) {
    reconnect();
  }
  client.loop();

  static uint32_t time_run_static = millis();
  if (millis() - time_run_static > 5000) {
    time_run_static = millis();
    static int dem = 0;
    dem++;
    String datasend = "dem:" + String(dem);
    client.publish("eenju/outdem", datasend.c_str());
    Serial.println(datasend);
  }
}