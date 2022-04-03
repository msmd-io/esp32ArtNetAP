#include <WiFi.h>
#include <WebServer.h>
#include "esp_wifi.h"

const char *ssid = "Art-Net";
const char *passphrase = "esp32artnetAP";

IPAddress local_IP(2,0,0,1);
IPAddress gateway(2,0,0,1);
IPAddress subnet(255,0,0,0);

WebServer server(80);

void handleRoot() {
  wifi_sta_list_t wifi_sta_list;
  tcpip_adapter_sta_list_t adapter_sta_list;
  memset(&wifi_sta_list, 0, sizeof(wifi_sta_list));
  memset(&adapter_sta_list, 0, sizeof(adapter_sta_list));
 
  esp_wifi_ap_get_sta_list(&wifi_sta_list);
  tcpip_adapter_get_sta_list(&wifi_sta_list, &adapter_sta_list);
  
  char html[512] = "Hello from ESP32";
  
  for (int i=0; i < adapter_sta_list.num; i++) {
    tcpip_adapter_sta_info_t station = adapter_sta_list.sta[i];
    char ipBuffer[16];
    sprintf(ipBuffer, IPSTR, IP2STR(&(station.ip)));
    sprintf(html, "%s\nNode IP %s" , html, ipBuffer);
  }
  server.send(200, "text/plain", html);
}

void setup()
{
  Serial.begin(115200);
  Serial.println();

  Serial.print("Setting soft-AP configuration ... ");
  WiFi.mode(WIFI_AP_STA);
  Serial.println(WiFi.softAPConfig(local_IP, gateway, subnet) ? "Ready" : "Failed!");

  Serial.print("Setting soft-AP ... ");
  Serial.println(WiFi.softAP(ssid,passphrase) ? "Ready" : "Failed!");
  //WiFi.softAP(ssid);
  //WiFi.softAP(ssid, passphrase, channel, ssdi_hidden, max_connection)
  
  Serial.print("Soft-AP IP address = ");
  Serial.println(WiFi.softAPIP());

  server.on("/", handleRoot);
  server.begin();
  Serial.println("HTTP server started");
}

void loop() {
  server.handleClient();
  delay(2);
}
