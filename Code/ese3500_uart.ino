#include <WiFi.h>
#include <WebServer.h>

const char* SSID     = "Ashwin's iPhone (2)";
const char* PASSWORD = "ashwinshotspot";

WebServer server(80);

float voltage = 0;
int   current = 0;
int   power   = 0;
int   pan     = -1;
int   tilt    = -1;

void handleData() {
  char buf[160];
  snprintf(buf, sizeof(buf),
    "{\"voltage\":%.2f,\"current\":%d,\"power\":%d,\"pan\":%s,\"tilt\":%s}",
    voltage, current, power,
    pan  >= 0 ? String(pan).c_str()  : "null",
    tilt >= 0 ? String(tilt).c_str() : "null");

  server.sendHeader("Access-Control-Allow-Origin", "*");
  server.send(200, "application/json", buf);
}

void setup() {
  Serial.begin(115200);
  delay(3000);
  Serial.println("Booting...");

  Serial1.begin(9600, SERIAL_8N1, 11, -1);

  WiFi.begin(SSID, PASSWORD);
  while (WiFi.status() != WL_CONNECTED) delay(500);
  Serial.print("IP: ");
  Serial.println(WiFi.localIP());

  server.on("/data", HTTP_GET, handleData);
  server.begin();
}

void loop() {
  if (Serial1.available()) {
    String line = Serial1.readStringUntil('\n');
    line.trim();

    int v, i, pa;
    long p;
    if (sscanf(line.c_str(), "V:%d,I:%d,P:%ld,PAN:%d", &v, &i, &p, &pa) == 4) {
      voltage = v / 1000.0;
      current = i;
      power   = (int)(voltage * i);
      pan     = pa;
      Serial.printf("V:%.2fV  I:%dmA  P:%dmW  PAN:%d°\n", voltage, current, power, pan);
    }
  }

  server.handleClient();
}